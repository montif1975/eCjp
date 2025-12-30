#include "ecjp.h"

#include "sys/stat.h"
#include <unistd.h>

#ifdef ECJP_RUN_ON_PC
    #define ecjp_fprintf(format, ...)    fprintf(stdout, format, __VA_ARGS__)
    #define ecjp_fprint(format)          fprintf(stdout, format)
#else
    #define ecjp_fprintf(format, ...)
    #define ecjp_fprint(format)
#endif

void usage(char *prog_name)
{
    ecjp_fprintf("Usage: %s [filename]\n", prog_name);
}

int main(int argc, char *argv[])
{
    ecjp_return_code_t ret;
    ecjp_check_result_t results;
    char *ptr;
    struct stat strstat;
    ecjp_key_elem_t *key_list = NULL;
    ecjp_outdata_t out;
    int index = 0;

    results.err_pos = -1;
    results.num_keys = 0;
    results.struct_type = ECJP_ST_NULL;
    
    ret = 0;

    // check arguments and open test files
    if(argc != 2) {
        usage(argv[0]);
        return -1;
    }
    if(argc == 2) {
        ecjp_fprintf("\nTesting input file: %s\n", argv[1]);
        memset(&strstat, 0, sizeof(struct stat));
        ret = stat(argv[1], &strstat);
        if (ret == 0)
        {
            long file_size = strstat.st_size;
            ptr = (char *)malloc(file_size + 1);
            if (ptr != NULL) {
                FILE *f = fopen(argv[1], "r");
                if (f != NULL) {
                    size_t read_bytes = fread(ptr, 1, file_size, f);
                    ptr[read_bytes] = '\0';
                    fclose(f);
                    ecjp_fprintf("\nTesting JSON file (%s) of size %ld bytes:\n", argv[1], file_size);
                    ret = ecjp_check_and_load(ptr,&key_list,&results,1);
                    if (ret != ECJP_NO_ERROR) {
                        ecjp_fprintf("ecjp_check_syntax() on JSON file: FAILED with error code: %d\n", ret);
                        if (results.err_pos >= 0) {
                            ecjp_fprintf("ecjp_check_syntax(): Error position: %d\n", results.err_pos);
                            if(results.err_pos >= 1024) {
                                ecjp_fprint("Error position is beyond 1024 characters, showing context around error:\n");
                                int start_pos = results.err_pos - 512;
                                if (start_pos < 0) start_pos = 0;
                                int end_pos = results.err_pos + 512;
                                if (end_pos > read_bytes) end_pos = read_bytes;
                                char context[1025];
                                memset(context, 0, sizeof(context));
                                strncpy(context, ptr + start_pos, end_pos - start_pos);
                                ecjp_show_error(context, results.err_pos - start_pos);
                            } else {
                                ecjp_show_error(ptr, results.err_pos);
                            }
                        }
                        free(ptr);
                        ptr = NULL;
                        return -1;
                    }
                    else {
                        ecjp_fprint("ecjp_check_syntax() on JSON file: SUCCEEDED.\n");
                        ecjp_fprintf("ecjp_check_syntax() - num. keys found = %d, struct type = %d.\n",results.num_keys,results.struct_type);
                        if ((results.struct_type == ECJP_ST_OBJ) && (results.num_keys != 0)) {
                            if (key_list != NULL) {
                                ecjp_print_keys(ptr, key_list);
                                ecjp_free_key_list(&key_list);
                            }    
                        } else if (results.struct_type == ECJP_ST_ARRAY) {
                            out.error_code = ECJP_NO_ERROR;
                            out.value = (char *)malloc(ECJP_MAX_ARRAY_ELEM_LEN);
                            out.value_size = ECJP_MAX_ARRAY_ELEM_LEN;
                            if (out.value == NULL) {
                                ecjp_fprint("Memory allocation failed for array element value buffer\n");
                                return -1;
                            }
                            ecjp_fprint("\nReading array elements:\n");
                            while (ecjp_read_array_element(ptr,index,&out) == ECJP_NO_ERROR) {
                                ecjp_fprintf("Array element #%d read successfully.\n",index);
                                ecjp_fprintf("Type = %d, Value = %s\n", out.type, (char *)out.value);
                                out.value_size = ECJP_MAX_ARRAY_ELEM_LEN;
                                index++;
                            }
                            free(out.value);
                            out.value = NULL;
                        }
                    }
                    free(ptr);
                    ptr = NULL;
                }
                else {
                    ecjp_fprintf("Failed to open file %s\n", argv[1]);
                    free(ptr);
                    ptr = NULL;
                    return -1;
                }
            }
            else {
                ecjp_fprint("Memory allocation failed for JSON file\n");
                return -1;
            }   
        } else {
            ecjp_fprintf("stat() failed for file %s\n", argv[1]);
            return -1;
        }
    }

    return 0;
}


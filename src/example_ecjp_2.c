//#include "include/ecjp.h"
#include "ecjp.h"

#include "sys/stat.h"
#include <unistd.h>

#define TEST_KEY_FIND

void usage(char *prog_name)
{
    fprintf(stderr, "Usage: %s [filename]\n", prog_name);
}

int main(int argc, char *argv[])
{
    int major, minor, patch;
    int array_index;
    ecjp_return_code_t ret;
    ecjp_check_result_t results;
    char *ptr;
    struct stat strstat;
    ecjp_outdata_t out;
    ecjp_key_elem_t *key_list = NULL;

    int err_pos = -1;
    results.err_pos = -1;
    results.num_keys = 0;
    results.struct_type = ECJP_ST_NULL;
    
    major = minor = patch = 0;
    ret = ecjp_get_version(&major, &minor, &patch);
    if (ret != ECJP_NO_ERROR) {
        fprintf(stderr, "ecjp_get_version() failed with error code: %d\n", ret);
    }
    else {
        fprintf(stdout, "\nUsing eCjp version: %d.%d.%d\n", major, minor, patch);
    }

    // check arguments and open test files
    if(argc != 2) {
        usage(argv[0]);
        return -1;
    }
    if(argc == 2) {
        fprintf(stdout, "\nUsing input file: %s\n", argv[1]);
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
                    fprintf(stdout, "\nUsing JSON file (%s) of size %ld bytes:\n", argv[1], file_size);
                    ret = ecjp_check_syntax(ptr,&results);
                    if (ret != ECJP_NO_ERROR) {
                        fprintf(stderr, "ecjp_check_syntax() on JSON file: FAILED with error code: %d\n", ret);
                        if (results.err_pos >= 0) {
                            fprintf(stderr, "ecjp_check_syntax(): Error position: %d\n", results.err_pos);
                            if(results.err_pos >= 1024) {
                                fprintf(stderr, "Error position is beyond 1024 characters, showing context around error:\n");
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
                        fprintf(stdout, "ecjp_check_syntax() on JSON file: SUCCEEDED.\n");
                        if (results.struct_type == ECJP_ST_OBJ) {
                            fprintf(stdout, "root structure is an OBJECT.\n");
                            ret = ecjp_load(ptr,&key_list,&results,ECJP_MAX_NESTED_LEVEL);
                            if (ret != ECJP_NO_ERROR) {
                                fprintf(stderr, "ecjp_load() on JSON file: FAILED with error code: %d\n", ret);
                                free(ptr);
                                ptr = NULL;
                                ecjp_free_key_list(&key_list);
                                return -1;
                            } else {
                                if (key_list != NULL && results.num_keys != 0) {
                                    ret = ecjp_get_keys_and_value(ptr, key_list);
                                    if (ret == ECJP_NO_MORE_KEY) {
                                        // all keys processed
                                        ret = ECJP_NO_ERROR;
                                    }
                                }
                            }
                        } else if (results.struct_type == ECJP_ST_ARRAY) {
                            fprintf(stdout, "root structure is an ARRAY.\n");
                            array_index = 0;
                            out.error_code = ECJP_NO_ERROR;
                            out.value = (char *)malloc(ECJP_MAX_ARRAY_ELEM_LEN);
                            out.value_size = ECJP_MAX_ARRAY_ELEM_LEN;
                            if (out.value == NULL) {
                                fprintf(stderr, "Memory allocation failed for array element value buffer\n");
                                ecjp_free_key_list(&key_list);
                                return -1;
                            }
                            fprintf(stdout, "\nReading array elements:\n");
                            while (ecjp_read_array_element(ptr,array_index,&out) == ECJP_NO_ERROR) {
                                fprintf(stdout, "Array element #%d read successfully.\n",array_index);
                                fprintf(stdout, "Type = %d, Value = %s\n", out.type, (char *)out.value);
                                out.value_size = ECJP_MAX_ARRAY_ELEM_LEN;
                                array_index++;
                            }
                            free(out.value);
                            out.value = NULL;
                        } else {
                            fprintf(stdout, "Top-level structure is NULL or UNDEFINED.\n");
                        }
                    }
                    free(ptr);
                    ptr = NULL;
                } else {
                    fprintf(stderr, "Failed to open file %s\n", argv[1]);
                    free(ptr);
                    ptr = NULL;
                    ret = -1;
                }
            } else {
                fprintf(stderr, "Memory allocation failed for JSON file\n");
                ret = -1;
            } 
        } else {
            fprintf(stderr, "stat() failed for file %s\n", argv[1]);
            ret = -1;
        }
    }

    ecjp_free_key_list(&key_list);

    return ret;
}


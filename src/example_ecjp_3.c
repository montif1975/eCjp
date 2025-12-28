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
    ecjp_return_code_t ret;
    ecjp_check_result_t results;
    char *ptr;
    struct stat strstat;
    ecjp_item_elem_t *item_list = NULL;

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
                        ret = ecjp_check_and_load_2(ptr,&item_list,&results);
                        if (ret != ECJP_NO_ERROR) {
                            fprintf(stderr, "ecjp_check_and_load_2() on JSON file: FAILED with error code: %d\n", ret);
                            free(ptr);
                            ptr = NULL;
                            ecjp_free_item_list(&item_list);
                            return -1;
                        } else {
                            while (item_list != NULL) {
                                fprintf(stdout, "Item read successfully.\n");
                                fprintf(stdout, "Type = %s, Value = %s\n", ecjp_type[item_list->item.type], (char *)item_list->item.value);
                                item_list = item_list->next;
                            }
                            ecjp_free_item_list(&item_list);
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

    ecjp_free_item_list(&item_list);

    return ret;
}


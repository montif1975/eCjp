//#include "include/ecjp.h"
#include "ecjp.h"

#include "sys/stat.h"
#include <unistd.h>

#define TEST_KEY_FIND

void print_and_free_key_list(ecjp_key_elem_t **key_list)
{
    ecjp_key_elem_t *current = *key_list;
    ecjp_key_elem_t *next;

    while (current != NULL) {
        if(current->key.type != ECJP_TYPE_UNDEFINED) {
            fprintf(stdout, "Key at pos %u, length %u, type %u\n",
                    current->key.start_pos,
                    current->key.length,
                    current->key.type);
        }
        next = current->next;
        current = next;
    }
    ecjp_free_key_list(key_list);
    return;
}

void print_all_key_and_value(char *ptr, ecjp_key_elem_t *key_list)
{
    ecjp_key_elem_t *current = key_list;
    ecjp_key_elem_t *next;
    int len;
    char buffer[ECJP_MAX_KEY_LEN];
    char *ptr_tmp;
    void *value;

    value = malloc(256);
    if (value == NULL) {
        fprintf(stdout, "Memory allocation failed in %s\n", __FUNCTION__);
        return;;
    }

    while (current != NULL) {
        ptr_tmp = ptr;
        if(current->key.type != ECJP_TYPE_UNDEFINED) {
#if 0
            fprintf(stdout, "Key at pos %u, length %u, type %u\n",
                    current->key.start_pos,
                    current->key.length,
                    current->key.type);
#endif
            len = (current->key.length < ECJP_MAX_KEY_LEN) ? current->key.length : (ECJP_MAX_KEY_LEN - 1);
            ptr_tmp += current->key.start_pos;
            memset(buffer, 0, sizeof(buffer));
            memset(value, 0, 256);
            strncpy(buffer, ptr_tmp, len);
            fprintf(stdout, "  Key: %s -- ", buffer);
            // print value
            if (ecjp_get_key_and_value(ptr, buffer, &key_list, value, 256) != ECJP_TYPE_UNDEFINED) {
                fprintf(stdout, "Value: %s\n", (char *)value);
            } else {    
                fprintf(stdout, "Value: <not found>\n");
            }   
        }
        next = current->next;
        current = next;
    }
    free(value);    

    return;
}

void print_all_keys(char *ptr, ecjp_key_elem_t *key_list)
{
    ecjp_outdata_t retval;
    ecjp_return_code_t ret;

    memset(&retval,0,sizeof(retval));
    retval.value = malloc(ECJP_MAX_KEY_LEN);
    retval.value_size = ECJP_MAX_KEY_LEN;
    ret = ECJP_NO_ERROR;

    while(ret != ECJP_NO_MORE_KEY) {
        ret = ecjp_get_keys(ptr,NULL,&key_list,&retval);
        fprintf(stdout,
                "Find key: %s [ret=%d type=%d last_pos=%d]\n",
                (char *)retval.value,
                retval.error_code,
                retval.type,
                retval.last_pos);
        // reset retval but not the last position
        retval.error_code = ECJP_NO_ERROR;
        retval.type = ECJP_TYPE_UNDEFINED;
        memset(retval.value,0,retval.value_size);
    }
    
    free(retval.value);

    return;
}

void print_keys_and_value(char *ptr,ecjp_key_elem_t *key_list)
{
    ecjp_outdata_t out_get;
    ecjp_outdata_t out_read;
    ecjp_indata_t in;
    ecjp_return_code_t ret;

    memset(&out_get,0,sizeof(out_get));
    out_get.value = malloc(ECJP_MAX_KEY_LEN);
    out_get.value_size = ECJP_MAX_KEY_LEN;
    memset(out_get.value,0,out_get.value_size);

    memset(&out_read,0,sizeof(out_read));
    out_read.value = malloc(ECJP_MAX_KEY_LEN);
    out_read.value_size = ECJP_MAX_KEY_LEN;
    memset(out_read.value,0,out_read.value_size);

    memset(&in,0,sizeof(in));

    ret = ECJP_NO_ERROR;

    do {
        ret = ecjp_get_keys(ptr,NULL,&key_list,&out_get);
        if (ret != ECJP_NO_MORE_KEY) {
#if 0            
            fprintf(stdout,
                    "Find key: %s [error_code=%d type=%d last_pos=%d]\n",
                    (char *)out_get.value,
                    out_get.error_code,
                    out_get.type,
                    out_get.last_pos);
#endif
            // read value for this key
            in.length = out_get.length;
            in.type = out_get.type;
            in.pos = out_get.last_pos;
            strncpy(in.key,out_get.value,out_get.value_size);
            ecjp_read_key(ptr,&in,&out_read);
            if (out_read.error_code == ECJP_NO_ERROR || out_read.error_code == ECJP_NO_SPACE_IN_BUFFER_VALUE) {
                fprintf(stdout,
                        "Key %s value: %s\n",
                        in.key,
                        (char *)out_read.value);
            }
            // reset out_get but not the last position
            out_get.error_code = ECJP_NO_ERROR;
            out_get.type = ECJP_TYPE_UNDEFINED;
            memset(out_get.value,0,out_get.value_size);
            // reset out_read
            out_read.error_code = ECJP_NO_ERROR;
            out_read.last_pos = 0;
            out_read.length = 0;
            out_read.type = ECJP_TYPE_UNDEFINED;
            memset(out_read.value,0,out_read.value_size);
            // reset in
            memset(&in,0,sizeof(in));   
        }        
    } while(ret != ECJP_NO_MORE_KEY);
    
    free(out_get.value);
    free(out_read.value);

    return;
}

void print_all_specific_key(char *ptr,ecjp_key_elem_t *key_list)
{
    ecjp_outdata_t retval;
    ecjp_return_code_t ret;
    char query[]="CFG";

    memset(&retval,0,sizeof(retval));
    retval.value = malloc(64);
    retval.value_size = 64;
    ret = ECJP_NO_ERROR;

    do {
        ret = ecjp_get_keys(ptr,query,&key_list,&retval);
        if (ret != ECJP_NO_MORE_KEY) {
            fprintf(stdout,
                    "Find key: %s [ret=%d type=%d last_pos=%d]\n",
                    query,
                    retval.error_code,
                    retval.type,
                    retval.last_pos);
            // reset retval but not the last position
            retval.error_code = ECJP_NO_ERROR;
            retval.type = ECJP_TYPE_UNDEFINED;
            memset(retval.value,0,retval.value_size);
        }
    } while (ret != ECJP_NO_MORE_KEY);
    
    free(retval.value);

    return;
}

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
    unsigned char level;
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
                        level = 1;
                        fprintf(stdout, "ecjp_check_syntax() on JSON file: SUCCEEDED.\n");
                        if (results.struct_type == ECJP_ST_OBJ) {
                            fprintf(stdout, "Level %d structure is an OBJECT.\n", level);
                            ret = ecjp_load(ptr,&key_list,&results,level);
                            if (ret != ECJP_NO_ERROR) {
                                fprintf(stderr, "ecjp_load() on JSON file: FAILED with error code: %d\n", ret);
                                free(ptr);
                                ptr = NULL;
                                return -1;
                            } else {
                                if (key_list != NULL && results.num_keys != 0) {
                                    ecjp_get_keys_and_value(ptr, key_list);
                                }
                            }
                        } else if (results.struct_type == ECJP_ST_ARRAY) {
                            fprintf(stdout, "Level %d structure is an ARRAY.\n", level);
                            array_index = 0;
                            out.error_code = ECJP_NO_ERROR;
                            out.value = (char *)malloc(ECJP_MAX_ARRAY_ELEM_LEN);
                            out.value_size = ECJP_MAX_ARRAY_ELEM_LEN;
                            if (out.value == NULL) {
                                fprintf(stderr, "Memory allocation failed for array element value buffer\n");
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
//                        fprintf(stdout, "ecjp_check_syntax() - num. keys found = %d, struct type = %d.\n",results.num_keys,results.struct_type);
                    }
                    free(ptr);
                    ptr = NULL;
                }
                else {
                    fprintf(stderr, "Failed to open file %s\n", argv[1]);
                    free(ptr);
                    ptr = NULL;
                    return -1;
                }
            }
            else {
                fprintf(stderr, "Memory allocation failed for JSON file\n");
                return -1;
            }   
        } else {
            fprintf(stderr, "stat() failed for file %s\n", argv[1]);
            return -1;
        }
    }

    return 0;
}


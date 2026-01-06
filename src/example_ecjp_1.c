/*
BSD 3-Clause License

Copyright (c) 2025, Alfredo Montini

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its
   contributors may be used to endorse or promote products derived from
   this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

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

#define TEST_KEY_FIND


#ifdef ECJP_TOKEN_LIST

int main(int argc, char *argv[])
{
    ecjp_fprint("This example is for key list implementation. Compile without ECJP_TOKEN_LIST defined.\n");
    return -1;
}
#else

void print_and_free_key_list(ecjp_key_elem_t **key_list)
{
    ecjp_key_elem_t *current = *key_list;
    ecjp_key_elem_t *next;

    while (current != NULL) {
        if(current->key.type != ECJP_TYPE_UNDEFINED) {
            ecjp_fprintf("Key at pos %u, length %u, type %u\n",
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

void print_all_keys(char *ptr, ecjp_key_elem_t *key_list)
{
    ecjp_outdata_t retval;
    ecjp_return_code_t ret;
    unsigned short int start = 0;

    memset(&retval,0,sizeof(retval));
    retval.value = malloc(ECJP_MAX_KEY_LEN);
    retval.value_size = ECJP_MAX_KEY_LEN;
    ret = ECJP_NO_ERROR;

    while(ret != ECJP_NO_MORE_KEY) {
        ret = ecjp_get_key(ptr,NULL,&key_list,start,&retval);
        ecjp_fprintf("Find key: %s [ret=%d type=%d last_pos=%d]\n",
                    (char *)retval.value,
                    retval.error_code,
                    retval.type,
                    retval.last_pos);
        start = retval.last_pos;
        // reset retval
        retval.error_code = ECJP_NO_ERROR;
        retval.type = ECJP_TYPE_UNDEFINED;
        retval.last_pos = 0;
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
    unsigned short int start = 0;

    memset(&out_get,0,sizeof(out_get));
    out_get.value = malloc(ECJP_MAX_KEY_LEN);
    out_get.value_size = ECJP_MAX_KEY_LEN;
    memset(out_get.value,0,out_get.value_size);

    memset(&out_read,0,sizeof(out_read));
    out_read.value = malloc(ECJP_MAX_KEY_VALUE_LEN);
    out_read.value_size = ECJP_MAX_KEY_VALUE_LEN;
    memset(out_read.value,0,out_read.value_size);

    memset(&in,0,sizeof(in));

    ret = ECJP_NO_ERROR;

    do {
        ret = ecjp_get_key(ptr,NULL,&key_list,start,&out_get);
        if (ret != ECJP_NO_MORE_KEY) {
            // read value for this key
            in.length = out_get.length;
            in.type = out_get.type;
            in.pos = out_get.last_pos;
            start = out_get.last_pos;
            strncpy(in.key,out_get.value,out_get.value_size);
            ecjp_read_key(ptr,&in,&out_read);
            if (out_read.error_code == ECJP_NO_ERROR || out_read.error_code == ECJP_NO_SPACE_IN_BUFFER_VALUE) {
                ecjp_fprintf("Key %s value: %s\n",
                            in.key,
                            (char *)out_read.value);
            }
            // reset out_get 
            out_get.error_code = ECJP_NO_ERROR;
            out_get.type = ECJP_TYPE_UNDEFINED;
            out_get.last_pos = 0;
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

void usage(char *prog_name)
{
    ecjp_fprintf("Usage: %s [filename]\n", prog_name);
}

int main(int argc, char *argv[])
{
    int major, minor, patch;
    char version_string[16];
    ecjp_return_code_t ret;
    ecjp_check_result_t results;
    char *ptr;
    struct stat strstat;
    ecjp_key_elem_t *key_list = NULL;

    results.err_pos = -1;
    results.num_keys = 0;
    results.struct_type = ECJP_ST_NULL;
    results.memory_used = 0;
    
    ret = ecjp_dummy();
    if (ret != ECJP_NO_ERROR) {
        ecjp_fprintf("ecjp_dummy() failed with error code: %d\n", ret);
    }
    else {
        ecjp_fprint("ecjp_dummy() succeeded.\n");
    }

    major = minor = patch = 0;
    ret = ecjp_get_version(&major, &minor, &patch);
    if (ret != ECJP_NO_ERROR) {
        ecjp_fprintf("ecjp_get_version() failed with error code: %d\n", ret);
    }
    else {
        ecjp_fprintf("ecjp_get_version() succeeded. Version: %d.%d.%d\n", major, minor, patch);
    }

    memset(version_string, 0, sizeof(version_string));
    ret = ecjp_get_version_string(version_string, sizeof(version_string));
    if (ret != ECJP_NO_ERROR) {
        ecjp_fprintf("ecjp_get_version_string() failed with error code: %d\n", ret);
    }
    else {
        ecjp_fprintf("ecjp_get_version_string() succeeded. Version string: %s\n", version_string);
    }

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
                    ret = ecjp_check_and_load(ptr,&key_list,&results,3);
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
                        if (results.num_keys != 0) {
                            if (key_list != NULL) {
                                    ecjp_print_keys(ptr, key_list);
#ifdef TEST_KEY_FIND
                                    // Test key finding
                                    print_keys_and_value(ptr,key_list);
//                                  print_all_keys(ptr,key_list);
#endif
                                    print_and_free_key_list(&key_list);
                                }    
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

#endif 


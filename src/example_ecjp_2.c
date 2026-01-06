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

void usage(char *prog_name)
{
    ecjp_fprintf("Usage: %s [filename]\n", prog_name);
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

    results.err_pos = -1;
    results.num_keys = 0;
    results.struct_type = ECJP_ST_NULL;
    results.memory_used = 0;

    major = minor = patch = 0;
    ret = ecjp_get_version(&major, &minor, &patch);
    if (ret != ECJP_NO_ERROR) {
        ecjp_fprintf("ecjp_get_version() failed with error code: %d\n", ret);
    }
    else {
        ecjp_fprintf("\nUsing eCjp version: %d.%d.%d\n", major, minor, patch);
    }

    // check arguments and open test files
    if(argc != 2) {
        usage(argv[0]);
        return -1;
    }
    if(argc == 2) {
        ecjp_fprintf("\nUsing input file: %s\n", argv[1]);
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
                    ecjp_fprintf("\nUsing JSON file (%s) of size %ld bytes:\n", argv[1], file_size);
                    ret = ecjp_check_syntax(ptr,&results);
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
                        if (results.struct_type == ECJP_ST_OBJ) {
                            ecjp_fprint("root structure is an OBJECT.\n");
                            results.err_pos = -1;
                            results.num_keys = 0;
                            results.struct_type = ECJP_ST_NULL;
                            results.memory_used = 0;                            
                            ret = ecjp_load(ptr,&key_list,&results,ECJP_MAX_NESTED_LEVEL);
                            if (ret != ECJP_NO_ERROR) {
                                ecjp_fprintf("ecjp_load() on JSON file: FAILED with error code: %d\n", ret);
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
                                    ecjp_free_key_list(&key_list);
                                }
                            }
                        } else if (results.struct_type == ECJP_ST_ARRAY) {
                            ecjp_fprint("root structure is an ARRAY.\n");
                            array_index = 0;
                            out.error_code = ECJP_NO_ERROR;
                            out.value = (char *)malloc(ECJP_MAX_ARRAY_ELEM_LEN);
                            out.value_size = ECJP_MAX_ARRAY_ELEM_LEN;
                            if (out.value == NULL) {
                                ecjp_fprint("Memory allocation failed for array element value buffer\n");
                                return -1;
                            }
                            ecjp_fprint("\nReading array elements:\n");
                            while (ecjp_read_array_element(ptr,array_index,&out) == ECJP_NO_ERROR) {
                                ecjp_fprintf("Array element #%d read successfully.\n",array_index);
                                ecjp_fprintf("Type = %d, Value = %s\n", out.type, (char *)out.value);
                                out.value_size = ECJP_MAX_ARRAY_ELEM_LEN;
                                array_index++;
                            }
                            free(out.value);
                            out.value = NULL;
                        } else {
                            ecjp_fprint("Top-level structure is NULL or UNDEFINED.\n");
                        }
                    }
                    free(ptr);
                    ptr = NULL;
                } else {
                    ecjp_fprintf("Failed to open file %s\n", argv[1]);
                    free(ptr);
                    ptr = NULL;
                    ret = -1;
                }
            } else {
                ecjp_fprint("Memory allocation failed for JSON file\n");
                ret = -1;
            } 
        } else {
            ecjp_fprintf("stat() failed for file %s\n", argv[1]);
            ret = -1;
        }
    }

    return ret;
}

#endif  // ECJP_TOKEN_LIST 

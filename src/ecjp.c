#include "include/ecjp.h"

#if DEBUG
#define ecjp_printf(format, ...)    printf(format, __VA_ARGS__)
#else
#define ecjp_printf(format, ...)
#endif

char *ljp_type[ECJP_TYPE_MAX_TYPES] = {
    "UNDEFINED",
    "STRING",
    "NUMBER",
    "OBJECT",
    "ARRAY",
    "BOOL",
    "NULL"
};

// Internal function definitions
ecjp_bool_t ecjp_is_whitespace(char c)
{
    if (c == ' ' || c == '\n' || c == '\r' || c == '\t') {
        return ECJP_BOOL_TRUE;
    }
    return ECJP_BOOL_FALSE;
};

ecjp_bool_t ecjp_is_excode(char c)
{
    if (c >= '0' && c <= '9') {
        // it's a digit
        return ECJP_BOOL_TRUE;
    } else if (c >= 'a' && c <= 'f') {
        // it's a letter in exadecimal range
        return ECJP_BOOL_TRUE;
    } else if (c >= 'A' && c <= 'F') {
        // it's a capital letter in exadecimal range
        return ECJP_BOOL_TRUE;
    }
    return ECJP_BOOL_FALSE;
}

ecjp_bool_t ecjp_is_ctrl(char c)
{
    if (((c >= 0x00) && (c <= 0x1F)) || (c == 0x7F)) {
        if ((c != '\n') && (c != '\r') && (c != '\t')) {
            return ECJP_BOOL_TRUE;
        }
    }
    return ECJP_BOOL_FALSE;
}

ecjp_bool_t ecjp_push_parse_stack(ecjp_parse_stack_item_t *s, char c)
{
    if (s->top >= ECJP_MAX_PARSE_STACK_DEPTH - 1) {
        ecjp_printf("%s - %d: Parse stack overflow\n", __FUNCTION__,__LINE__);
        return ECJP_BOOL_FALSE;
    }
    s->top++;
    s->char_value[s->top] = c;
    return ECJP_BOOL_TRUE;
};

ecjp_bool_t ecjp_pop_parse_stack(ecjp_parse_stack_item_t *s, char expected)
{
    if (s->top < 0) {
        ecjp_printf("%s - %d: Parse stack underflow\n", __FUNCTION__,__LINE__);
        return ECJP_BOOL_FALSE;
    }
    if (expected != s->char_value[s->top]) {
        ecjp_printf("%s - %d: Parse stack mismatch: expected %c, got %c\n", __FUNCTION__,__LINE__, expected, s->char_value[s->top]);
        return ECJP_BOOL_FALSE;
    }
    else {
#ifdef DEBUG_VERBOSE
        ecjp_printf("%s - %d: Popped %c from parse stack\n", __FUNCTION__,__LINE__, s->char_value[s->top]);
#endif
        s->char_value[s->top] = 0;
    }
    s->top--;
    return ECJP_BOOL_TRUE;
};

ecjp_bool_t ecjp_peek_parse_stack(ecjp_parse_stack_item_t *s, char check)
{
    if (s->top < 0) {
        ecjp_printf("%s - %d: Parse stack underflow on peek\n", __FUNCTION__,__LINE__);
        return ECJP_BOOL_FALSE;
    }
    if (check == s->char_value[s->top]) {
        return ECJP_BOOL_TRUE;
    } else {
#ifdef DEBUG_VERBOSE        
        ecjp_printf("%s - %d: Parse stack peek mismatch: search %c, got %c\n", __FUNCTION__,__LINE__, check, s->char_value[s->top]);
#endif
        return ECJP_BOOL_FALSE;
    }
    return ECJP_BOOL_TRUE;
};

int ecjp_add_node_end(ecjp_key_elem_t **head, ecjp_key_token_t *data)
{
    ecjp_key_elem_t  *new_node;
    ecjp_key_elem_t  *current;

    new_node = (ecjp_key_elem_t *)malloc(sizeof(ecjp_key_elem_t));
    if (!new_node)
        return (-1);
    new_node->key.start_pos = data->start_pos;
    new_node->key.length = data->length;
    new_node->key.type = data->type;
    new_node->next = NULL;
    if (*head == NULL)
    {
        *head = new_node;
    }
    else
    {
        current = *head;
        while (current->next != NULL)
            current = current->next;
        current->next = new_node;
    }
    return (0);
}

#ifdef DEBUG_VERBOSE
void ecjp_print_check_summary(ecjp_parser_data_t *p)
{
    ecjp_flags_t flags = p->flags;
    ecjp_printf("Parsing summary:%s\n","");
    ecjp_printf("  Total characters processed: %d\n", p->index);
    ecjp_printf("  Final parser status: %d\n", p->status);
    ecjp_printf("  Total objects: %d\n", p->num_objects);
    ecjp_printf("  Total arrays: %d\n", p->num_arrays);
    ecjp_printf("  Final open brackets count: %d\n", p->open_brackets);
    ecjp_printf("  Final open square brackets count: %d\n", p->open_square_brackets);
    ecjp_printf("Flags status:%s\n","");
    ecjp_printf("  in_string: %d\n", flags.in_string);
    ecjp_printf("  in_key: %d\n", flags.in_key);
    ecjp_printf("  in_value: %d\n", flags.in_value);
    ecjp_printf("  in_number: %d\n", flags.in_number);
    ecjp_printf("  trailing_comma: %d\n", flags.trailing_comma);
    ecjp_printf("Stack:%s\n","");
    ecjp_printf("  Stack top index: %d\n", p->parse_stack.top);
    return;
};
#endif


// External function definitions
ecjp_return_code_t ecjp_dummy(void)
{
    fprintf(stderr, "Hello ecjp library!\n");
    return ECJP_NO_ERROR;
};

ecjp_return_code_t ecjp_get_version(int *major, int *minor, int *patch)
{
    if (major != NULL) {
        *major = ECJP_VERSION_MAJOR;
    }
    if (minor != NULL) {
        *minor = ECJP_VERSION_MINOR;
    }
    if (patch != NULL) {
        *patch = ECJP_VERSION_PATCH;
    }
    return ECJP_NO_ERROR;
};

ecjp_return_code_t ecjp_get_version_string(char *version_string, size_t max_length)
{
    if (version_string == NULL || max_length == 0) {
        return ECJP_NULL_POINTER;
    }
    snprintf(version_string, max_length, "%d.%d.%d", ECJP_VERSION_MAJOR, ECJP_VERSION_MINOR, ECJP_VERSION_PATCH);
    return ECJP_NO_ERROR;
};

ecjp_return_code_t  ecjp_show_error(const char *input, int err_pos)
{
    int row, i, j, err_row, err_column;
    char input_no_newline[ECJP_MAX_INPUT_SIZE];

    // Remove newlines from input for proper error display
    j = 0;
    memset(input_no_newline, 0, ECJP_MAX_INPUT_SIZE);
    for (i = 0; i < strlen(input) && j < ECJP_MAX_INPUT_SIZE - 1; i++) {
        if (input[i] != '\n' && input[i] != '\r') {
            input_no_newline[j++] = input[i];
        }
        else {
            // Replace newline with space to maintain character positions
            input_no_newline[j++] = ' ';
        }
    }
    input_no_newline[j] = '\0';
#ifdef DEBUG_VERBOSE
    ecjp_printf("%s - %d: j = %d - i= %d -input len = %ld\n",__FUNCTION__,__LINE__,j,i,strlen(input));
    ecjp_printf("%s - %d: Input without newlines for error display:\n%s\n",__FUNCTION__,__LINE__,input_no_newline);
#endif

    row = strlen(input) / ECJP_MAX_PRINT_COLUMNS;
    err_row = err_pos / ECJP_MAX_PRINT_COLUMNS;
    err_column = err_pos % ECJP_MAX_PRINT_COLUMNS;

    ecjp_printf("%s - %d: Error at position %d (row %d, column %d):\n", __FUNCTION__,__LINE__,err_pos, err_row + 1, err_column + 1);

    int remain;
    for (i = 0; i <= row; i++) {
        remain = strlen(input_no_newline) - (i * ECJP_MAX_PRINT_COLUMNS);
        remain = (remain > ECJP_MAX_PRINT_COLUMNS) ? ECJP_MAX_PRINT_COLUMNS : remain;
        fprintf(stderr, "%.*s\n",remain, &input_no_newline[i * ECJP_MAX_PRINT_COLUMNS]);
        if(i == err_row) {
            for (j = 0; j < err_column; j++) {
                fprintf(stderr, "-");
            }
            fprintf(stderr, "^\n");
        }
    }
    return ECJP_NO_ERROR;
};

ecjp_return_code_t ecjp_print_keys(const char *input, ecjp_key_elem_t *key_list)
{
    ecjp_key_elem_t *current = key_list;
    int key_index = 0;
    char buffer[ECJP_MAX_KEY_LEN];

    if (input == NULL)
        return ECJP_NULL_POINTER;

    while (current != NULL) {
#if DEBUG        
        if(current->key.length >= ECJP_MAX_KEY_LEN) {
            fprintf(stdout, "  Key length exceeds maximum buffer size (%d), truncating output.\n", ECJP_MAX_KEY_LEN - 1);
        }
#endif
        memset(buffer, 0, sizeof(buffer));
        strncpy(buffer, &input[current->key.start_pos], (current->key.length < ECJP_MAX_KEY_LEN) ? current->key.length : (ECJP_MAX_KEY_LEN - 1));
        ecjp_printf("%s - %d: Key #%d: %s - Type: %s\n",__FUNCTION__,__LINE__,key_index,buffer,ljp_type[current->key.type]);

        current = current->next;
        key_index++;
    }
    return ECJP_NO_ERROR;
};


ecjp_return_code_t ecjp_free_key_list(ecjp_key_elem_t **key_list)
{
    ecjp_key_elem_t *current;
    ecjp_key_elem_t *next;

    if(key_list == NULL || *key_list == NULL)
        return ECJP_NO_ERROR;
        
    current = *key_list;

    while (current != NULL) {
        next = current->next;
        free(current);
        current = next;
    }
    *key_list = NULL;

    return ECJP_NO_ERROR;
}


ecjp_value_type_t ecjp_get_key(const char input[], char *key, ecjp_key_elem_t **key_list)
{
    ecjp_key_elem_t *current = *key_list;
    char buffer[ECJP_MAX_KEY_LEN];
    unsigned char len;
    ecjp_value_type_t type = ECJP_TYPE_UNDEFINED;

    if (input == NULL || key == NULL || key_list == NULL || *key_list == NULL) {
        return type;
    }

    while (current != NULL) {
#if DEBUG
        if(current->key.length >= ECJP_MAX_KEY_LEN) {
            ecjp_printf("%s - %d: Key length exceeds maximum buffer size (%d), truncating output.\n",__FUNCTION__,__LINE__,(ECJP_MAX_KEY_LEN - 1));
        }
#endif
        memset(buffer, 0, sizeof(buffer));
        len = (current->key.length < ECJP_MAX_KEY_LEN) ? current->key.length : (ECJP_MAX_KEY_LEN - 1);
        strncpy(buffer, &input[current->key.start_pos], len);
#ifdef DEBUG_VERBOSE
        ecjp_printf("%s - %d: Comparing key '%s' with stored key '%s' for %d bytes\n",__FUNCTION__,__LINE__,key,buffer,len);
#endif
        if (strncmp(buffer, key, len) == 0) {
            if (current->key.type != ECJP_TYPE_UNDEFINED) {
                type = current->key.type;
                return type;
            }
        }
        current = current->next;
    }   

    return type;
}

ecjp_return_code_t ecjp_get_keys(const char input[],char *key,ecjp_key_elem_t **key_list,ecjp_outdata_t *out)
{
    ecjp_key_elem_t *current = *key_list;
    char buffer[ECJP_MAX_KEY_LEN];
    unsigned char len;
    ecjp_return_code_t ret;

    ret = ECJP_GENERIC_ERROR;
    if (input == NULL || key_list == NULL || *key_list == NULL || out == NULL) {
        ret = ECJP_NULL_POINTER;
        return ret;
    }
    
    if (out->last_pos != 0) {
        // walk the list to find the start position
        while ((current != NULL) && (current->key.start_pos != out->last_pos)) {
            current = current->next;
        }
        // skip to next token
        current = current->next;
    }
    if(current == NULL) {
        ret = ECJP_NO_MORE_KEY;
        return ret;
    }
    
    while (current != NULL) {
        if(current->key.length >= ECJP_MAX_KEY_LEN) {
#if DEBUG
            ecjp_printf("%s - %d: Key length exceeds maximum buffer size (%d), truncating output.\n",__FUNCTION__,__LINE__,(ECJP_MAX_KEY_LEN - 1));
#endif
        }
        memset(buffer, 0, sizeof(buffer));
        len = (current->key.length < ECJP_MAX_KEY_LEN) ? current->key.length : (ECJP_MAX_KEY_LEN - 1);
        strncpy(buffer, &input[current->key.start_pos], len);
        if(key != NULL)
        {
            // search for specific key, return NO_ERROR only if key is found, no need to copy again in out.value
#ifdef DEBUG_VERBOSE
            ecjp_printf("%s - %d: Comparing key '%s' with stored key '%s' for %d bytes at pos %d\n",__FUNCTION__,__LINE__,key, buffer,len,current->key.start_pos);
#endif
            if (strncmp(buffer, key, len) == 0) {
                out->error_code = ECJP_NO_ERROR;
                out->type = current->key.type;
                out->last_pos = current->key.start_pos;
                out->length = current->key.length;
                ret = out->error_code;
                break;
            }
        } else {
#ifdef DEBUG_VERBOSE
            ecjp_printf("%s - %d: Find key '%s' stored at pos %d\n",__FUNCTION__,__LINE__,buffer,current->key.start_pos);
#endif
            out->error_code = ECJP_NO_ERROR;
            out->type = current->key.type;
            out->last_pos = current->key.start_pos;
            out->length = current->key.length;
            if ((out->value != NULL) && (out->value_size >= len)) {
                strncpy(out->value,buffer,len);
            } else {
                out->error_code = ECJP_NO_SPACE_IN_BUFFER_VALUE;
            }
            ret = out->error_code;
            break;
        }
        current = current->next;
    }
    if (ret == ECJP_GENERIC_ERROR)
        ret = ECJP_NO_MORE_KEY;

    return ret;
}

ecjp_return_code_t ecjp_read_key(const char input[],ecjp_indata_t *in,ecjp_outdata_t *out)
{
    ecjp_return_code_t ret = ECJP_NO_ERROR;
    char *ptr;
    char *ptr_value; // pointer to value start
    unsigned int vsize;
    unsigned short int open_brackets;

    if (in == NULL || out == NULL) {
        ecjp_printf("%s - %d: NULL pointer in/out",__FUNCTION__,__LINE__);
        ret = ECJP_NULL_POINTER;
        return ret;
    }
    if (input == NULL) {
        ecjp_printf("%s - %d: No input, NULL pointer",__FUNCTION__,__LINE__);
        ret = ECJP_NULL_POINTER;
        return ret;
    }

    if (in->type == ECJP_TYPE_UNDEFINED) {
        // skip research
        out->type = ECJP_TYPE_UNDEFINED;
        out->error_code = ECJP_EMPTY_STRING;
        ret = out->error_code;
        return ret;
    }

    if(out->value == NULL || out->value_size == 0) {
#if DEBUG
        ecjp_printf("%s - %d: No buffer supplied for store value\n", __FUNCTION__,__LINE__);
#endif
        out->error_code = ECJP_NO_SPACE_IN_BUFFER_VALUE;
        ret = out->error_code;
        return ret;
    }

    vsize = 0;
    open_brackets = 0;
    
    // set to input buffer start position
    ptr = (char *)&input[in->pos + in->lenght + 1]; // skip key and quote
    // find colon
    while (*ptr != ':' && *ptr != '\0') {
        ptr++;
    }
    if(*ptr == ':') {
        ptr++; // skip colon
    }
    // calculate value size
    switch (in->type) {
        case ECJP_TYPE_STRING:
            out->type = ECJP_TYPE_STRING;
            while(ecjp_is_whitespace(*ptr) == ECJP_BOOL_TRUE) {
                ptr++;
            }
            if (*ptr == '"') {
                ptr++;
                ptr_value = ptr;
                while (*ptr != '"' && *ptr != '\0') {
                    vsize++;
                    ptr++;
                }
                out->error_code = ECJP_NO_ERROR;
            }   
            break;

        case ECJP_TYPE_NUMBER:
            out->type = ECJP_TYPE_NUMBER;
            while(ecjp_is_whitespace(*ptr) == ECJP_BOOL_TRUE) {
                ptr++;
            }
            ptr_value = ptr;
            while ((*ptr >= '0' && *ptr <= '9') || *ptr == '-' || *ptr == '+' || *ptr == '.' || *ptr == 'e' || *ptr == 'E') {
                vsize++;
                ptr++;
            }
            out->error_code = ECJP_NO_ERROR;
            break;
        
        case ECJP_TYPE_BOOL:
            out->type = ECJP_TYPE_BOOL;
            while(ecjp_is_whitespace(*ptr) == ECJP_BOOL_TRUE) {
                ptr++;
            }
            ptr_value = ptr;
            if (strncmp(ptr, "true", 4) == 0 || strncmp(ptr, "false", 5) == 0) {
                vsize = (strncmp(ptr, "true", 4) == 0) ? 4 : 5;
            }
            out->error_code = ECJP_NO_ERROR;
            break;

        case ECJP_TYPE_NULL:
            out->type = ECJP_TYPE_NULL;
            while(ecjp_is_whitespace(*ptr) == ECJP_BOOL_TRUE) {
                ptr++;
            }
            ptr_value = ptr;
            vsize = 4; // "null"
            out->error_code = ECJP_NO_ERROR;
            break;

        case ECJP_TYPE_OBJECT:
            out->type = ECJP_TYPE_OBJECT;
            while(ecjp_is_whitespace(*ptr) == ECJP_BOOL_TRUE) {
                ptr++;
            }
            ptr_value = ptr;
            if(*ptr == '{') {
                vsize = 1; // at least the opening bracket
                open_brackets = 1;
                ptr++;
                while ((open_brackets != 0) && (*ptr != '\0')) {
                    if(*ptr == '}') {
                        open_brackets--;
                    } else if (*ptr == '{') {
                        open_brackets++;
                    }
                    vsize++;
                    ptr++;
                    out->error_code = ECJP_NO_ERROR;
                }
            } else {
                vsize = 0;
                out->error_code = ECJP_EMPTY_STRING;
            }   
            break;

        case ECJP_TYPE_ARRAY:
            out->type = ECJP_TYPE_ARRAY;
            while(ecjp_is_whitespace(*ptr) == ECJP_BOOL_TRUE) {
                ptr++;
            }
            ptr_value = ptr;
            if(*ptr == '[') {
                vsize = 1; // at least the opening bracket
                open_brackets = 1;
                ptr++;
                while ((open_brackets != 0) && (*ptr != '\0')) {
                    if(*ptr == ']') {
                        open_brackets--;
                    } else if (*ptr == '[') {
                        open_brackets++;
                    }   
                    vsize++;
                    ptr++;
                }
                out->error_code = ECJP_NO_ERROR;
            } else {
                vsize = 0;
                out->error_code = ECJP_EMPTY_STRING;
            }
            break;

        default:
            vsize = 0;
            out->type = ECJP_TYPE_UNDEFINED;
            out->error_code = ECJP_EMPTY_STRING;
            break;
    }
    if (out->value_size <= vsize) {
        vsize = out->value_size - 1; // leave space for null terminator
        memcpy(out->value, ptr_value, out->value_size);
        ((char *)out->value)[vsize] = '\0';
        ecjp_printf("%s - %d: Value buffer too small, truncated to %d bytes\n", __FUNCTION__,__LINE__,(int)vsize);
        out->error_code = ECJP_NO_SPACE_IN_BUFFER_VALUE;
    }
    else
        memcpy(out->value, ptr_value, vsize);

    out->last_pos = in->pos;
    out->length = in->lenght;

#ifdef DEBUG_VERBOSE
    ecjp_printf("%s - %d: Find key %s of type %s with value: %s\n",__FUNCTION__,__LINE__,in->key,ljp_type[out->type],(char *)out->value);
#endif

    ret = out->error_code;

    return ret;
}


ecjp_value_type_t ecjp_get_key_and_value(const char input[], char *key, ecjp_key_elem_t **key_list, void *value, size_t value_size)
{
    ecjp_key_elem_t *current = *key_list;
    char buffer[ECJP_MAX_KEY_LEN];
    unsigned char len;
    char *ptr; // index that walks through input
    char *ptr_value; // pointer to value start
    size_t vsize = 0;
    ecjp_value_type_t type = ECJP_TYPE_UNDEFINED;
    unsigned short int open_brackets = 0;

    if (input == NULL || key == NULL || key_list == NULL || *key_list == NULL) {
        return type;
    }

    while (current != NULL) {
#if DEBUG        
        if(current->key.length >= ECJP_MAX_KEY_LEN) {
            ecjp_printf("%s - %d: Key length exceeds maximum buffer size (%d), truncating output.\n",__FUNCTION__,__LINE__,(ECJP_MAX_KEY_LEN - 1));
        }
#endif
        memset(buffer, 0, sizeof(buffer));
        len = (current->key.length < ECJP_MAX_KEY_LEN) ? current->key.length : (ECJP_MAX_KEY_LEN - 1);
        strncpy(buffer, &input[current->key.start_pos], len);
#ifdef DEBUG_VERBOSE
        ecjp_printf("%s - %d: Comparing key '%s' with stored key '%s' for %d bytes\n",__FUNCTION__,__LINE__,key,buffer,len);
#endif
        if (strncmp(buffer, key, len) == 0) {
            if (current->key.type != ECJP_TYPE_UNDEFINED) {
                type = current->key.type; // return last found type
                if(value == NULL || value_size == 0) {
#if DEBUG
                    ecjp_printf("%s - %d: No buffer supplied for store value\n", __FUNCTION__,__LINE__);
#endif
                    return type;
                }
                else {
                    // retrieve value
                    ptr = (char *)&input[current->key.start_pos + current->key.length + 1]; // skip key and quote
                    // find colon
                    while (*ptr != ':' && *ptr != '\0') {
                        ptr++;
                    }
                    if(*ptr == ':') {
                        ptr++; // skip colon
                    }
                    // calculate value size
                    switch (type) {
                        case ECJP_TYPE_STRING:
                            while(ecjp_is_whitespace(*ptr) == ECJP_BOOL_TRUE) {
                                ptr++;
                            }
                            if (*ptr == '"') {
                                ptr++;
                                ptr_value = ptr;
                                while (*ptr != '"' && *ptr != '\0') {
                                    vsize++;
                                    ptr++;
                                }
                            }   
                            break;

                        case ECJP_TYPE_NUMBER:
                            while(ecjp_is_whitespace(*ptr) == ECJP_BOOL_TRUE) {
                                ptr++;
                            }
                            ptr_value = ptr;
                            while ((*ptr >= '0' && *ptr <= '9') || *ptr == '-' || *ptr == '+' || *ptr == '.' || *ptr == 'e' || *ptr == 'E') {
                                vsize++;
                                ptr++;
                            }
                            break;
                        
                        case ECJP_TYPE_BOOL:
                            while(ecjp_is_whitespace(*ptr) == ECJP_BOOL_TRUE) {
                                ptr++;
                            }
                            ptr_value = ptr;
                            if (strncmp(ptr, "true", 4) == 0 || strncmp(ptr, "false", 5) == 0) {
                                vsize = (strncmp(ptr, "true", 4) == 0) ? 4 : 5;
                            }
                            break;

                        case ECJP_TYPE_NULL:
                            while(ecjp_is_whitespace(*ptr) == ECJP_BOOL_TRUE) {
                                ptr++;
                            }
                            ptr_value = ptr;
                            vsize = 4; // "null"
                            break;

                        case ECJP_TYPE_OBJECT:
                            while(ecjp_is_whitespace(*ptr) == ECJP_BOOL_TRUE) {
                                ptr++;
                            }
                            ptr_value = ptr;
                            if(*ptr == '{') {
                                vsize = 1; // at least the opening bracket
                                open_brackets = 1;
                                ptr++;
                                while ((open_brackets != 0) && (*ptr != '\0')) {
                                    if(*ptr == '}') {
                                        open_brackets--;
                                    } else if (*ptr == '{') {
                                        open_brackets++;
                                    }
                                    vsize++;
                                    ptr++;
                                }
                            } else {
                                vsize = 0;
                            }   
                            break;

                        case ECJP_TYPE_ARRAY:
                            while(ecjp_is_whitespace(*ptr) == ECJP_BOOL_TRUE) {
                                ptr++;
                            }
                            ptr_value = ptr;
                            if(*ptr == '[') {
                                vsize = 1; // at least the opening bracket
                                open_brackets = 1;
                                ptr++;
                                while ((open_brackets != 0) && (*ptr != '\0')) {
                                    if(*ptr == ']') {
                                        open_brackets--;
                                    } else if (*ptr == '[') {
                                        open_brackets++;
                                    }   
                                    vsize++;
                                    ptr++;
                                }
                            } else {
                                vsize = 0;
                            }
                            break;

                        default:
                            vsize = 0;
                            break;
                    }
#ifdef DEBUG_VERBOSE
                    ecjp_printf("%s - %d: value=%p value_size=%d vsize=%d ptr_value=%p ptr=%p\n",
                                __FUNCTION__,
                                __LINE__,
                                value,
                                (int)value_size,
                                (int)vsize,
                                ptr_value,
                                ptr);
#endif
                    if (value_size <= vsize) {
                        vsize = value_size - 1; // leave space for null terminator
                        memcpy(value, ptr_value, value_size);
                        ((char *)value)[vsize] = '\0';
                        printf("%s: Value buffer too small, truncated to %d bytes\n", __FUNCTION__, (int)vsize);
                    }
                    else
                        memcpy(value, ptr_value, vsize);
                    return type;
                }
            }
        }
        current = current->next;
    }   

    return type;
}


ecjp_return_code_t ecjp_check_syntax(const char *input,int *err_pos,ecjp_key_elem_t **key_list)
{
    ecjp_parser_data_t parser_data;
    ecjp_parser_data_t *p;
    ecjp_key_token_t key_token;

    memset(&key_token, 0, sizeof(ecjp_key_token_t));
    memset(&parser_data, 0, sizeof(ecjp_parser_data_t));
    p =  &parser_data;
    p->parse_stack.top = -1;

    if ((input == NULL) || (err_pos == NULL)) {
        ecjp_printf("%s - %d: NULL pointer input/err_pos\n",__FUNCTION__,__LINE__);
        return ECJP_NULL_POINTER;
    }
    if (strlen(input) == 0) {
        ecjp_printf("%s - %d: Empty string input\n",__FUNCTION__,__LINE__);
        return ECJP_EMPTY_STRING;
    }
#ifdef DEBUG_VERBOSE
    ecjp_printf("%s - %d:\nInput string: %s\n",__FUNCTION__,__LINE__,input);
#endif

    p->index = 0;
    p->flags.all = 0;
    p->status = ECJP_PS_START;
    while (input[p->index] != '\0') {
        // walk through the string
#ifdef DEBUG_VERBOSE        
        ecjp_printf("%s - %d: Index %d, Status %d, Char '%c'\n", __FUNCTION__,__LINE__, p->index, p->status, input[p->index]);
#endif
        switch (p->status)
        {
            case ECJP_PS_START:
                switch (input[p->index]) {
                    case ' ':
                    case '\n':
                    case '\r':
                    case '\t':
                        // skip whitespace
                        p->index++;
                        continue;

                    case '{':
                        p->open_brackets++;
                        p->num_objects++;
                        if(ecjp_push_parse_stack(&(p->parse_stack), '{') == ECJP_BOOL_FALSE) {
                            *err_pos = p->index;
                            return ECJP_GENERIC_ERROR;
                        }   
                        p->status = ECJP_PS_IN_OBJECT;
                        break;

                    case '[':
                        p->open_square_brackets++;
                        p->num_arrays++;
                        if(ecjp_push_parse_stack(&(p->parse_stack), '[') == ECJP_BOOL_FALSE) {
                            *err_pos = p->index;
                            return ECJP_GENERIC_ERROR;
                        }   
                        p->status = ECJP_PS_IN_ARRAY;
                        break;

                    default:
                        *err_pos = p->index;
                        ecjp_printf("%s - %d: Input must start with '{' or ']'\n", __FUNCTION__,__LINE__);
                        return ECJP_SYNTAX_ERROR;
                        break;
                }
                break;
 
            case ECJP_PS_IN_OBJECT:
                switch (input[p->index]) {
                    case ' ':
                    case '\n':
                    case '\r':
                    case '\t':
                        // skip whitespace
                        p->index++;
                        continue;

                    case '{':
                        p->open_brackets++;
                        p->num_objects++;
                        if(ecjp_push_parse_stack(&(p->parse_stack), '{') == ECJP_BOOL_FALSE) {
                            *err_pos = p->index;
                            return ECJP_GENERIC_ERROR;
                        }
                        p->status = ECJP_PS_IN_OBJECT;
                        break;  

                    case '}':
                        p->open_brackets--;
                        if (p->open_brackets < 0) {
                            *err_pos = p->index;
                            ecjp_printf("%s - %d: Mismatched closing bracket\n", __FUNCTION__,__LINE__);
                            return ECJP_SYNTAX_ERROR;
                        }
                        if (p->flags.trailing_comma) {
                            *err_pos = p->index;
                            ecjp_printf("%s - %d: Trailing comma before closing bracket\n", __FUNCTION__,__LINE__);
                            return ECJP_SYNTAX_ERROR;
                        }
                        if (ecjp_pop_parse_stack(&(p->parse_stack), '{') == ECJP_BOOL_FALSE) {
                            *err_pos = p->index;
                            ecjp_printf("%s - %d: Unexpected closing bracket\n", __FUNCTION__,__LINE__);
                            return ECJP_SYNTAX_ERROR;
                        }
                        if (p->open_brackets == 0 && p->open_square_brackets == 0) {
                            p->status = ECJP_PS_END;
                        } else {
                            p->status = ECJP_PS_WAIT_COMMA;
                        }
                        break;

                    case ',':
                        if (p->flags.trailing_comma) {
                            *err_pos = p->index;
                            ecjp_printf("%s - %d: Multiple trailing commas\n", __FUNCTION__,__LINE__);
                            return ECJP_SYNTAX_ERROR;
                        }
                        p->flags.trailing_comma = 1;
                        break;

                    case '"':
                        p->flags.in_string = 1;
                        p->status = ECJP_PS_IN_KEY;
                        p->flags.in_key = 1;
                        p->flags.trailing_comma = 0;
                        // Record key position
                        key_token.start_pos = (p->index + 1) ; // skip initial quote
                        break;
                    
                    default:
                        *err_pos = p->index;
                        ecjp_printf("%s - %d: Expected string key or closing bracket, receive: %c\n", __FUNCTION__,__LINE__,input[p->index]);
                        return ECJP_SYNTAX_ERROR;
                        break;  
                }
                break;
            
            case ECJP_PS_IN_ARRAY:
                switch (input[p->index]) {
                    case ' ':
                    case '\n':
                    case '\r':
                    case '\t':
                        // skip whitespace
                        p->index++;
                        continue;

                    case '{':
                        p->open_brackets++;
                        p->num_objects++;
                        if (ecjp_push_parse_stack(&(p->parse_stack), '{') == ECJP_BOOL_FALSE) {
                            *err_pos = p->index;
                            return ECJP_GENERIC_ERROR;
                        }
                        p->status = ECJP_PS_IN_OBJECT;
                        if(p->flags.trailing_comma) {
                            p->flags.trailing_comma = 0;
                        }   
                        break;

                    case '}':
                        p->open_brackets--;
                        if (p->open_brackets < 0) {
                            *err_pos = p->index;
                            ecjp_printf("%s - %d: Mismatched closing bracket\n", __FUNCTION__,__LINE__);
                            return ECJP_SYNTAX_ERROR;
                        }
                        if (ecjp_pop_parse_stack(&(p->parse_stack), '{') == ECJP_BOOL_FALSE) {
                            *err_pos = p->index;
                            ecjp_printf("%s - %d: Unexpected closing bracket\n", __FUNCTION__,__LINE__);
                            return ECJP_SYNTAX_ERROR;
                        }
                        p->status = ECJP_PS_WAIT_COMMA;
                        break;

                    case '[':
                        p->open_square_brackets++;
                        p->num_arrays++;
                        if (ecjp_push_parse_stack(&(p->parse_stack), '[') == ECJP_BOOL_FALSE) {
                            *err_pos = p->index;
                            return ECJP_GENERIC_ERROR;
                        }
                        p->status = ECJP_PS_IN_ARRAY;
                        if (p->flags.trailing_comma) {
                            p->flags.trailing_comma = 0;
                        }   
                        break;

                    case ']':
                        p->open_square_brackets--;
                        if (p->open_square_brackets < 0) {
                            *err_pos = p->index;
                            ecjp_printf("%s - %d: Mismatched closing square bracket\n", __FUNCTION__,__LINE__);
                            return ECJP_SYNTAX_ERROR;
                        }
                        if (ecjp_pop_parse_stack(&(p->parse_stack), '[') == ECJP_BOOL_FALSE) {
                            *err_pos = p->index;
                            ecjp_printf("%s - %d: Unexpected closing square bracket\n", __FUNCTION__,__LINE__);
                            return ECJP_SYNTAX_ERROR;
                        }
                        p->status = ECJP_PS_WAIT_COMMA;
                        break;
                    
                    case '"':
                        p->flags.in_string = 1;
                        p->status = ECJP_PS_IN_VALUE;
                        if (p->flags.trailing_comma) {
                            p->flags.trailing_comma = 0;
                        }
                        break;

                    default:
                        if (strncmp(&input[p->index], "true", 4) == 0 || strncmp(&input[p->index], "false", 5) == 0 || strncmp(&input[p->index], "null", 4) == 0) {
                            // valid value
                            p->status = ECJP_PS_WAIT_COMMA;
                            p->index += (input[p->index] == 'f') ? 5 : 4; // move index forward
                            if (p->flags.trailing_comma) {
                                p->flags.trailing_comma = 0;
                            }   
                            continue;
                        }
                        if ((input[p->index] >= '0' && input[p->index] <= '9') || input[p->index] == '-') {
                            // valid value start
                            p->flags.in_number = 1;
                            if (input[p->index] == '0') {
                                p->flags.start_zero = 1;
                            }
                            p->status = ECJP_PS_IN_VALUE;
                            if (p->flags.trailing_comma) {
                                p->flags.trailing_comma = 0;
                            }       
                        } else {
                            *err_pos = p->index;
                            ecjp_printf("%s - %d: Invalid character in value\n", __FUNCTION__,__LINE__);
                            return ECJP_SYNTAX_ERROR;
                        }
                        break;
                }
                break;
            
            case ECJP_PS_IN_KEY:
                // in this status we are always inside a string (p->flags.in_string == 1) 
                switch (input[p->index]) {
                    case '\\':
                        // skip escaped character
                        p->index++;
                        switch(input[p->index]) {
                            case '"':
                            case '\\':
                            case '/':
                            case 'b':
                            case 'f':
                            case 'r':
                            case 'n':
                            case 't':
                                p->index++;
                                continue;

                            case 'u':
                                if(ecjp_is_excode(input[p->index+1]) &&
                                   ecjp_is_excode(input[p->index+2]) &&
                                   ecjp_is_excode(input[p->index+3]) &&
                                   ecjp_is_excode(input[p->index+4])) {
                                    p->index += 4;
                                } else {
                                    *err_pos = p->index;
                                    ecjp_printf("%s - %d: Invalid character in unicode sequence\n", __FUNCTION__,__LINE__);
                                    return ECJP_SYNTAX_ERROR;
                                }
                                break;

                            default:
                                *err_pos = p->index;
                                ecjp_printf("%s - %d: Invalid character in escape sequence (%c)\n", __FUNCTION__,__LINE__, input[p->index]);
                                return ECJP_SYNTAX_ERROR;
                                break;
                        }
                        break;

                    case '"':
                        p->flags.in_string = 0;
                        p->status = ECJP_PS_WAIT_COLON;
                        p->flags.in_key = 0;
                        break;

                    default:
                        if(ecjp_is_ctrl(input[p->index])) {
                            *err_pos = p->index;
                            ecjp_printf("%s - %d: Invalid control character in key\n", __FUNCTION__,__LINE__);
                            return ECJP_SYNTAX_ERROR;
                        } else {
                            // continue in key
                            key_token.length++;
                        }
                        break;
                }
                break;

            case ECJP_PS_WAIT_COLON:
                switch (input[p->index]) {
                    case ' ':
                    case '\n':
                    case '\r':
                    case '\t':
                        // skip whitespace
                        p->index++;
                        continue;

                    case ':':
                        p->status = ECJP_PS_WAIT_VALUE;
                        break;

                    default:
                        *err_pos = p->index;
                        ecjp_printf("%s - %d: Expected colon after key, received: %c\n",__FUNCTION__,__LINE__,input[p->index]);
                        return ECJP_SYNTAX_ERROR;
                        break;
                }
                break;

            case ECJP_PS_WAIT_VALUE:
                switch (input[p->index]) {
                    case ' ':
                    case '\n':
                    case '\r':
                    case '\t':
                        // skip whitespace
                        p->index++;
                        continue;

                    case '{':
                        p->open_brackets++;
                        p->num_objects++;
                        if (ecjp_push_parse_stack(&(p->parse_stack), '{') == ECJP_BOOL_FALSE) {
                            *err_pos = p->index;
                            return ECJP_GENERIC_ERROR;
                        }
                        p->status = ECJP_PS_IN_OBJECT;
                        // Record key type
                        key_token.type = ECJP_TYPE_OBJECT;
                        // Add key token to the list
                        if (ecjp_add_node_end(key_list, &key_token) != 0) {
                            *err_pos = p->index;
                            ecjp_printf("%s - %d: Failed to add key token to the list\n", __FUNCTION__,__LINE__);
                            return ECJP_GENERIC_ERROR;
                        }
#ifdef DEBUG_VERBOSE
                        else
                            ecjp_printf("%s - %d: Added object key token to the list, key_list = %p\n", __FUNCTION__,__LINE__, (void *)*key_list);
#endif
                        // Reset key_token for future keys
                        memset(&key_token, 0, sizeof(ecjp_key_token_t));
                        break;

                    case '[':
                        p->open_square_brackets++;
                        p->num_arrays++;
                        if (ecjp_push_parse_stack(&(p->parse_stack), '[') == ECJP_BOOL_FALSE) {
                            *err_pos = p->index;
                            return ECJP_GENERIC_ERROR;
                        }
                        p->status = ECJP_PS_IN_ARRAY;
                        // Record key type
                        key_token.type = ECJP_TYPE_ARRAY;
                        // Add key token to the list
                        if (ecjp_add_node_end(key_list, &key_token) != 0) {
                            *err_pos = p->index;
                            ecjp_printf("%s - %d: Failed to add key token to the list\n", __FUNCTION__,__LINE__);
                            return ECJP_GENERIC_ERROR;
                        }
#ifdef DEBUG_VERBOSE
                        else
                            ecjp_printf("%s - %d: Added object key token to the list, key_list = %p\n", __FUNCTION__,__LINE__, (void *)*key_list);
#endif
                        // Reset key_token for future keys
                        memset(&key_token, 0, sizeof(ecjp_key_token_t));
                        break;

                    case '"':
                        p->flags.in_string = 1;
                        p->status = ECJP_PS_IN_VALUE;
                        // Record key type
                        key_token.type = ECJP_TYPE_STRING;
                        // Add key token to the list
                        if (ecjp_add_node_end(key_list, &key_token) != 0) {
                            *err_pos = p->index;
                            ecjp_printf("%s - %d: Failed to add key token to the list\n", __FUNCTION__,__LINE__);
                            return ECJP_GENERIC_ERROR;
                        }
#ifdef DEBUG_VERBOSE
                        else
                            ecjp_printf("%s - %d: Added object key token to the list, key_list = %p\n", __FUNCTION__,__LINE__, (void *)*key_list);
#endif
                        // Reset key_token for future keys
                        memset(&key_token, 0, sizeof(ecjp_key_token_t));
                        break;

                    default:
                        if (strncmp(&input[p->index], "true", 4) == 0 || strncmp(&input[p->index], "false", 5) == 0 || strncmp(&input[p->index], "null", 4) == 0) {
                            // valid value
                            p->status = ECJP_PS_WAIT_COMMA;
                            // Record key type
                            if ((input[p->index] == 't') || (input[p->index] == 'f')) {
                                key_token.type = ECJP_TYPE_BOOL;
                            } else {
                                key_token.type = ECJP_TYPE_NULL;
                            }
                            // Add key token to the list
                            if (ecjp_add_node_end(key_list, &key_token) != 0) {
                                *err_pos = p->index;
                                ecjp_printf("%s - %d: Failed to add key token to the list\n", __FUNCTION__,__LINE__);
                                return ECJP_GENERIC_ERROR;
                            }
#ifdef DEBUG_VERBOSE
                            else
                                ecjp_printf("%s - %d: Added object key token to the list, key_list = %p\n", __FUNCTION__,__LINE__, (void *)*key_list);
#endif
                            // Reset key_token for future keys
                            memset(&key_token, 0, sizeof(ecjp_key_token_t));
                            p->index += (input[p->index] == 'f') ? 5 : 4; // move index forward
                            continue;
                        }
                        if ((input[p->index] >= '0' && input[p->index] <= '9') || input[p->index] == '-') {
                            // valid value start
                            p->flags.in_number = 1;
                            if (input[p->index] == '0') {
                                p->flags.start_zero = 1;
                            }
                            p->status = ECJP_PS_IN_VALUE;
                            // Record key type
                            key_token.type = ECJP_TYPE_NUMBER;
                            // Add key token to the list
                            if (ecjp_add_node_end(key_list, &key_token) != 0) {
                                *err_pos = p->index;
                                ecjp_printf("%s - %d: Failed to add key token to the list\n", __FUNCTION__,__LINE__);
                                return ECJP_GENERIC_ERROR;
                            }
#ifdef DEBUG_VERBOSE
                            else
                                ecjp_printf("%s - %d: Added object key token to the list, key_list = %p\n", __FUNCTION__,__LINE__, (void *)*key_list);
#endif  
                            // Reset key_token for future keys
                            memset(&key_token, 0, sizeof(ecjp_key_token_t));
                        } else {
                            *err_pos = p->index;
                            ecjp_printf("%s - %d: Invalid character in value\n", __FUNCTION__,__LINE__);
                            return ECJP_SYNTAX_ERROR;
                        }
                        break;
                }
                break;

            case ECJP_PS_IN_VALUE:
                switch(input[p->index]) {
                    case '"':
                        if(!p->flags.in_string) {
                            *err_pos = p->index;
                            ecjp_printf("%s - %d: Unexpected quote in value\n", __FUNCTION__,__LINE__);
                            return ECJP_SYNTAX_ERROR;
                        }
                        p->flags.in_string = 0;
                        p->status = ECJP_PS_WAIT_COMMA;
                        break;

                    default:
                        if (p->flags.in_string) {
                            switch (input[p->index]) {
                                case '\\':
                                    // skip escaped character
                                    p->index++;
                                    switch(input[p->index]) {
                                        case '"':
                                        case '\\':
                                        case '/':
                                        case 'b':
                                        case 'f':
                                        case 'r':
                                        case 'n':
                                        case 't':
                                            p->index++;
                                            continue;

                                        case 'u':
                                            if(ecjp_is_excode(input[p->index+1]) &&
                                               ecjp_is_excode(input[p->index+2]) &&
                                               ecjp_is_excode(input[p->index+3]) &&
                                               ecjp_is_excode(input[p->index+4])) {
                                                p->index += 4;
                                            } else {
                                                *err_pos = p->index;
                                                ecjp_printf("%s - %d: Invalid character in unicode sequence\n", __FUNCTION__,__LINE__);
                                                return ECJP_SYNTAX_ERROR;
                                            }
                                            break;

                                        default:
                                            *err_pos = p->index;
                                            ecjp_printf("%s - %d: Invalid character in escape sequence (%c)\n", __FUNCTION__,__LINE__, input[p->index]);
                                            return ECJP_SYNTAX_ERROR;
                                            break;
                                    }
                                    break;

                                default:
                                    if (ecjp_is_ctrl(input[p->index])) {
                                        *err_pos = p->index;
                                        ecjp_printf("%s - %d: Invalid control character inside value\n", __FUNCTION__,__LINE__);
                                        return ECJP_SYNTAX_ERROR;
                                    }
                                    break;
                            }
                        } else {
                            switch (input[p->index]) {
                                case ' ':
                                case '\n':
                                case '\r':
                                case '\t':
                                    if (p->flags.in_number) {
                                        p->flags.in_number = 0;
                                        p->flags.start_zero = 0;
                                        p->status = ECJP_PS_WAIT_COMMA;
                                    }
                                    else {
                                        // skip whitespace
                                        p->index++;
                                        continue;
                                    }
                                    break;
                                
                                case ',':
                                    if(p->flags.trailing_comma) {
                                        *err_pos = p->index;
                                        ecjp_printf("%s - %d: Multiple trailing commas\n", __FUNCTION__,__LINE__);
                                        return ECJP_SYNTAX_ERROR;
                                    }
                                    if (p->flags.in_number) {
                                        p->flags.in_number = 0;
                                        p->flags.start_zero = 0;
                                    }
                                    p->flags.trailing_comma = 1;
                                    if (ecjp_peek_parse_stack(&(p->parse_stack),'[') == ECJP_BOOL_TRUE) {
                                        p->status = ECJP_PS_IN_ARRAY;
                                    } else {
                                        p->status = ECJP_PS_IN_OBJECT;
                                    }
                                    break;
                                
                                case '0':
                                case '1':
                                case '2':
                                case '3':
                                case '4':
                                case '5':
                                case '6':
                                case '7':
                                case '8':
                                case '9':
                                case '.':
                                case 'e':
                                case 'E':
                                case '+':
                                case '-':
                                    if (p->flags.in_number) {
                                        // valid number continuation
                                        if (input[p->index] == '.') {
                                            if (p->flags.start_zero == 1) {
                                                p->flags.start_zero = 0;
                                            } 
                                            break;
                                        } else {
                                            if (p->flags.start_zero == 1) {
                                                // number can start with '0' unless is decimal value
                                                *err_pos = p->index;
                                                ecjp_printf("%s - %d: Number can't start with value 0\n", __FUNCTION__,__LINE__);
                                                return ECJP_SYNTAX_ERROR;
                                            }
                                            break;
                                        }
                                    } else {
                                        *err_pos = p->index;
                                        ecjp_printf("%s - %d: Unexpected number character in value\n", __FUNCTION__,__LINE__);
                                        return ECJP_SYNTAX_ERROR;
                                    }
                                    break;

                                case '}':
                                    if (ecjp_pop_parse_stack(&(p->parse_stack), '{') == ECJP_BOOL_FALSE) {
                                        *err_pos = p->index;
                                        ecjp_printf("%s - %d: Unexpected closing bracket\n", __FUNCTION__,__LINE__);
                                        return ECJP_SYNTAX_ERROR;
                                    }
                                    p->open_brackets--;
                                    if (p->open_brackets < 0) {
                                        *err_pos = p->index;
                                        ecjp_printf("%s - %d: Mismatched closing bracket\n", __FUNCTION__,__LINE__);
                                        return ECJP_SYNTAX_ERROR;
                                    }
                                    if (p->open_brackets == 0 && p->open_square_brackets == 0) {
                                        p->status = ECJP_PS_END;
                                    } else {
                                        p->status = ECJP_PS_WAIT_COMMA;
                                    }
                                    break;

                                case ']':
                                    if (ecjp_pop_parse_stack(&(p->parse_stack), '[') == ECJP_BOOL_FALSE) {
                                        *err_pos = p->index;
                                        ecjp_printf("%s - %d: Unexpected closing square bracket\n", __FUNCTION__,__LINE__);
                                        return ECJP_SYNTAX_ERROR;
                                    }
                                    p->open_square_brackets--;
                                    if (p->open_square_brackets < 0) {
                                        *err_pos = p->index;
                                        ecjp_printf("%s - %d: Mismatched closing square bracket\n", __FUNCTION__,__LINE__);
                                        return ECJP_SYNTAX_ERROR;
                                    }
                                    if (p->open_square_brackets == 0 && p->open_brackets == 0) {
                                        p->status = ECJP_PS_END;
                                    } else {
                                        p->status = ECJP_PS_WAIT_COMMA;
                                    }
                                    break;

                                default:
                                    *err_pos = p->index;
                                    ecjp_printf("%s - %d: Invalid character in value\n", __FUNCTION__,__LINE__);
                                    return ECJP_SYNTAX_ERROR;
                                    break;
                            }
                        }
                        break;
                }
                break;
                        
            case ECJP_PS_WAIT_COMMA:
                switch (input[p->index]) {
                    case ' ':
                    case '\n':
                    case '\r':
                    case '\t':
                        // skip whitespace
                        p->index++;
                        continue;
                    
                    case '}':
                        if (p->flags.trailing_comma) {
                            *err_pos = p->index;
                            ecjp_printf("%s - %d: Trailing comma before closing bracket\n", __FUNCTION__,__LINE__);
                            return ECJP_SYNTAX_ERROR;
                        }
                        if (ecjp_pop_parse_stack(&(p->parse_stack), '{') == ECJP_BOOL_FALSE) {
                            *err_pos = p->index;
                            ecjp_printf("%s - %d: Unexpected closing bracket\n", __FUNCTION__,__LINE__);
                            return ECJP_SYNTAX_ERROR;
                        }
                        p->open_brackets--;
                        if (p->open_brackets < 0) {
                            *err_pos = p->index;
                            ecjp_printf("%s - %d: Mismatched closing bracket\n", __FUNCTION__,__LINE__);
                            return ECJP_SYNTAX_ERROR;
                        }
                        if (p->open_brackets == 0 && p->open_square_brackets == 0) {
                            p->status = ECJP_PS_END;
                        }
                        break;

                    case ']':
                        if (p->flags.trailing_comma) {
                            *err_pos = p->index;
                            ecjp_printf("%s - %d: Trailing comma before closing square bracket\n", __FUNCTION__,__LINE__);
                            return ECJP_SYNTAX_ERROR;
                        }
                        if (ecjp_pop_parse_stack(&(p->parse_stack), '[') == ECJP_BOOL_FALSE) {
                            *err_pos = p->index;
                            ecjp_printf("%s - %d: Unexpected closing square bracket\n", __FUNCTION__,__LINE__);
                            return ECJP_SYNTAX_ERROR;
                        }
                        p->open_square_brackets--;
                        if (p->open_square_brackets < 0) {
                            *err_pos = p->index;
                            ecjp_printf("%s - %d: Mismatched closing square bracket\n", __FUNCTION__,__LINE__);
                            return ECJP_SYNTAX_ERROR;
                        }
                        if (p->open_brackets == 0 && p->open_square_brackets == 0) {
                            p->status = ECJP_PS_END;
                        }
                        break;

                    case ',':
                        if (p->flags.trailing_comma) {
                            *err_pos = p->index;
                            ecjp_printf("%s - %d: Multiple trailing commas\n", __FUNCTION__,__LINE__);
                            return ECJP_SYNTAX_ERROR;
                        }
                        if (ecjp_peek_parse_stack(&(p->parse_stack),'[') == ECJP_BOOL_TRUE) {
                            p->status = ECJP_PS_IN_ARRAY;
                        } else {
                            p->status = ECJP_PS_IN_OBJECT;
                        }
                        p->flags.trailing_comma = 1;
                        break;

                    default:
                        *err_pos = p->index;
                        ecjp_printf("%s - %d: Expected comma after value\n", __FUNCTION__,__LINE__);
                        return ECJP_SYNTAX_ERROR;
                        break;
                }   
                break;
        
            case ECJP_PS_END:
                // should be only whitespace after end
                switch (input[p->index]) {
                    case ' ':
                    case '\n':
                    case '\r':
                    case '\t':
                        // skip whitespace
                        p->index++;
                        continue;

                    default:
                        *err_pos = p->index;
                        ecjp_printf("%s - %d: Unexpected character after end of JSON\n", __FUNCTION__,__LINE__);
                        return ECJP_SYNTAX_ERROR;
                }
                break;
        
            default:
                break;
        }

        p->index++;
    }

    if (p->status != ECJP_PS_END) {
        ecjp_printf("%s - %d: Incomplete JSON structure\n", __FUNCTION__,__LINE__);
        return ECJP_SYNTAX_ERROR;
    }
    if ((p->open_brackets != 0) || (p->open_square_brackets != 0)) {
        *err_pos = (p->index - 1);
        ecjp_printf("%s - %d: Mismatched brackets at end of input\n", __FUNCTION__,__LINE__);
        return ECJP_BRACKETS_MISSING;
    }

#ifdef DEBUG_VERBOSE
    ecjp_print_check_summary(p);
#endif

    return ECJP_NO_ERROR;
};

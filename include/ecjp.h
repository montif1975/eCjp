

#ifndef ECJP_H
#define ECJP_H

#ifdef __cplusplus
extern "C" {
#endif

// Add your function declarations here

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "ecjp_limit.h"

#define DEBUG                       1
#define DEBUG_VERBOSE

#define ECJP_VERSION_MAJOR           0
#define ECJP_VERSION_MINOR           1
#define ECJP_VERSION_PATCH           0

typedef enum {
    ECJP_BOOL_FALSE,
    ECJP_BOOL_TRUE           
} ecjp_bool_t;

typedef struct key_token {
    ECJP_TYPE_POS_KEY   start_pos;
    ECJP_TYPE_LEN_KEY   length;
    unsigned char       type;
} ecjp_key_token_t;

typedef struct key_elem {
    ecjp_key_token_t key;
    struct key_elem *next;
} ecjp_key_elem_t;

typedef enum {
    ECJP_NO_ERROR = 0,
    ECJP_GENERIC_ERROR,
    ECJP_BRACKETS_MISSING,
    ECJP_SYNTAX_ERROR,
    ECJP_NULL_POINTER,
    ECJP_EMPTY_STRING,
    ECJP_NO_MORE_KEY,
    ECJP_NO_SPACE_IN_BUFFER_VALUE,
    ECJP_MAX_ERROR
} ecjp_return_code_t;

typedef enum {
    ECJP_PS_START = 0,
    ECJP_PS_IN_OBJECT,
    ECJP_PS_IN_ARRAY,
    ECJP_PS_IN_KEY,
    ECJP_PS_WAIT_VALUE,
    ECJP_PS_IN_VALUE,
    ECJP_PS_WAIT_COLON,
    ECJP_PS_WAIT_COMMA,
    ECJP_PS_END,
    ECJP_PS_MAX_STATUS
} ecjp_parse_status_t;

typedef union  {
        unsigned char all;
        struct {
            unsigned char in_string     : 1;
            unsigned char in_number     : 1;
            unsigned char in_key        : 1;
            unsigned char in_value      : 1;
            unsigned char trailing_comma: 1;
            unsigned char start_zero    : 1;
            unsigned char reserved      : 2;
        };
} ecjp_flags_t;

typedef struct ljp_parse_stack_item {
    char char_value[ECJP_MAX_PARSE_STACK_DEPTH];
    int top;
} ecjp_parse_stack_item_t;

typedef struct ljp_parser_data {
    int index;
    int status;
    int open_brackets;
    int num_objects;
    int open_square_brackets;
    int num_arrays;
    ecjp_parse_stack_item_t parse_stack;
    ecjp_flags_t flags;
} ecjp_parser_data_t;

typedef enum {
    ECJP_TYPE_UNDEFINED = 0,
    ECJP_TYPE_STRING,
    ECJP_TYPE_NUMBER,
    ECJP_TYPE_OBJECT,
    ECJP_TYPE_ARRAY,
    ECJP_TYPE_BOOL,
    ECJP_TYPE_NULL,
    ECJP_TYPE_MAX_TYPES  
} ecjp_value_type_t;

typedef struct ljp_outdata {
    ecjp_return_code_t  error_code;
    ECJP_TYPE_POS_KEY   last_pos;
    ECJP_TYPE_LEN_KEY   length;
    ecjp_value_type_t   type;
    void                *value;
    unsigned int        value_size;
} ecjp_outdata_t;

typedef struct ljp_indata {
    char                key[ECJP_MAX_KEY_LEN];
    ecjp_value_type_t   type;
    ECJP_TYPE_POS_KEY   pos;
    ECJP_TYPE_LEN_KEY   lenght;
} ecjp_indata_t;

// function prototypes for internal use
ecjp_bool_t ecjp_is_whitespace(char c);
ecjp_bool_t ecjp_is_excode(char c);
ecjp_bool_t ecjp_push_parse_stack(ecjp_parse_stack_item_t *stack, char value);
ecjp_bool_t ecjp_pop_parse_stack(ecjp_parse_stack_item_t *stack, char expected_value);
ecjp_bool_t ecjp_peek_parse_stack(ecjp_parse_stack_item_t *stack, char check_value);
int ecjp_add_node_end(ecjp_key_elem_t **key_list, ecjp_key_token_t *new_key);
#if DEBUG
void ecjp_print_check_summary(ecjp_parser_data_t *p);
#endif

// Function declarations (public API)
ecjp_return_code_t ecjp_dummy(void);
ecjp_return_code_t ecjp_get_version(int *major, int *minor, int *patch);
ecjp_return_code_t ecjp_get_version_string(char *version_string, size_t max_length);
ecjp_return_code_t ecjp_show_error(const char *input, int err_pos);
ecjp_return_code_t ecjp_print_keys(const char *input, ecjp_key_elem_t *key_list);
ecjp_return_code_t ecjp_free_key_list(ecjp_key_elem_t **key_list);
ecjp_value_type_t ecjp_get_key(const char input[], char *key, ecjp_key_elem_t **key_list);
ecjp_return_code_t ecjp_get_keys(const char input[],char *key,ecjp_key_elem_t **key_list,ecjp_outdata_t *out);
ecjp_return_code_t ecjp_read_key(const char input[],ecjp_indata_t *in,ecjp_outdata_t *out);
ecjp_value_type_t ecjp_get_key_and_value(const char input[], char *key, ecjp_key_elem_t **key_list, void *value, size_t value_size);
ecjp_return_code_t ecjp_check_syntax(const char *input, int *err_pos,ecjp_key_elem_t **key_list);

#ifdef __cplusplus
}
#endif

#endif // ECJP_H



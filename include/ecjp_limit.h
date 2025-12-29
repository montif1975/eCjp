#ifndef ECJP_LIMIT_H
#define ECJP_LIMIT_H

#ifdef ECJP_RUN_ON_PC
#define ECJP_MAX_INPUT_SIZE          1024*1024 // 1 MB
#define ECJP_MAX_PRINT_COLUMNS       80
#define ECJP_MAX_PARSE_STACK_DEPTH   2048
#define ECJP_MAX_KEY_LEN             512
#define ECJP_MAX_ARRAY_ELEM_LEN      1024*16 // 16 kB
#define ECJP_MAX_ITEM_LEN            1024*16 // 16 kB
#define ECJP_MAX_NESTED_LEVEL        1024

#define ECJP_TYPE_POS_KEY            unsigned short int
#define ECJP_TYPE_LEN_KEY            unsigned short int  

#else
#ifdef ECJP_RUN_ON_MCU
#define ECJP_MAX_INPUT_SIZE          1024 // 1 kB
#define ECJP_MAX_PRINT_COLUMNS       80
#define ECJP_MAX_PARSE_STACK_DEPTH   64
#define ECJP_MAX_KEY_LEN             32
#define ECJP_MAX_ARRAY_ELEM_LEN      256
#define ECJP_MAX_ITEM_LEN            512
#define ECJP_MAX_NESTED_LEVEL        8

#define ECJP_TYPE_POS_KEY            unsigned short int  
#define ECJP_TYPE_LEN_KEY            unsigned char  

#else
// run with default
#define ECJP_MAX_INPUT_SIZE          8192 // 1 kB
#define ECJP_MAX_PRINT_COLUMNS       80
#define ECJP_MAX_PARSE_STACK_DEPTH   128
#define ECJP_MAX_KEY_LEN             64
#define ECJP_MAX_ITEM_LEN            512
#define ECJP_MAX_ARRAY_ELEM_LEN      1024
#define ECJP_MAX_NESTED_LEVEL        12

#define ECJP_TYPE_POS_KEY            unsigned short int  
#define ECJP_TYPE_LEN_KEY            unsigned char  

#endif // ECJP_RUN_ON_MCU

#endif // ECJP_RUN_ON_PC

#endif // ECJP_LIMIT_H

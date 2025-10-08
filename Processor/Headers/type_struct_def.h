#ifndef STRUCTS_H
#define STRUCTS_H

#include <stdio.h>

typedef int CalcErr_t;
typedef int StackErr_t;
typedef int stack_t;

const int POISON = 0xDEADFACE;
const int CANARY = 0xFEEDCAFE;

const size_t max_stack_capasity = (size_t) -16;


#ifdef DEBUG_CANARY

    struct stack_origin_info
    {
        const char* stack_name;
        const char* origin_function;
        const char* origin_file;
        size_t      origin_line;
    };

    struct stack_struct
    {
        int               stack_canary1;
        stack_origin_info info;
        stack_t*          data;
        size_t            capacity;
        size_t            cur_position;
        int               stack_canary2;
    };

#elif DEBUG_STACK_VERIFY

    struct stack_origin_info
    {
        const char* stack_name;
        const char* origin_function;
        const char* origin_file;
        size_t      origin_line;
    };

    struct stack_struct
    {
        stack_origin_info info;
        stack_t*          data;
        size_t            capacity;
        size_t            cur_position;
    };

#elif DEBUG_HASH

    struct stack_origin_info
    {
        const char* stack_name;
        const char* origin_function;
        const char* origin_file;
        size_t      origin_line;
    };

    struct stack_struct
    {
        stack_origin_info info;
        int               hash;
        stack_t*          data;
        size_t            capacity;
        size_t            cur_position;
    };
#else

    struct stack_struct
    {
        stack_t*          data;
        size_t            capacity;
        size_t            cur_position;
    };

#endif

enum other_const {
    RELEASE_MODE  =  0xCAFEBABE,
    FINISHED      =  777,
    FILE_OPEN_ERR = -111,
    FILE_OPEN_SUC =  111,
    ALLOC_ERR     = -222,
    DUMP_FAILED   = -333,
    DUMP_SUCCESS  =  333
};

enum stack_errors {
    NULL_POINT_STK    =   1,
    NULL_POINT_DATA   =   2,
    STK_OVERFLOW      =   4,
    MAX_CAPASITY      =   8,
    LEFT_n_RIGHT_EXIT =  16,
    LEFT_EXIT         =  32,
    RIGHT_EXIT        =  64,
    HASH_CHANGE       = 128,
    NEG_CAPASITY      = 256
};

enum calc_errors {
    NULL_POINT_CALC   =   1,
    NULL_POINT_BC_BUF =   2,
    NEG_BC_SIZE       =   4,
    NULL_POINT_REG    =   8
};

#endif
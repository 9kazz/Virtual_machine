#ifndef STRUCTS_H
#define STRUCTS_H

#include <stdio.h>

typedef int stack_t;
const int POISON = 0xDEADFACE;
const int CANARY = 0xFEEDCAFE;

const size_t max_stack_capasity = (size_t) -16;

typedef int StackErr_t;


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

#elif DEBUG_HESH

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
        int               hesh;
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


enum canary_error {
    NO_EXIT           =  1,
    LEFT_EXIT         = -1,
    RIGHT_EXIT        = -2,
    LEFT_n_RIGHT_EXIT = -3
};

enum error_type {
    VERIFY_SUCCESS    =  2,
    ALLOC_ERR         = -4,
    NULL_POINTER      = -5,
    CREATE_SUCCESS    =  5,
    STACK_OVERFLOW    = -6,
    MAX_CAPASITY      = -7,
    CORRECT_CAPASITY  =  8,
    NEG_CAPASITY      = -8,
    HESH_ERR          = -9,
    HESH_OK           =  9,
    FILE_OPEN_ERR     = -10,
    FILE_OPEN_SUCCESS =  10,
    INCOR_OPERATOR    = -11,
    CALC_SUCCESS      =  11,
    RELEASE_MODE      =  12,
    DUMP_SUCCESS      =  13,
    DUMP_FAILED       = -13
};

#endif
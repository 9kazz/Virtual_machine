#ifndef STACK_H
#define STACK_H

#include <stdio.h>

#include "LogFile.h"
#include "type_struct_def.h"

#ifdef DEBUG_CANARY
    #define INIT_STK(name)                                  \
        stack_struct name {};                               \
            name.stack_canary1               = CANARY;      \
            name.info.stack_name             = #name;       \
            name.info.origin_function        = __func__;    \
            name.info.origin_file            = __FILE__;    \
            name.info.origin_line            = __LINE__;    \
            name.data                        = NULL;        \
            name.capacity                    = 0;           \
            name.cur_position                = 0;           \
            name.stack_canary2               = CANARY;           
            
#elif DEBUG_STACK_VERIFY
    #define INIT_STK(name)                                  \
        stack_struct name {};                               \
            name.info.stack_name             = #name;       \
            name.info.origin_function        = __func__;    \
            name.info.origin_file            = __FILE__;    \
            name.info.origin_line            = __LINE__;    \
            name.data                        = NULL;        \
            name.capacity                    = 0;           \
            name.cur_position                = 0;       

#elif DEBUG_HESH  
    #define INIT_STK(name)                                  \
        stack_struct name {};                               \
            name.info.stack_name             = #name;       \
            name.info.origin_function        = __func__;    \
            name.info.origin_file            = __FILE__;    \
            name.info.origin_line            = __LINE__;    \
            name.hesh                        = 0;           \
            name.data                        = NULL;        \
            name.capacity                    = 0;           \
            name.cur_position                = 0;      

#else
    #define INIT_STK(name)                                  \
        stack_struct name {};                               \
            name.data         = NULL;                       \
            name.capacity     = 0;                          \
            name.cur_position = 0;     
#endif 


#define STK_CTOR(stack, size_of_stack)                      \
    INIT_STK(stack)                                         \
    Stack_Ctor(&stack, size_of_stack);


StackErr_t Stack_Ctor   (stack_struct* stack, int     size_of_stack);

StackErr_t Stack_Push   (stack_struct* stack, stack_t value);

stack_t    Stack_Pop    (stack_struct* stack);

StackErr_t Stack_Dtor   (stack_struct* stack);

stack_t    Stack_LastEl (stack_struct* stack);

StackErr_t Stack_Realloc(stack_struct* stack);

#endif
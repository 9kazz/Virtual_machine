#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>

#include "type_struct_def.h"
#include "LogFile.h"

#define BLUE   "\033[34m"
#define RED    "\033[31m"
#define YELLOW "\033[93m"
#define GREEN  "\033[32m"
#define RESET  "\033[0m"


StackErr_t Stack_Dump                 (stack_struct* stack);

StackErr_t Stack_Verify_Ctor          (stack_struct* stack,  int         size_of_stack);

StackErr_t Stack_Verify               (stack_struct* stack,  const char* checking_function);

#ifdef DEBUG_CANARY

    StackErr_t struct_canary_checking  (stack_struct* stack, const char* checking_function, int* error_code);
    StackErr_t buffer_canary_checking  (stack_struct* stack, const char* checking_function, int* error_code);

#elif DEBUG_HASH    
    int        stack_hash_function    (stack_struct* stack);
    StackErr_t stack_hash_checking    (stack_struct* stack,  const char* checking_function, int* error_code);

#endif

StackErr_t fopen_checking(FILE* opening_file);


#endif
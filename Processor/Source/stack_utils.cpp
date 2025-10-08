#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "stack_utils.h"
#include "stack.h"
#include "type_struct_def.h"
#include "LogFile.h"

StackErr_t Stack_Verify(stack_struct* stack, const char* checking_function) {

    int error_code = 0;

    #ifdef DEBUG_STACK_VERIFY

        if (stack == NULL) {

            fprintf(LogFile, "Stack_Verify: NULL pointer to stack" "\n");
            fprintf(LogFile, "Error code: %d (NULL_POINT_STK)" "\n\n", NULL_POINT_STK);

            Stack_Dump(stack);

            error_code = error_code | NULL_POINT_STK;

            fprintf(LogFile, "Stack_Verify: verification ended with error code: %d\n", error_code);
            return error_code;
        }

        if (stack -> data == NULL) {

            fprintf(LogFile, "Stack_Verify in %s: NULL pointer to data in %s" "\n", checking_function, stack -> info.stack_name);
            fprintf(LogFile, "Error code: %d (NULL_POINT_DATA)" "\n\n", NULL_POINT_DATA);

            Stack_Dump(stack);

            error_code = error_code | NULL_POINT_DATA;

            fprintf(LogFile, "Stack_Verify: verification ended with error code: %d\n", error_code);
            return error_code;
        }

        if (stack -> cur_position >= stack -> capacity) {

            fprintf(LogFile, "Stack_Verify in %s: overflow at %s" "\n", checking_function, stack -> info.stack_name);
            fprintf(LogFile, "Error code: %d (STK_OVERFLOW)" "\n\n", STK_OVERFLOW);

            Stack_Dump(stack);

            error_code = error_code | STK_OVERFLOW; 
        }

        if (stack -> capacity >= max_stack_capasity) {

            fprintf(LogFile, "Stack_Verify in %s: suspicious capasity (too big) of %s" "\n", checking_function, stack -> info.stack_name);
            fprintf(LogFile, "Error code: %d (MAX_CAPASITY)" "\n\n", MAX_CAPASITY);

            Stack_Dump(stack);

            error_code = error_code | MAX_CAPASITY;
        }        

    #elif DEBUG_CANARY

        struct_canary_checking(stack, checking_function, &error_code);   
                                                            
        buffer_canary_checking(stack, checking_function, &error_code);        

    #elif DEBUG_HASH

        stack_hash_checking(stack, checking_function, &error_code);
      
    #else

        fprintf(LogFile, "Stack_Verify: program works in release mode");

        return RELEASE_MODE;

    #endif

    fprintf(LogFile, "Stack_Verify: verification ended with error code: %d\n", error_code);

    return error_code;
}

StackErr_t Stack_Verify_Ctor(stack_struct* stack, int size_of_stack) {

    int error_code = 0;

    #if defined(DEBUG_STACK_VERIFY) || defined(DEBUG_CANARY) || defined(DEBUG_HASH)

        if (stack == NULL) {

            fprintf(LogFile, "Stack_Verify_Ctor in Stack_Ctor: NULL pointer to stack" "\n");
            fprintf(LogFile, "Error code: %d (NULL_POINT_STK)" "\n\n", NULL_POINT_STK);

            Stack_Dump(stack);

            error_code = error_code | NULL_POINT_STK;

            fprintf(LogFile, "Stack_Verify_Ctor: verification ended with error code: %d\n", error_code);
            return error_code;
        }


        if (size_of_stack < 0) {

            fprintf(LogFile, "Stack_Verify_Ctor: negative capacity of %s" "\n", stack -> info.stack_name);
            fprintf(LogFile, "Error code: %d (NEG_CAPASITY)" "\n\n", NEG_CAPASITY);

            Stack_Dump(stack);

            error_code = error_code | NEG_CAPASITY;
        }

    #else 

        fprintf(LogFile, "Stack_Verify_Ctor: program works in release mode" "\n\n");

        return RELEASE_MODE;

    #endif

    fprintf(LogFile, "Stack_Verify_Ctor: verification ended with error code: %d" "\n\n", error_code);

    return error_code;
}

StackErr_t Stack_Dump(stack_struct* stack) {

    #if defined(DEBUG_STACK_VERIFY) || defined(DEBUG_CANARY) || defined(DEBUG_HASH)

        if (stack == NULL) {
            fprintf(LogFile, "Stack_Dump: NULL pointer to stack" "\n\n");
            return DUMP_FAILED;
        }

        if (stack->data == NULL) {
            fprintf(LogFile, "Stack_Dump: NULL pointer to stack.data" "\n\n");
            return DUMP_FAILED;
        }

        fprintf(LogFile, "Stack_Dump prints: " "%s" "\n", stack -> info.stack_name);
        fprintf(LogFile, "origin function: "   "%s" "\n", stack -> info.origin_function);
        fprintf(LogFile, "origin file: "       "%s" "\n", stack -> info.origin_file);
        fprintf(LogFile, "origin line: "       "%d" "\n", stack -> info.origin_line);

        fprintf(LogFile, "%s [%p]\n", stack -> info.stack_name, &stack);
        fprintf(LogFile, "{"  "\n");
        fprintf(LogFile, "\t" "cur_position: %d;"   "\n", stack -> cur_position);
        fprintf(LogFile, "\t" "capasity: %d;"       "\n", stack -> capacity);
        fprintf(LogFile, "\t" "data [%p]"           "\n", stack -> data);
        fprintf(LogFile, "\t" "{" "\n");

        for (size_t i = 0; i < stack -> capacity; i++) {

            if (i < stack -> cur_position) {

                if (stack -> data[i] == POISON) {
                    fprintf(LogFile, "\t" "*[%d] = %d (POISON)" "\n" , i, stack -> data[i]);
                    continue;
                }

                fprintf(LogFile, "\t" "*[%d] = %d" "\n" , i, stack -> data[i]);

                continue;
            }

            if (stack -> data[i] == POISON) {
                fprintf(LogFile, "\t" " [%d] = %d (POISON)" "\n" , i, stack -> data[i]);
                continue;
            }

            fprintf(LogFile, "\t" " [%d] = %d" "\n" , i, stack -> data[i]);
        }

        fprintf(LogFile, "\t" "}" "\n");
        fprintf(LogFile, "}" "\n");

        #ifdef DEBUG_CANARY
            fprintf(LogFile, "Struct_Canary_1 = %d" "\n", stack -> stack_canary1);
            fprintf(LogFile, "Struct_Canary_2 = %d" "\n", stack -> stack_canary2);
            fprintf(LogFile, "Stack_Canary_1  = %d" "\n", *(stack->data - 1));
            fprintf(LogFile, "Stack_Canary_1  = %d" "\n", *(stack->data + stack->capacity));
        #elif DEBUG_HASH
            fprintf(LogFile, "Hash = %d" "\n", stack -> hash);
        #endif

        fprintf(LogFile, "\n\n");
        
    #else
        
        return RELEASE_MODE;
        
    #endif
    
        return DUMP_SUCCESS;
}

#ifdef DEBUG_CANARY

    StackErr_t struct_canary_checking (stack_struct* stack, const char* checking_function, int* error_code) {

        if (stack -> stack_canary1 != CANARY &&
            stack -> stack_canary2 != CANARY) {
            
            fprintf(LogFile, "Struct_canary_checking in %s: stack structure is damaged (both of directions) in %s" "\n", checking_function, stack -> info.stack_name);
            fprintf(LogFile, "Error code: %d (LEFT_n_RIGHT_EXIT)" "\n\n", LEFT_n_RIGHT_EXIT);

            Stack_Dump(stack);
            
            *error_code = *error_code | LEFT_n_RIGHT_EXIT;
        }

        if (stack -> stack_canary1 != CANARY) {
            
            fprintf(LogFile, "Struct_canary_checking in %s: stack structure is damaged (left direction) in %s" "\n", checking_function, stack -> info.stack_name);
            fprintf(LogFile, "Error code: %d (LEFT_EXIT)" "\n\n", LEFT_EXIT);

            Stack_Dump(stack);

            *error_code = *error_code | LEFT_EXIT;
        }

        if (stack -> stack_canary2 != CANARY) {
            
            fprintf(LogFile, "Struct_canary_checking in %s: stack structure is damaged (right direction) in %s" "\n", checking_function, stack -> info.stack_name);
            fprintf(LogFile, "Error code: %d (RIGHT_EXIT)" "\n\n", RIGHT_EXIT);

            Stack_Dump(stack);

            *error_code = *error_code | RIGHT_EXIT;
        }

        return FINISHED;  
    }
        
    StackErr_t buffer_canary_checking (stack_struct* stack, const char* checking_function, int* error_code) {
        
        stack_t canary1 = *(stack->data - 1); 
        stack_t canary2 = *(stack->data + stack->capacity);

        if (canary1 != CANARY && 
            canary2 != CANARY) {
            
            fprintf(LogFile, "buffer_canary_checking in %s: exit outside (both of directions) of %s" "\n", checking_function, stack -> info.stack_name);
            fprintf(LogFile, "Error code: %d (LEFT_n_RIGHT_EXIT)" "\n\n", LEFT_n_RIGHT_EXIT);

            Stack_Dump(stack);

            *error_code = *error_code | LEFT_n_RIGHT_EXIT;
        }

        if (canary1 != CANARY) {

            fprintf(LogFile, "buffer_canary_checking in %s: exit outside (left direction) of %s" "\n", checking_function, stack -> info.stack_name);
            fprintf(LogFile, "Error code: %d (LEFT_EXIT)" "\n\n", LEFT_EXIT);

            Stack_Dump(stack);

            *error_code = *error_code | LEFT_EXIT;
        }

        if (canary2 != CANARY) {

            fprintf(LogFile, "buffer_canary_checking in %s: exit outside (right direction) of %s" "\n", checking_function, stack -> info.stack_name);
            fprintf(LogFile, "Error code: %d (RIGHT_EXIT)" "\n\n", RIGHT_EXIT);

            Stack_Dump(stack);

            *error_code = *error_code | RIGHT_EXIT;
        }    

        return FINISHED;  
    }

#elif DEBUG_HASH

    int stack_hash_function(stack_struct* stack) {

        int prime_coef = 257;
        int mod        = ~(1 << 31);
        
        int hash = 0;

        for (size_t i = 0; i < stack -> capacity; i++) {
            stack_t el_of_stack = stack -> data[i];
            hash = (hash * prime_coef + el_of_stack) % mod;
        }

        hash = hash + stack->capacity * 997 + stack->cur_position * 571;
        stack -> hash = hash;

        return hash;
    }

    StackErr_t stack_hash_checking(stack_struct* stack, const char* checking_function, int* error_code) {

        int hash_before_func = stack -> hash;

        int hash_after_func  = stack_hash_function(stack);

        if (hash_after_func != hash_before_func) {

            fprintf(LogFile, "stack_hash_checking in %s: are not the same hash values before and after using function in stack: %s" "\n", checking_function, stack -> info.stack_name);
            fprintf(LogFile, "Error code: %d (HASH_ERR)" "\n\n", HASH_ERR);

            Stack_Dump(stack);

            *error_code = *error_code | HASH_СHANGE;
        }

        return FINISHED;
    }

#endif


StackErr_t fopen_checking(FILE* opening_file) {

    if ( ! opening_file) {

        fprintf(stderr, "fopen_checking: can not open the file" "\n");
        fprintf(stderr, "Error code: %d (FILE_OPEN_ERR)" "\n\n", FILE_OPEN_ERR);

        return FILE_OPEN_ERR;
    }

    return FILE_OPEN_SUC;
}

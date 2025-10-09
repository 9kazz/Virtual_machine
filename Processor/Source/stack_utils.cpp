#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "type_struct_def.h"
#include "LogFile.h"
#include "stack_utils.h"
#include "stack.h"

StackErr_t Stack_Verify(stack_struct* stack, const char* checking_function) {

    #ifdef DEBUG_STACK_VERIFY

        if (stack == NULL) {

            fprintf(LogFile, "Stack_Verify in Stack_Ctor: NULL pointer to stack" "\n");
            fprintf(LogFile, "Error code: %d (NULL_POINTER)" "\n\n", NULL_POINTER);

            Stack_Dump(stack);

            exit(NULL_POINTER);  // без exit`a безмолвная и мучительная смерть проги
            return NULL_POINTER;
        }

        if (stack -> data == NULL) {

            fprintf(LogFile, "Stack_Verify in %s: NULL pointer to data in %s" "\n", checking_function, stack -> info.stack_name);
            fprintf(LogFile, "Error code: %d (NULL_POINTER)" "\n\n", NULL_POINTER);

            Stack_Dump(stack);

            exit(NULL_POINTER);  // без exit`a безмолвная и мучительная смерть проги
            return NULL_POINTER;
        }

        if (stack -> cur_position >= stack -> capacity) {

            fprintf(LogFile, "Stack_Verify in %s: overflow at %s" "\n", checking_function, stack -> info.stack_name);
            fprintf(LogFile, "Error code: %d (STACK_OVERFLOW)" "\n\n", STACK_OVERFLOW);

            Stack_Dump(stack);

            return STACK_OVERFLOW; 
        }

        if (stack -> capacity >= max_stack_capasity) {

            fprintf(LogFile, "Stack_Verify in %s: suspicious capasity (too big) of %s" "\n", checking_function, stack -> info.stack_name);
            fprintf(LogFile, "Error code: %d (MAX_CAPASITY)" "\n\n", MAX_CAPASITY);

            Stack_Dump(stack);

            return MAX_CAPASITY;
        }

        fprintf(LogFile, "Stack_Verify in %s: checking is successful" "\n", checking_function);
        fprintf(LogFile, "End with %d (VERIFY_SUCCESS)" "\n\n", VERIFY_SUCCESS);

        return VERIFY_SUCCESS;
        

    #elif DEBUG_CANARY

        StackErr_t verify = struct_canary_checking(stack, checking_function);   
        if (verify != NO_EXIT)
            return verify; 
                                                            
        verify = canary_checking(stack, checking_function);
        if (verify != NO_EXIT)
            return verify;    
    
        fprintf(LogFile, "Stack_Verify in %s: checking is successful" "\n", checking_function);
        fprintf(LogFile, "End with %d (VERIFY_SUCCESS)" "\n\n", VERIFY_SUCCESS);
        
        return VERIFY_SUCCESS;
        

    #elif DEBUG_HESH

        StackErr_t verify = stack_hesh_checking(stack, checking_function);
        if (verify == HESH_ERR)    
            return verify;       
        
        fprintf(LogFile, "Stack_Verify in %s: checking is successful" "\n", checking_function);
        fprintf(LogFile, "End with %d (VERIFY_SUCCESS)" "\n\n", VERIFY_SUCCESS);
            
        return VERIFY_SUCCESS;

    #else
        return RELEASE_MODE;

    #endif
}

StackErr_t Stack_Verify_Ctor(stack_struct* stack, int size_of_stack) {

#if defined(DEBUG_STACK_VERIFY) || defined(DEBUG_CANARY) || defined(DEBUG_HESH)

    if (stack == NULL) {

        fprintf(LogFile, "Stack_Verify in Stack_Ctor: NULL pointer to stack" "\n");
        fprintf(LogFile, "Error code: %d (NULL_POINTER)" "\n\n", NULL_POINTER);

        Stack_Dump(stack);

        exit(NULL_POINTER);  // без exit`a безмолвная и мучительная смерть проги
        return NULL_POINTER;
    }


    if (size_of_stack < 0) {

        fprintf(LogFile, "Stack_Verify: negative capacity of %s" "\n", stack -> info.stack_name);
        fprintf(LogFile, "Error code: %d (NEG_CAPASITY)" "\n\n", NEG_CAPASITY);

        Stack_Dump(stack);

        exit(NEG_CAPASITY);  // без exit`a безмолвная и мучительная смерть проги
        return NEG_CAPASITY;
    }

    return CORRECT_CAPASITY;

#else 
    return RELEASE_MODE;

#endif
}

StackErr_t Stack_Dump(stack_struct* stack) {

#if defined(DEBUG_STACK_VERIFY) || defined(DEBUG_CANARY) || defined(DEBUG_HESH)
        if (stack == NULL) {
            fprintf(LogFile, "Stack_Dump: NULL pointer to stack" "\n");
            return DUMP_FAILED;
        }

        if (stack->data == NULL) {
            fprintf(LogFile, "Stack_Dump: NULL pointer to stack.data" "\n");
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
        #elif DEBUG_HESH
            fprintf(LogFile, "Hesh = %d" "\n", stack -> hesh);
        #endif

        fprintf(LogFile, "\n\n");

        return DUMP_SUCCESS;

#else
        return RELEASE_MODE;

#endif
}

#ifdef DEBUG_CANARY

    StackErr_t struct_canary_checking (stack_struct* stack, const char* checking_function) {

        if (stack -> stack_canary1 != CANARY &&
            stack -> stack_canary2 != CANARY) {
            
            fprintf(LogFile, "Struct_canary_checking in %s: stack structure is damaged (both of directions) in %s" "\n", checking_function, stack -> info.stack_name);
            fprintf(LogFile, "Error code: %d (LEFT_n_RIGHT_EXIT)" "\n\n", LEFT_n_RIGHT_EXIT);

            Stack_Dump(stack);
            
            return LEFT_n_RIGHT_EXIT;
        }

        if (stack -> stack_canary1 != CANARY) {
            
            fprintf(LogFile, "Struct_canary_checking in %s: stack structure is damaged (left direction) in %s" "\n", checking_function, stack -> info.stack_name);
            fprintf(LogFile, "Error code: %d (LEFT_EXIT)" "\n\n", LEFT_EXIT);

            Stack_Dump(stack);

            return LEFT_EXIT;
        }

        if (stack -> stack_canary2 != CANARY) {
            
            fprintf(LogFile, "Struct_canary_checking in %s: stack structure is damaged (right direction) in %s" "\n", checking_function, stack -> info.stack_name);
            fprintf(LogFile, "Error code: %d (RIGHT_EXIT)" "\n\n", RIGHT_EXIT);

            Stack_Dump(stack);

            return RIGHT_EXIT;
        }

        return NO_EXIT;  
    }
        
    StackErr_t canary_checking (stack_struct* stack, const char* checking_function) {
        
        stack_t canary1 = *(stack->data - 1); 
        stack_t canary2 = *(stack->data + stack->capacity);

        if (canary1 != CANARY && 
            canary2 != CANARY) {
            
            fprintf(LogFile, "canary_checking in %s: exit outside (both of directions) of %s" "\n", checking_function, stack -> info.stack_name);
            fprintf(LogFile, "Error code: %d (LEFT_n_RIGHT_EXIT)" "\n\n", LEFT_n_RIGHT_EXIT);

            Stack_Dump(stack);

            return LEFT_n_RIGHT_EXIT;
        }

        if (canary1 != CANARY) {

            fprintf(LogFile, "canary_checking in %s: exit outside (left direction) of %s" "\n", checking_function, stack -> info.stack_name);
            fprintf(LogFile, "Error code: %d (LEFT_EXIT)" "\n\n", LEFT_EXIT);

            Stack_Dump(stack);

            return LEFT_EXIT;
        }

        if (canary2 != CANARY) {

            fprintf(LogFile, "canary_checking in %s: exit outside (right direction) of %s" "\n", checking_function, stack -> info.stack_name);
            fprintf(LogFile, "Error code: %d (RIGHT_EXIT)" "\n\n", RIGHT_EXIT);

            Stack_Dump(stack);

            return RIGHT_EXIT;
        }    

        return NO_EXIT;  
    }

#elif DEBUG_HESH

    int stack_hesh_function(stack_struct* stack) {

        int prime_coef = 257;
        int mod        = ~(1 << 31);
        
        int hesh = 0;

        for (size_t i = 0; i < stack -> capacity; i++) {
            stack_t el_of_stack = stack -> data[i];
            hesh = (hesh * prime_coef + el_of_stack) % mod;
        }

        hesh = hesh + stack->capacity * 997 + stack->cur_position * 571;
        stack -> hesh = hesh;

        return hesh;
    }

    StackErr_t stack_hesh_checking(stack_struct* stack, const char* checking_function) {

        int hesh_before_func = stack -> hesh;

        int hesh_after_func  = stack_hesh_function(stack);

        if (hesh_after_func != hesh_before_func) {

            fprintf(LogFile, "stack_hesh_checking in %s: are not the same hesh values before and after using function in stack: %s" "\n", checking_function, stack -> info.stack_name);
            fprintf(LogFile, "Error code: %d (HESH_ERR)" "\n\n", HESH_ERR);

            Stack_Dump(stack);

            return HESH_ERR;
        }

        return HESH_OK;
    }

#endif


StackErr_t fopen_checking(FILE* file) {

    if ( ! file) {

        fprintf(stderr, "fopen_checking: can not open the file" "\n");
        fprintf(stderr, "Error code: %d (FILE_OPEN_ERR)" "\n\n", FILE_OPEN_ERR);

        return FILE_OPEN_ERR;
    }

    return FILE_OPEN_SUCCESS;
}

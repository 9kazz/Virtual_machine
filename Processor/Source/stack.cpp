#include <stdio.h>
#include <stdlib.h>

#include "LogFile.h"
#include "type_struct_def.h"    
#include "stack_utils.h"
#include "stack.h"


StackErr_t Stack_Ctor(stack_struct* stack, int size_of_stack) {

    Stack_Verify_Ctor(stack, size_of_stack);

    #ifdef DEBUG_CANARY
    
        stack_t* temp_pointer = (stack_t*) calloc( (size_t) size_of_stack + 2, sizeof(stack -> data[0]));

        if (temp_pointer == NULL) {
            fprintf(LogFile, "Stack_Ctor: NULL pointer" "\n\n" );
            return ALLOC_ERR;
        }

        stack -> data = temp_pointer + 1;

        for (int i = 0; i < size_of_stack; i++)
            stack -> data[i] = POISON;
        
        *(stack->data - 1)             = CANARY;
        *(stack->data + size_of_stack) = CANARY;
    
        stack -> capacity = size_of_stack;

        return CREATE_SUCCESS;


    #else

        stack_t* temp_pointer = (stack_t*) calloc( (size_t) size_of_stack, sizeof(stack -> data[0]));

        if (temp_pointer == NULL) {
            fprintf(LogFile, "Stack_Ctor: NULL pointer" "\n\n");
            return ALLOC_ERR;
        }

        stack -> data = temp_pointer;

        for (int i = 0; i < size_of_stack; i++)
            stack -> data[i] = POISON;

        stack -> capacity = size_of_stack;

        #ifdef DEBUG_HESH
            stack_hesh_function(stack);
        #endif
        Stack_Verify_Ctor(stack, size_of_stack);

        return CREATE_SUCCESS;

    #endif
}

StackErr_t Stack_Push(stack_struct* stack, stack_t value) {

    Stack_Verify(stack, "Stack_Push");
    
    if (value == POISON)
        fprintf(LogFile, "Warning from Stack_Push: pushed POISON value" "\n\n");
    
    stack -> data[stack -> cur_position] = value;

    ++(stack -> cur_position);

    #ifdef DEBUG_HESH
        stack_hesh_function(stack);
    #endif
    Stack_Verify(stack, "Stack_Push");
    
    return VERIFY_SUCCESS;
}

stack_t Stack_Pop(stack_struct* stack) { 

    Stack_Verify(stack, "Stack_Pop");

    --(stack -> cur_position);

    stack_t temp_buffer = stack -> data[stack -> cur_position];
    stack -> data[stack -> cur_position] = POISON;

    #ifdef DEBUG_HESH
        stack_hesh_function(stack);
    #endif
    Stack_Verify(stack, "Stack_Pop");

    return temp_buffer;
}

StackErr_t Stack_Dtor(stack_struct* stack) {

    Stack_Verify(stack, "Stack_Dotr");

    for (size_t i = 0; i < stack -> cur_position; i++)
        stack -> data[i] = POISON; 

    stack -> cur_position = 0;

    free(stack); 

    #ifdef DEBUG_HESH
        stack_hesh_function(stack);
    #endif
    Stack_Verify(stack, "Stack_Dtor");

    return VERIFY_SUCCESS;
}

stack_t Stack_Top(stack_struct* stack) {
    
    Stack_Verify(stack, "Stack_Top");

    size_t last_el_index = stack -> cur_position - 1;

    #ifdef DEBUG_HESH
        stack_hesh_function(stack);
    #endif
    Stack_Verify(stack, "Stack_Top");

    return stack -> data[last_el_index];
}

StackErr_t Stack_Realloc(stack_struct* stack) {
    
    Stack_Verify(stack, "Stack_Realloc");

    size_t new_capacity = 2 * stack -> capacity;

    stack_t* temp_pointer = (stack_t*) realloc(stack, new_capacity);

    if ( ! temp_pointer) {
        fprintf(LogFile, "Stack_Ctor: NULL pointer" "\n\n" );
        return ALLOC_ERR;
    }

    stack -> data = temp_pointer;

    for (size_t i = stack -> cur_position; i < new_capacity; i++)
        stack -> data[i] = POISON;

    #ifdef DEBUG_CANARY
        *(stack->data + new_capacity) = CANARY;
    #endif 

    stack -> capacity = new_capacity;

    #ifdef DEBUG_HESH
        stack_hesh_function(stack);
    #endif
    Stack_Verify(stack, "Stack_Realloc");
    
    return VERIFY_SUCCESS;
}
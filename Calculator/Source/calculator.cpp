#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>

#include "utils.h"
#include "calculator.h"

int calculator(stack_struct* stack, int* bit_code_buf, size_t bit_code_size) {

    size_t ind = 0;

    while (ind < bit_code_size &&
           bit_code_buf[ind] != HLT) 
    {
        int operator = bit_code_buf[ind];
        switch(operator)
        {
            case PUSH:
                stack_t push_arg = bit_code_buf[ind + 1];
                Stack_Push(stack, push_arg);
                break;

            case POP: 
                Stack_Pop(stack);
                break;

            case OUT: 
                Stack_Out(stack);
                break;

            case ADD: case SUB: case MUL: case DIV: 
                Stack_Calc (stack, operator);
                break;
        }

    }
}

int Stack_Calc (stack_struct* stack, int operator) {

    int operand_1 = 0, operand_2 = 0;
    int result = 0;

    operand_2 = Stack_Pop(stack);
    operand_1 = Stack_Pop(stack);

    switch (operator)
    {
    case ADD:
        result = operand_1 + operand_2;
        break;
    
    case SUB:
        result = operand_1 - operand_2;
        break;

    case MUL:
        result = operand_1 * operand_2;
        break;

    case DIV:
        result = (int) (operand_1 / operand_2);
        break;

    default:
        fprintf(stderr, "stack_Calc: incorrect operator (%d)", operator);
        return UNKNOWN_COM;
    }

    Stack_Push(stack, result);

    return CALC_SUCCESS;
}
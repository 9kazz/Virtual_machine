#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>
#include <math.h>

#include "calculator.h"
#include "LogFile.h"
#include "type_struct_def.h"    
#include "stack_utils.h"
#include "stack.h"
#include "utils.h"

void calculator(CalcStruct* calc_struct) {

    size_t bite_code_size = calc_struct -> bite_code.size;
    int* bite_code_buf    = calc_struct -> bite_code.buffer;
    stack_struct* stack   = &(calc_struct -> calc_stack);

    size_t ind = 0;

    while (ind < bite_code_size &&
           bite_code_buf[ind] != CMD_HLT) 
    {
        int arif_operator = bite_code_buf[ind];
        switch(arif_operator)
        {
            case CMD_PUSH:
            {
                stack_t push_arg = bite_code_buf[ind + 1];
                Stack_Push(stack, push_arg);
                break;
            }

            case CMD_OUT: 
                Stack_Pop(stack);
                break;

            case CMD_ADD: case CMD_SUB: case CMD_MUL: case CMD_DIV: case CMD_SQRT:
                Stack_Calc(stack, arif_operator);
                break;

            default:
                fprintf(stderr, "Calculator: unknown arifmetic command");
                break;
        }
        ind += 2;
    }
}

int Stack_Calc (stack_struct* stack, int arif_operator) {

    int result = 0;

    int operand_1 = 0;
    int operand_2 = 0;
    
    operand_1 = Stack_Pop(stack);

    if (arif_operator == CMD_ADD || 
        arif_operator == CMD_SUB ||
        arif_operator == CMD_MUL ||
        arif_operator == CMD_DIV ) 
    {
        operand_2 = Stack_Pop(stack);  
    }

    switch (arif_operator)
    {
    case CMD_ADD:
        result = operand_2 + operand_1;
        break;
    
    case CMD_SUB:
        result = operand_2 - operand_1;
        break;

    case CMD_MUL:
        result = operand_2 * operand_1;
        break;

    case CMD_DIV:
        result = (int) (operand_2 / operand_1);
        break;

    case CMD_SQRT:
        result = sqrt( (double) operand_1);
        break;

    default:
        fprintf(stderr, "stack_Calc: incorrect operator (%d)", arif_operator);
        return UNKNOWN_COM;
    }

    Stack_Push(stack, result);

    return CALC_SUCCESS;
}
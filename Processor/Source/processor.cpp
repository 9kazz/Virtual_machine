#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>
#include <math.h>

#include "processor.h"
#include "LogFile.h"
#include "type_struct_def.h"    
#include "stack_utils.h"
#include "stack.h"
#include "utils.h"

void processor(CalcStruct* calc_struct) {

    size_t bite_code_size =   calc_struct -> bite_code.size;
    int*   bite_code_buf  =   calc_struct -> bite_code.buffer;
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

            case CMD_PUSHR:
            {
                int register_num = bite_code_buf[ind + 1];
                Stack_PushR(calc_struct, register_num);
                break;
            }

            case CMD_POPR:
            {
                int register_num = bite_code_buf[ind + 1];
                Stack_PopR(calc_struct, register_num);
                break;
            }

            case CMD_IN:
                Stack_In(calc_struct);
                break;

            case CMD_JMP:
            {
                int cmd_num_to_return = bite_code_buf[ind + 1];
                Jump_to_JMP(&ind, cmd_num_to_return);
                continue;
            }

            case CMD_JB:
            {
                int cmd_num_to_return = bite_code_buf[ind + 1];
                Jump_Below_JB(&ind, cmd_num_to_return, stack);
                continue;
            }

            case CMD_JBE:
            {    
                int cmd_num_to_return = bite_code_buf[ind + 1];
                Jump_Below_Equal_JBE(&ind, cmd_num_to_return, stack);
                continue;
            }

            case CMD_JA:
            {
                int cmd_num_to_return = bite_code_buf[ind + 1];
                Jump_Above_JA(&ind, cmd_num_to_return, stack);
                continue;
            }

            case CMD_JAE:
            {
                int cmd_num_to_return = bite_code_buf[ind + 1];
                Jump_Above_Equal_JAE(&ind, cmd_num_to_return, stack);
                continue;
            }

            case CMD_JE:
            {
                int cmd_num_to_return = bite_code_buf[ind + 1];
                Jump_Equal_JE(&ind, cmd_num_to_return, stack);
                continue;
            }

            case CMD_JNE:
            {
                int cmd_num_to_return = bite_code_buf[ind + 1];
                Jump_Not_Equal_JNE(&ind, cmd_num_to_return, stack);
                continue;
            }

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
        fprintf(stderr, "Stack_Calc: incorrect operator (%d)", arif_operator);
        return UNKNOWN_COM;
    }

    Stack_Push(stack, result);

    return CALC_SUCCESS;
}

StackErr_t Stack_PushR (CalcStruct* calc_struct, int register_num) {
    assert(calc_struct);
    assert(register_num >= 0 && register_num <= COUNT_OF_REG);

    stack_t       value = calc_struct -> register_buf[register_num];
    stack_struct* stack = &(calc_struct -> calc_stack);

    return Stack_Push(stack, value);
}

StackErr_t Stack_PopR (CalcStruct* calc_struct, int register_num) {
    assert(calc_struct);
    assert(register_num >= 0 && register_num <= COUNT_OF_REG);

    stack_struct* stack = &(calc_struct -> calc_stack);
    stack_t       value = Stack_Pop(stack);

    calc_struct -> register_buf[register_num] = value;

    return SUCCESS;
}

StackErr_t Stack_In (CalcStruct* calc_struct) {
    assert(calc_struct);

    printf("Enter value:\t");

    stack_struct* stack = &(calc_struct -> calc_stack);

    int value = 0;
    int scanf_checking = scanf("%d", &value);

    if (scanf_checking != 1) {
        printf("Incorrect input");
        return INCOR_INPUT;
    }

    Stack_Push(stack, value);

    return SUCCESS;
}

int Jump_to_JMP (size_t* ind, int cmd_num) {
    assert(ind);

    *ind = cmd_num;

    return SUCCESS;
}

#define JUMP_IF(operator, func_name)                                           \
                                                                               \
int Jump_##func_name (size_t* stack_ind, int cmd_num, stack_struct* stack) {   \
    assert(stack_ind);                                                         \
    assert(stack);                                                             \
                                                                               \
    int num1 = stack -> data[stack -> cur_position - 2];                       \
    int num2 = stack -> data[stack -> cur_position - 1];                       \
                                                                               \
    if (num1 operator num2)                                                    \
        Jump_to_JMP(stack_ind, cmd_num);                                       \
                                                                               \
    return SUCCESS;                                                            \
}

JUMP_IF(<, Below_JB)
JUMP_IF(<=, Below_Equal_JBE)
JUMP_IF(>, Above_JA)    
JUMP_IF(>=, Above_Equal_JAE)
JUMP_IF(==, Equal_JE)
JUMP_IF(!=, Not_Equal_JNE)


CalcErr_t Proc_Dtor (CalcStruct* calc_struct) {
    assert(calc_struct);

    size_t size_of_struct  = sizeof(*calc_struct);

    stack_struct stack     = calc_struct -> calc_stack;
    int* bite_code_pointer = calc_struct -> bite_code.buffer;

    Stack_Dtor(&stack);

    free(bite_code_pointer);
    
    memset(calc_struct, POISON, size_of_struct);

    return DESTROY_SUC;
}

CalcErr_t Proc_Verify (CalcStruct* calc_struct, const char* checking_function) {

    int error_code = 0;

    #ifdef DEBUG_STACK_VERIFY

        if (calc_struct == NULL) {

            fprintf(LogFile, "Calc_Verify: NULL pointer to CalcStruct" "\n");
            fprintf(LogFile, "Error code: %d (NULL_POINT_CALC)" "\n\n", NULL_POINT_CALC);

            Proc_Dump(calc_struct);

            error_code = error_code | NULL_POINT_CALC;

            fprintf(LogFile, "Calc_Verify: verification ended with error code: %d\n", error_code);
            return error_code;
        }
        
        if (calc_struct -> bite_code.size < 0) {

            fprintf(LogFile, "Calc_Verify: negative size of bite code buffer in %s" "\n", checking_function);
            fprintf(LogFile, "Error code: %d (NEG_BC_SIZE)" "\n\n", NEG_BC_SIZE);

            Proc_Dump(calc_struct);

            error_code = error_code | NEG_BC_SIZE;   
            
            fprintf(LogFile, "Calc_Verify: verification ended with error code: %d\n", error_code);
            return error_code;
        }
        
        if (calc_struct -> bite_code.buffer == NULL) {

            fprintf(LogFile, "Calc_Verify: NULL pointer to bite code buffer in %s" "\n", checking_function);
            fprintf(LogFile, "Error code: %d (NULL_POINT_BC_BUF)" "\n\n", NULL_POINT_BC_BUF);

            Proc_Dump(calc_struct);

            error_code = error_code | NULL_POINT_BC_BUF;     
            
            fprintf(LogFile, "Calc_Verify: verification ended with error code: %d\n", error_code);
            return error_code;
        }


        if (calc_struct -> register_buf == NULL) {

            fprintf(LogFile, "Calc_Verify: NULL poiner to register buffer in %s" "\n", checking_function);
            fprintf(LogFile, "Error code: %d (NULL_POINT_REG)" "\n\n", NULL_POINT_REG);

            Proc_Dump(calc_struct);

            error_code = error_code | NULL_POINT_REG;       
            
            fprintf(LogFile, "Calc_Verify: verification ended with error code: %d\n", error_code);
            return error_code;
        }        

    #endif

    return error_code;
}

CalcErr_t Proc_Dump(CalcStruct* calc_struct) {

    #if defined(DEBUG_STACK_VERIFY) || defined(DEBUG_CANARY) || defined(DEBUG_HASH)

        if (calc_struct == NULL) {
            fprintf(LogFile, "Calc_Dump: NULL pointer to CalcStruct" "\n\n");
            return DUMP_FAILED;
        }

        if (calc_struct -> bite_code.buffer == NULL) {
            fprintf(LogFile, "Calc_Dump: NULL pointer to bite code buffer" "\n\n");
            return DUMP_FAILED;
        }        

        if (calc_struct -> register_buf == NULL) {
            fprintf(LogFile, "Calc_Dump: NULL pointer to register buffer" "\n\n");
            return DUMP_FAILED;
        }

        fprintf(LogFile, "Calc_Dump prints:" "\n");
        fprintf(LogFile, "[%p]" "\n", calc_struct);
        fprintf(LogFile, "Bite code structure:" "\n");
        fprintf(LogFile, "{" "\n");
        fprintf(LogFile, "size = %d" "\n", calc_errors -> bite_code.size);
        fprintf(LogFile, "buffer:" "\n");
            fprintf(LogFile, "\t" "{" "\n");
        
                for (size_t el_num = 0; el_num < calc_struct -> bite_code.size; el_num++) 
                    fprintf(LogFile, "\t\t" "[%d] = %d" "\n", el_num, calc_struct -> bite_code.buffer[el_num]);
        
            fprintf(LogFile, "\t" "}" "\n");
        
        fprintf(LogFile, "Register buffer:" "\n");
            fprintf(LogFile, "\t" "{" "\n");
        
                for (size_t el_num = 0; el_num < COUNT_OF_REG; el_num++) 
                    fprintf(LogFile, "\t\t" "[%d] = %d" "\n", el_num, calc_struct -> register_buf[el_num]);
        
            fprintf(LogFile, "\t" "}" "\n");

        Stack_Dump( &(calculator -> calc_stack));
        
        fprintf(LogFile, "\n\n");
        
    #else
        
        return RELEASE_MODE;
        
    #endif
    
        return DUMP_SUCCESS;
}
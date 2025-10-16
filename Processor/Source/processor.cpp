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

void processor(ProcStruct* Proc_struct) { // each command has argument (it can be fictive (POISON))

    
    while (Proc_struct -> bite_code.ind_counter  <  Proc_struct -> bite_code.size &&
        Proc_struct -> bite_code.buffer[Proc_struct -> bite_code.ind_counter]  !=  CMD_HLT) 
        {
            
        int code_of_cmd = Proc_struct -> bite_code.buffer[Proc_struct -> bite_code.ind_counter];
        
        CmdStruct* info_of_cmd = find_cmd_in_arr(Proc_struct, code_of_cmd);

        info_of_cmd -> cmd_func(Proc_struct);

        Proc_struct -> bite_code.ind_counter += 2; // each command has argument (it can be fictive (POISON))
    }

    return;
}

#define STACK_ARIFMETIC(operator, func_name)                                 \
                                                                             \
CalcErr_t Stack_Arif_##func_name (ProcStruct* Proc_struct) {                 \
    assert(Proc_struct);                                                     \
                                                                             \
    int operand_1 = Stack_Pop( &(Proc_struct -> calc_stack));                \
    int operand_2 = Stack_Pop( &(Proc_struct -> calc_stack));                \
                                                                             \
    int result = (int) (operand_2 operator operand_1);                       \
                                                                             \
    Stack_Push(&(Proc_struct -> calc_stack), result);                        \
                                                                             \
    return CALC_SUCCESS;                                                     \
}

STACK_ARIFMETIC(+, Add)
STACK_ARIFMETIC(-, Sub)
STACK_ARIFMETIC(*, Mul)
STACK_ARIFMETIC(/, Div)

CalcErr_t Stack_Arif_Sqrt (ProcStruct* Proc_struct) {                       
    assert(Proc_struct);                                                
                                                                        
    int operand = Stack_Pop( &(Proc_struct -> calc_stack));                  
                                                                        
    int result = sqrt( (double) operand);              
                                                                        
    Stack_Push(&(Proc_struct -> calc_stack), result);                   
                                                                        
    return CALC_SUCCESS;                                                
}


StackErr_t Stack_Push_Proc (ProcStruct* Proc_struct) {
    assert(Proc_struct);

    stack_t push_arg    = Proc_struct -> bite_code.buffer[Proc_struct -> bite_code.ind_counter + 1];
    stack_struct* stack = &(Proc_struct -> calc_stack);

    return Stack_Push(stack, push_arg);
}

StackErr_t Stack_Pop_Proc (ProcStruct* Proc_struct) {
    assert(Proc_struct);

    stack_struct* stack = &(Proc_struct -> calc_stack);

    return Stack_Pop(stack);
}

CalcErr_t Stack_PushR (ProcStruct* Proc_struct) {
    assert(Proc_struct);
    
    int register_num = Proc_struct -> bite_code.buffer[Proc_struct -> bite_code.ind_counter + 1];
    assert(register_num >= 0 && register_num <= COUNT_OF_REG);

    stack_t       value =   Proc_struct -> register_buf[register_num];
    stack_struct* stack = &(Proc_struct -> calc_stack);

    return Stack_Push(stack, value);
}

CalcErr_t Stack_PopR (ProcStruct* Proc_struct) {
    assert(Proc_struct);

    int register_num = Proc_struct -> bite_code.buffer[Proc_struct -> bite_code.ind_counter + 1];
    assert(register_num >= 0 && register_num <= COUNT_OF_REG);

    stack_struct* stack = &(Proc_struct -> calc_stack);
    stack_t       value = Stack_Pop(stack);

    Proc_struct -> register_buf[register_num] = value;

    return SUCCESS;
}

CalcErr_t Stack_In (ProcStruct* Proc_struct) {
    assert(Proc_struct);

    printf("Enter value:\t");

    stack_struct* stack = &(Proc_struct -> calc_stack);

    int value = 0;
    int scanf_checking = scanf("%d", &value);

    if (scanf_checking != 1) {
        printf("Incorrect input");
        return INCOR_INPUT;
    }

    Stack_Push(stack, value);

    return SUCCESS;
}

CalcErr_t Jump_to_JMP (ProcStruct* Proc_struct) {
    assert(Proc_struct);

    int ind_to_jump = Proc_struct -> bite_code.buffer[Proc_struct -> bite_code.ind_counter + 1];

    Proc_struct -> bite_code.ind_counter = ind_to_jump;

    Proc_struct -> bite_code.ind_counter -= 2;

    return SUCCESS;
}

#define JUMP_IF(operator, func_name)                                                        \
                                                                                            \
CalcErr_t Jump_##func_name (ProcStruct* Proc_struct) {                                      \
    assert(Proc_struct);                                                                    \
                                                                                            \
    int num1 = Proc_struct -> calc_stack.data[Proc_struct -> calc_stack.cur_position - 2];  \
    int num2 = Proc_struct -> calc_stack.data[Proc_struct -> calc_stack.cur_position - 1];  \
                                                                                            \
    if (num1 operator num2)                                                                 \
        Jump_to_JMP(Proc_struct);                                                           \
                                                                                            \
    return SUCCESS;                                                                         \
}

JUMP_IF(<, Below_JB)
JUMP_IF(<=, Below_Equal_JBE)
JUMP_IF(>, Above_JA)    
JUMP_IF(>=, Above_Equal_JAE)
JUMP_IF(==, Equal_JE)
JUMP_IF(!=, Not_Equal_JNE)

CalcErr_t Call_command (ProcStruct* Proc_struct) {
    assert(Proc_struct);

    int remember_ind = Proc_struct -> bite_code.ind_counter + 2;
    
    stack_struct* stack = &(Proc_struct -> calc_stack);
    
    Stack_Push(stack, remember_ind);

    Jump_to_JMP(Proc_struct);

    return SUCCESS;
}

CalcErr_t Return_to_call_RET (ProcStruct* Proc_struct) {
    assert(Proc_struct);

    stack_struct* stack = &(Proc_struct -> calc_stack);

    int ind_to_return = Stack_Pop(stack);
    
    Proc_struct -> bite_code.ind_counter = ind_to_return;

    Proc_struct -> bite_code.ind_counter -= 2;    

    return SUCCESS;
}

CalcErr_t Push_from_RAM_PUSHM(ProcStruct* Proc_struct) {
    assert(Proc_struct);

    int register_num = Proc_struct -> bite_code.buffer[Proc_struct -> bite_code.ind_counter + 1];
    
    size_t RAM_ind   = Proc_struct -> register_buf[register_num];

    stack_struct* stack = &(Proc_struct -> calc_stack);
    int pushing_value   =   Proc_struct -> RAM_buf[RAM_ind];

    Stack_Push(stack, pushing_value);

    return SUCCESS;
}

CalcErr_t Pop_to_RAM_POPM(ProcStruct* Proc_struct) {
    assert(Proc_struct);

    int register_num = Proc_struct -> bite_code.buffer[Proc_struct -> bite_code.ind_counter + 1];
    
    size_t RAM_ind   = Proc_struct -> register_buf[register_num];

    stack_struct* stack = &(Proc_struct -> calc_stack);

    int popping_value = Stack_Pop(stack);

    Proc_struct -> RAM_buf[RAM_ind] = popping_value;

    return SUCCESS;
}

CalcErr_t Proc_Dtor (ProcStruct* Proc_struct) {
    assert(Proc_struct);

    size_t size_of_struct  = sizeof(*Proc_struct);

    stack_struct stack     = Proc_struct -> calc_stack;
    int* bite_code_pointer = Proc_struct -> bite_code.buffer;

    Stack_Dtor(&stack);

    free(bite_code_pointer);
    
    memset(Proc_struct, POISON, size_of_struct);

    return DESTROY_SUC;
}

CalcErr_t Proc_Verify (ProcStruct* Proc_struct, const char* checking_function) {

    int error_code = 0;

    #ifdef DEBUG_STACK_VERIFY

        if (Proc_struct == NULL) {

            fprintf(LogFile, "Calc_Verify: NULL pointer to ProcStruct" "\n");
            fprintf(LogFile, "Error code: %d (NULL_POINT_CALC)" "\n\n", NULL_POINT_CALC);

            Proc_Dump(Proc_struct);

            error_code = error_code | NULL_POINT_CALC;

            fprintf(LogFile, "Calc_Verify: verification ended with error code: %d\n", error_code);
            return error_code;
        }
        
        if (Proc_struct -> bite_code.size < 0) {

            fprintf(LogFile, "Calc_Verify: negative size of bite code buffer in %s" "\n", checking_function);
            fprintf(LogFile, "Error code: %d (NEG_BC_SIZE)" "\n\n", NEG_BC_SIZE);

            Proc_Dump(Proc_struct);

            error_code = error_code | NEG_BC_SIZE;   
            
            fprintf(LogFile, "Calc_Verify: verification ended with error code: %d\n", error_code);
            return error_code;
        }
        
        if (Proc_struct -> bite_code.buffer == NULL) {

            fprintf(LogFile, "Calc_Verify: NULL pointer to bite code buffer in %s" "\n", checking_function);
            fprintf(LogFile, "Error code: %d (NULL_POINT_BC_BUF)" "\n\n", NULL_POINT_BC_BUF);

            Proc_Dump(Proc_struct);

            error_code = error_code | NULL_POINT_BC_BUF;     
            
            fprintf(LogFile, "Calc_Verify: verification ended with error code: %d\n", error_code);
            return error_code;
        }


        if (Proc_struct -> register_buf == NULL) {

            fprintf(LogFile, "Calc_Verify: NULL poiner to register buffer in %s" "\n", checking_function);
            fprintf(LogFile, "Error code: %d (NULL_POINT_REG)" "\n\n", NULL_POINT_REG);

            Proc_Dump(Proc_struct);

            error_code = error_code | NULL_POINT_REG;       
            
            fprintf(LogFile, "Calc_Verify: verification ended with error code: %d\n", error_code);
            return error_code;
        }        

    #endif

    return error_code;
}

CalcErr_t Proc_Dump(ProcStruct* Proc_struct) {

    #if defined(DEBUG_STACK_VERIFY) || defined(DEBUG_CANARY) || defined(DEBUG_HASH)

        if (Proc_struct == NULL) {
            fprintf(LogFile, "Calc_Dump: NULL pointer to ProcStruct" "\n\n");
            return DUMP_FAILED;
        }

        if (Proc_struct -> bite_code.buffer == NULL) {
            fprintf(LogFile, "Calc_Dump: NULL pointer to bite code buffer" "\n\n");
            return DUMP_FAILED;
        }        

        if (Proc_struct -> register_buf == NULL) {
            fprintf(LogFile, "Calc_Dump: NULL pointer to register buffer" "\n\n");
            return DUMP_FAILED;
        }

        fprintf(LogFile, "Calc_Dump prints:" "\n");
        fprintf(LogFile, "[%p]" "\n", Proc_struct);
        fprintf(LogFile, "Bite code structure:" "\n");
        fprintf(LogFile, "{" "\n");
        fprintf(LogFile, "size = %d" "\n", calc_errors -> bite_code.size);
        fprintf(LogFile, "buffer:" "\n");
            fprintf(LogFile, "\t" "{" "\n");
        
                for (size_t el_num = 0; el_num < Proc_struct -> bite_code.size; el_num++) 
                    fprintf(LogFile, "\t\t" "[%d] = %d" "\n", el_num, Proc_struct -> bite_code.buffer[el_num]);
        
            fprintf(LogFile, "\t" "}" "\n");
        
        fprintf(LogFile, "Register buffer:" "\n");
            fprintf(LogFile, "\t" "{" "\n");
        
                for (size_t el_num = 0; el_num < COUNT_OF_REG; el_num++) 
                    fprintf(LogFile, "\t\t" "[%d] = %d" "\n", el_num, Proc_struct -> register_buf[el_num]);
        
            fprintf(LogFile, "\t" "}" "\n");

        Stack_Dump( &(calculator -> calc_stack));
        
        fprintf(LogFile, "\n\n");
        
    #else
        
        return RELEASE_MODE;
        
    #endif
    
        return DUMP_SUCCESS;
}
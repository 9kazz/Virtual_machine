#ifndef PROCESSOR_H
#define PROCESSOR_H

#include "utils.h"
#include "LogFile.h"
#include "type_struct_def.h"    
#include "stack_utils.h"
#include "stack.h"

void      processor    (CalcStruct*   calc_struct);

int       Stack_Calc   (stack_struct* stack, int  arif_operator);

CalcErr_t Proc_Dtor    (CalcStruct* calc_struct);
CalcErr_t Proc_Verify (CalcStruct* calc_struct, const char* checking_function);
CalcErr_t Proc_Dump(CalcStruct* calc_struct) ;

StackErr_t Stack_PushR (CalcStruct* calc_struct, int register_num);
StackErr_t Stack_PopR  (CalcStruct* calc_struct, int register_num);
StackErr_t Stack_In    (CalcStruct* calc_struct);

int Jump_to_JMP          (size_t* stack_ind, int cmd_num);
int Jump_Below_JB        (size_t* stack_ind, int cmd_num, stack_struct* stack);
int Jump_Below_Equal_JBE (size_t* stack_ind, int cmd_num, stack_struct* stack);
int Jump_Above_JA        (size_t* stack_ind, int cmd_num, stack_struct* stack);
int Jump_Above_Equal_JAE (size_t* stack_ind, int cmd_num, stack_struct* stack);
int Jump_Equal_JE        (size_t* stack_ind, int cmd_num, stack_struct* stack);
int Jump_Not_Equal_JNE   (size_t* stack_ind, int cmd_num, stack_struct* stack);

#endif
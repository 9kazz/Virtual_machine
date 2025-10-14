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
CalcErr_t Proc_Verify  (CalcStruct* calc_struct, const char* checking_function);
CalcErr_t Proc_Dump    (CalcStruct* calc_struct);

StackErr_t Stack_Push_Proc (CalcStruct* calc_struct);
StackErr_t Stack_Pop_Proc  (CalcStruct* calc_struct);

StackErr_t Stack_PushR   (CalcStruct* calc_struct);
StackErr_t Stack_PopR    (CalcStruct* calc_struct);
StackErr_t Stack_In      (CalcStruct* calc_struct);

int Jump_to_JMP          (CalcStruct* calc_struct);
int Jump_Below_JB        (CalcStruct* calc_struct);
int Jump_Below_Equal_JBE (CalcStruct* calc_struct);
int Jump_Above_JA        (CalcStruct* calc_struct);
int Jump_Above_Equal_JAE (CalcStruct* calc_struct);
int Jump_Equal_JE        (CalcStruct* calc_struct);
int Jump_Not_Equal_JNE   (CalcStruct* calc_struct);

#endif
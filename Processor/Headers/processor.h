#ifndef PROCESSOR_H
#define PROCESSOR_H

#include "utils.h"
#include "LogFile.h"
#include "type_struct_def.h"    
#include "stack_utils.h"
#include "stack.h"

void      processor    (CalcStruct*   calc_struct);

CalcErr_t Proc_Dtor    (CalcStruct* calc_struct);
CalcErr_t Proc_Verify  (CalcStruct* calc_struct, const char* checking_function);
CalcErr_t Proc_Dump    (CalcStruct* calc_struct);

StackErr_t Stack_Push_Proc (CalcStruct* calc_struct);
StackErr_t Stack_Pop_Proc  (CalcStruct* calc_struct);

CalcErr_t Stack_Arif_Add  (CalcStruct* calc_struct);
CalcErr_t Stack_Arif_Sub  (CalcStruct* calc_struct);
CalcErr_t Stack_Arif_Mul  (CalcStruct* calc_struct);
CalcErr_t Stack_Arif_Div  (CalcStruct* calc_struct);
CalcErr_t Stack_Arif_Sqrt (CalcStruct* calc_struct);

CalcErr_t Stack_PushR   (CalcStruct* calc_struct);
CalcErr_t Stack_PopR    (CalcStruct* calc_struct);
CalcErr_t Stack_In      (CalcStruct* calc_struct);

CalcErr_t Jump_to_JMP          (CalcStruct* calc_struct);
CalcErr_t Jump_Below_JB        (CalcStruct* calc_struct);
CalcErr_t Jump_Below_Equal_JBE (CalcStruct* calc_struct);
CalcErr_t Jump_Above_JA        (CalcStruct* calc_struct);
CalcErr_t Jump_Above_Equal_JAE (CalcStruct* calc_struct);
CalcErr_t Jump_Equal_JE        (CalcStruct* calc_struct);
CalcErr_t Jump_Not_Equal_JNE   (CalcStruct* calc_struct);

#endif
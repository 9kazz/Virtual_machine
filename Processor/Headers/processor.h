#ifndef PROCESSOR_H
#define PROCESSOR_H

#include "utils.h"
#include "LogFile.h"
#include "type_struct_def.h"    
#include "stack_utils.h"
#include "stack.h"

void      processor    (ProcStruct*   Proc_struct);

CalcErr_t Proc_Dtor    (ProcStruct* Proc_struct);
CalcErr_t Proc_Verify  (ProcStruct* Proc_struct, const char* checking_function);
CalcErr_t Proc_Dump    (ProcStruct* Proc_struct);

StackErr_t Stack_Push_Proc (ProcStruct* Proc_struct);
StackErr_t Stack_Pop_Proc  (ProcStruct* Proc_struct);

CalcErr_t Stack_Arif_Add  (ProcStruct* Proc_struct);
CalcErr_t Stack_Arif_Sub  (ProcStruct* Proc_struct);
CalcErr_t Stack_Arif_Mul  (ProcStruct* Proc_struct);
CalcErr_t Stack_Arif_Div  (ProcStruct* Proc_struct);
CalcErr_t Stack_Arif_Sqrt (ProcStruct* Proc_struct);

CalcErr_t Stack_PushR   (ProcStruct* Proc_struct);
CalcErr_t Stack_PopR    (ProcStruct* Proc_struct);
CalcErr_t Stack_In      (ProcStruct* Proc_struct);

CalcErr_t Call_command       (ProcStruct* Proc_struct);
CalcErr_t Return_to_call_RET (ProcStruct* Proc_struct);

CalcErr_t Jump_to_JMP          (ProcStruct* Proc_struct);
CalcErr_t Jump_Below_JB        (ProcStruct* Proc_struct);
CalcErr_t Jump_Below_Equal_JBE (ProcStruct* Proc_struct);
CalcErr_t Jump_Above_JA        (ProcStruct* Proc_struct);
CalcErr_t Jump_Above_Equal_JAE (ProcStruct* Proc_struct);
CalcErr_t Jump_Equal_JE        (ProcStruct* Proc_struct);
CalcErr_t Jump_Not_Equal_JNE   (ProcStruct* Proc_struct);

#endif
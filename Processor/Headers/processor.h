#ifndef CALCULATOR_H
#define CALCULATOR_H

#include "utils.h"
#include "type_struct_def.h"    
#include "LogFile.h"
#include "stack_utils.h"
#include "stack.h"

CalcErr_t processor(CalcStruct*   calc_struct);

CalcErr_t Stack_Calc(stack_struct* stack, int  arif_operator);

CalcErr_t Stack_PushR (CalcStruct* calc_struct, int register_num);
CalcErr_t Stack_PopR  (CalcStruct* calc_struct, int register_num);
CalcErr_t Stack_In    (CalcStruct* calc_struct);

CalcErr_t JMP ( size_t* ind, int cmd_num);

#endif
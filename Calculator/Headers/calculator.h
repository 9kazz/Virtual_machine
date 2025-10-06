#ifndef CALCULATOR_H
#define CALCULATOR_H

#include "utils.h"
#include "LogFile.h"
#include "type_struct_def.h"    
#include "stack_utils.h"
#include "stack.h"

void calculator(CalcStruct*   calc_struct);

int  Stack_Calc(stack_struct* stack, int  arif_operator);

StackErr_t Stack_PushR (CalcStruct* calc_struct, int register_num);
StackErr_t Stack_PopR  (CalcStruct* calc_struct, int register_num);
StackErr_t Stack_In    (CalcStruct* calc_struct);

#endif
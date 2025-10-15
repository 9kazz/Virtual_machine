#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>

#include "LogFile.h"
#include "type_struct_def.h"    
#include "stack_utils.h"
#include "stack.h"


                            /*CONSTANTS*/

const int COUNT_OF_REG          = 8;
const int MAX_COUNT_OF_CMD      = 32;
const int RETURN_STACK_CAPACITY = 32;


enum errors_and_success {
    UNKNOWN_COM = -100,
    INCOR_INPUT = -101, 
    DESTROY_SUC =  100, 
    SUCCESS     =  777
};

enum commands {
    CMD_HLT         =  0,
    CMD_PUSH        =  1,
    CMD_OUT         =  2,
    CMD_ADD         =  3,
    CMD_SUB         =  4,
    CMD_MUL         =  5,
    CMD_DIV         =  6,
    CMD_SQRT        =  7,
    CMD_PUSHR       = 33,
    CMD_POPR        = 34,
    CMD_IN          = 35,
    CMD_JMP         = 64,
    CMD_JB          = 65,
    CMD_JBE         = 66,
    CMD_JA          = 67,
    CMD_JAE         = 68,
    CMD_JE          = 69,
    CMD_JNE         = 70,
    CMD_CALL        = 71,
    CMD_RET         = 72
};


                            /*STRUCTURES*/
struct ProcStruct;
struct CmdStruct;
struct BiteCodeStruct;

struct  BiteCodeStruct {
    int*   buffer;
    size_t size;
};

struct CmdStruct {
    const char* name;
    int         code;
    int       (*cmd_func) (ProcStruct*);
};

struct ProcStruct {
    CmdStruct*     cmd_info_arr;
    BiteCodeStruct bite_code;
    size_t         ind_counter;
    stack_struct   calc_stack;
    stack_struct   return_stack;
    int*           register_buf;   
};


                            /*DEFINES*/

#define CALC_CTOR(name)                                                                        \
                                                                                               \
    CmdStruct* name##_cmd_info_arr = create_cmd_info_arr();                                    \
                                                                                               \
    size_t name##_bite_code_size = 0;                                                          \
    int*   name##_bite_code_buf  = create_bite_code_buf(input_file, &name##_bite_code_size);   \
                                                                                               \
    BiteCodeStruct name##_bite_code_struct {};                                                 \
        name##_bite_code_struct.buffer = name##_bite_code_buf;                                 \
        name##_bite_code_struct.size   = name##_bite_code_size;                                \
                                                                                               \
    STK_CTOR(name##_stack, name##_bite_code_size)                                              \
                                                                                               \
    STK_CTOR(name##_return_stack, RETURN_STACK_CAPACITY)                                       \
                                                                                               \
    int name##_reg_arr[8] = {0};                                                               \
                                                                                               \
    ProcStruct name{};                                                                         \
        name.cmd_info_arr = name##_cmd_info_arr;                                               \
        name.bite_code    = name##_bite_code_struct;                                           \
        name.ind_counter  = 0;                                                                 \
        name.calc_stack   = name##_stack;                                                      \
        name.return_stack = name##_return_stack;                                               \
        name.register_buf = name##_reg_arr;                                                    
    
#define SAFE_CALLOC(name, size_of_buf, el_type)                                     \
    el_type* temp_##name = (el_type*) calloc(size_of_buf, sizeof(el_type));         \
                                                                                    \
    if (temp_##name == NULL)                                                        \
        fprintf(stderr, "Allocation error of" #name);                               \
                                                                                    \
    el_type* name = temp_##name;



                            /*FUNCTIONS*/

CmdStruct* create_cmd_info_arr (void);
CmdStruct* find_cmd_in_arr     (ProcStruct* Proc_struct, int code_of_cmd);

int*   create_bite_code_buf (FILE* input_file, size_t* bite_code_size);

size_t size_of_file         (FILE* file_input);

#endif
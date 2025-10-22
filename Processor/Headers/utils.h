#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>

#include "LogFile.h"
#include "type_struct_def.h"    
#include "stack_utils.h"
#include "stack.h"


                            /*CONSTANTS*/

const int COUNT_OF_REG          =  8;
const int MAX_COUNT_OF_CMD      =  128;
const int RETURN_STACK_CAPACITY =  128;
const int RAM_SIZE_X            =  40;
const int RAM_SIZE_Y            =  40;
const int CAPASITY_OF_RAM       =  RAM_SIZE_X * RAM_SIZE_Y + 1;


enum errors_and_success {
    UNKNOWN_COM       = -100,
    INCOR_INPUT       = -101, 
    DESTROY_SUC       =  100, 
    SUCCESS           =  777,
    FAIL              =  -1,
    NULL_POINT_PROC   = -2,
    NEG_BC_SIZE       = -3,
    NULL_POINT_BC_BUF = -4,
    NULL_POINT_REG    = -5,
    NULL_POINT_RAM    = -6
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
    CMD_RET         = 72,
    CMD_PUSHM       = 73,
    CMD_POPM        = 74
};


                            /*STRUCTURES*/
struct ProcStruct;
struct CmdStruct;
struct BiteCodeStruct;

struct  BiteCodeStruct {
    int*   buffer;
    size_t size;
    size_t ind_counter;
};

struct CmdStruct {
    const char* name;
    int         code;
    int       (*cmd_func) (ProcStruct*);
};

struct ProcStruct {
    CmdStruct*     cmd_info_arr;
    BiteCodeStruct bite_code;
    stack_struct   calc_stack;
    stack_struct   return_stack;
    int*           register_buf;   
    int*           RAM_buf;
};


                            /*DEFINES*/

#define SPU_CTOR(name)                                                                        \
                                                                                               \
    int RAM[CAPASITY_OF_RAM] = {0};                                                            \
                                                                                               \
    CmdStruct* name##_cmd_info_arr = create_cmd_info_arr();                                    \
                                                                                               \
    size_t name##_bite_code_size = 0;                                                          \
    int*   name##_bite_code_buf  = create_bite_code_buf(input_file, &name##_bite_code_size);   \
                                                                                               \
    BiteCodeStruct name##_bite_code_struct {};                                                 \
        name##_bite_code_struct.buffer      = name##_bite_code_buf;                            \
        name##_bite_code_struct.size        = name##_bite_code_size;                           \
        name##_bite_code_struct.ind_counter = 0;                                               \
                                                                                               \
    STK_CTOR(name##_stack, name##_bite_code_size)                                              \
                                                                                               \
    STK_CTOR(name##_return_stack, RETURN_STACK_CAPACITY)                                       \
                                                                                               \
    int name##_reg_arr[COUNT_OF_REG] = {0};                                                    \
                                                                                               \
    ProcStruct name{};                                                                         \
        name.cmd_info_arr = name##_cmd_info_arr;                                               \
        name.bite_code    = name##_bite_code_struct;                                           \
        name.calc_stack   = name##_stack;                                                      \
        name.return_stack = name##_return_stack;                                               \
        name.register_buf = name##_reg_arr;                                                    \
        name.RAM_buf      = RAM;
    
#define SAFE_CALLOC(name, size_of_buf, el_type)                                     \
    el_type* temp_##name = (el_type*) calloc(size_of_buf, sizeof(el_type));         \
                                                                                    \
    if (temp_##name == NULL)                                                        \
        fprintf(stderr, "Allocation error of" #name);                               \
                                                                                    \
    el_type* name = temp_##name;



                            /*FUNCTIONS*/

CmdStruct* create_cmd_info_arr (void);

int*   create_bite_code_buf (FILE* input_file, size_t* bite_code_size);

size_t size_of_file         (FILE* file_input);

CalcErr_t RAM_Draw_in_file  (FILE* draw_file, ProcStruct* Proc_struct);

void command_line_flags     (int argc, char* argv[], FILE** input_file);

#endif
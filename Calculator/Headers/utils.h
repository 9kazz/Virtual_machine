#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>

#include "LogFile.h"
#include "type_struct_def.h"    
#include "stack_utils.h"
#include "stack.h"

enum errors_and_success {
    UNKNOWN_COM = -100,
    INCOR_INPUT = -101, 
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
    CMD_IN          = 35
};

const int COUNT_OF_REG = 8;

struct  BiteCodeStruct {
    int*   buffer;
    size_t size;
};

struct CalcStruct {
    BiteCodeStruct bite_code;
    stack_struct   calc_stack;
    int*           register_buf;    
};


#define CALC_CTOR(name)                                                                        \
    size_t name##_bite_code_size = 0;                                                          \
    int*   name##_bite_code_buf  = create_bite_code_buf(input_file, &name##_bite_code_size);   \
                                                                                               \
    BiteCodeStruct name##_bite_code_struct {};                                                 \
        name##_bite_code_struct.buffer = name##_bite_code_buf;                                 \
        name##_bite_code_struct.size   = name##_bite_code_size;                                \
                                                                                               \
    STK_CTOR(name##_stack, name##_bite_code_size)                                              \
                                                                                               \
    int name##_reg_arr[8] = {0};                                                               \
                                                                                               \
    CalcStruct name{};                                                                         \
        name.bite_code    = name##_bite_code_struct;                                           \
        name.calc_stack   = name##_stack;                                                      \
        name.register_buf = name##_reg_arr;
    

int*   create_bite_code_buf (FILE* input_file, size_t* bite_code_size);

size_t size_of_file         (FILE* file_input);

#endif
#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>

#include "LogFile.h"
#include "type_struct_def.h"    
#include "stack_utils.h"
#include "stack.h"

enum errors {
    UNKNOWN_COM = -100
};

enum commands {
    CMD_HLT         =  0,
    CMD_PUSH        =  1,
    CMD_OUT         =  2,
    CMD_ADD         =  3,
    CMD_SUB         =  4,
    CMD_MUL         =  5,
    CMD_DIV         =  6,
    CMD_SQRT        =  7
};

struct  BiteCodeStruct {
    int*   buffer;
    size_t size;
};

struct CalcStruct {
    BiteCodeStruct      bite_code;
    stack_struct        calc_stack;
};


#define CREATE_CALC_STRUCT(name)                                                               \
    size_t name##_bite_code_size = 0;                                                          \
    int*   name##_bite_code_buf  = create_bite_code_buf(input_file, &name##_bite_code_size);   \
                                                                                               \
    BiteCodeStruct name##_bite_code_struct {};                                                 \
        name##_bite_code_struct.buffer = name##_bite_code_buf;                                 \
        name##_bite_code_struct.size   = name##_bite_code_size;                                \
                                                                                               \
    STK_CTOR(name##_stack, name##_bite_code_size)                                              \
                                                                                               \
    CalcStruct name{};                                                                         \
        name.bite_code  = name##_bite_code_struct;                                             \
        name.calc_stack = name##_stack;
    

int* create_bite_code_buf (FILE* input_file, size_t* bite_code_size);

#endif
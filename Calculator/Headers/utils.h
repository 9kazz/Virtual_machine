#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>


enum errors {
    UNKNOWN_COM = -100
};

enum success {
    CALC_SUCCESS = 100
};

enum commands {
    HLT         =  0,
    POP         =  1,
    PUSH        =  2,
    OUT         =  3,
    ADD         =  4,
    SUB         =  5,
    MUL         =  6,
    DIV         =  7
};

const int POISON = 0xDEADFACE;
int* create_bit_code_buf (FILE* input_file, size_t* bit_code_size);

#endif
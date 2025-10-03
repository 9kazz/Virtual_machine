#ifndef ASSEMBLER_H
#define ASSEMBLER_H

enum errors {
    UNKNOWN_COM = -1
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

const int POISON           = 0xDEADFACE;
const int COMMAND_MAX_LEN  = 32;

int*  fill_bit_code_buf (char** pointers_array, size_t count_of_lines, int*    bit_code_buf);

int*  assembler         (char** pointers_array, size_t count_of_lines, size_t* bit_code_capacity);

int   command_identify  (const char* command);

#endif
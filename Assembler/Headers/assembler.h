#ifndef ASSEMBLER_H
#define ASSEMBLER_H

enum errors {
    UNKNOWN_COM = -1
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

const int POISON           = 0xDEADFACE;
const int COMMAND_MAX_LEN  = 32;

int*  fill_bit_code_buf (char** pointers_array, size_t count_of_lines, int*    bit_code_buf);

int*  assembler         (char** pointers_array, size_t count_of_lines, size_t* bit_code_capacity);

int   command_identify  (const char* command);

#endif
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
    CMD_JNE         = 70
};

const int POISON           = 0xDEADFACE;
const int COMMAND_MAX_LEN  = 32;
const int COUNT_OF_REG     =  8;
const int LABEL_BUF_SIZE   = 10;

int* fill_byte_code_buf  (char**     pointers_array, size_t count_of_lines, int*        byte_code_buf,       int* label_array);
int* assembler          (char**      pointers_array, size_t count_of_lines, size_t*     byte_code_capacity);
int  command_identify   (const char* command_str);
int  argument_identify  (int         count_of_arg,   int    command_int,    const char* argument_str,        int* label_array);

int  register_num       (const char* argument_str);

int* create_label_array (void);
int* fill_label_array   (char**      pointers_array, size_t count_of_lines, int* label_array);
int  identify_label     (const char* argument_str,   int*   label_array);


#endif
#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include <stdio.h>
#include "utils.h"

enum function_status {
    END_SUCCESS = 1,
    UNKNOWN_COM = -1,
    NOT_LABEL   = -2,
    IS_LABEL    =  2,
    DESTROY_SUC = 666
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

const int POISON           = 0xDEADFACE;
const int COMMAND_MAX_LEN  = 32;
const int COUNT_OF_REG     =  8;
const int LABEL_BUF_SIZE   = 10;

int    assembler          (asm_struct* Assembler);
size_t fill_byte_code_buf (asm_struct* Assembler);
int    command_identify   (const char* command_str);
int    argument_identify  (asm_struct* Assembler, int count_of_arg, int command_int,  const char* argument_str);

int    register_num       (const char* argument_str);

int    fill_label_array   (asm_struct* Assembler, char*       command_str,  size_t* count_of_commands_without_labeles);
int    identify_label     (asm_struct* Assembler, const char* argument_str);
int    is_label           (char*       string);

int    indentify_register_RAM (char* argument_str);

#define ASM_STRUCT_INIT(name)                                \
    asm_struct name;                                         \
        name.byte_code_buf     = NULL;                       \
        name.asm_code_buf      = NULL;                       \
        name.pointers_array    = NULL;                       \
        name.count_of_commands = 0;                          \
        name.ind_counter       = 0; 

#endif
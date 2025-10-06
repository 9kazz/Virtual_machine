#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>

#include "assembler.h"
#include "utils.h"

int* assembler (char** pointers_array, size_t count_of_lines, size_t* bit_code_capacity) {

    *bit_code_capacity = 2 * count_of_lines;

    int* bit_code_buf = (int*) calloc(*bit_code_capacity, sizeof(char)); //??

    fill_bit_code_buf (pointers_array, count_of_lines, bit_code_buf);  // TODO: обработка ошибок

    return bit_code_buf;
}

int* fill_bit_code_buf (char** pointers_array, size_t count_of_lines, int* bit_code_buf) {  // TODO: возврат ошибок

    char command_str [COMMAND_MAX_LEN] = {0}; 
    char argument_str[COMMAND_MAX_LEN] = {0};

    size_t str_num         = 0;
    size_t bit_code_index  = 0;
    int    count_of_arg    = 0;

    while (str_num < count_of_lines && 
           pointers_array[str_num] != NULL) 
    {
        count_of_arg = sscanf( (const char*) pointers_array[str_num], "%32s %32s", command_str, argument_str); // COMMAND_MAX_LEN = 32

        int  command_int = command_identify( (const char*) command_str); // TODO: UNKNOW_COM err analise
        int argument_int = argument_identify(count_of_arg, command_int, (const char*) argument_str);
        
        switch (count_of_arg)
        {
            case 1:  // one argument
                argument_int = POISON;
                break;

            case 2: // two arguments
                break;

            default:
                fprintf(stderr, "Incorrect ASM-code");
                return NULL;       
        }

        bit_code_buf[ bit_code_index++ ] = command_int;
        bit_code_buf[ bit_code_index++ ] = argument_int;

        ++str_num;
    }

    return bit_code_buf;
}

int command_identify (const char* command_str) {
    
    if (strcmp(command_str, "PUSH") == 0) {
        return CMD_PUSH;

    } else if (strcmp(command_str, "ADD") == 0) {
        return CMD_ADD;

    } else if (strcmp(command_str, "SUB") == 0) {
        return CMD_SUB;

    } else if (strcmp(command_str, "MUL") == 0) {
        return CMD_MUL;

    } else if (strcmp(command_str, "DIV") == 0) {
        return CMD_DIV;
    
    } else if (strcmp(command_str, "OUT") == 0) {
        return CMD_OUT;
    
    } else if (strcmp(command_str, "HLT") == 0) {
        return CMD_HLT;
    
    } else if (strcmp(command_str, "SQRT") == 0) {
        return CMD_SQRT;

    } else if (strcmp(command_str, "PUSHR") == 0) {
        return CMD_PUSHR;

    } else if (strcmp(command_str, "POPR") == 0) {
        return CMD_POPR;

    } else if (strcmp(command_str, "IN") == 0) {
        return CMD_IN;
    } else 
        return UNKNOWN_COM;
}

int argument_identify (int count_of_arg, int command_int, const char* argument_str) {

    switch (count_of_arg)
    {
        case 1:  // one argument
            return POISON;
            break;

        case 2: // two arguments

            if (command_int == CMD_PUSHR ||
                command_int == CMD_POPR  ||
                command_int == CMD_IN)   
            {
                return register_num(argument_str);
            } 
            
            return atoi(argument_str);

        default:
            break;
    }

    fprintf(stderr, "Incorrect ASM-code");
    return POISON;
}

int register_num (const char* argument_str) {

    const char* first_reg = "RAX";

    int offset_from_first_reg = my_strcmp(argument_str, first_reg);

    if (offset_from_first_reg < 0 ||
        offset_from_first_reg > COUNT_OF_REG) 
    {
        fprintf(stderr, "Register_num: incorrect register");
        return 0;
    }

    return offset_from_first_reg;
}

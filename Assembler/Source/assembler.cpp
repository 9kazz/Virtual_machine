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

    char  command[COMMAND_MAX_LEN] = {0}; 
    int   argument                 =  POISON;

    size_t str_num         = 0;
    size_t bit_code_index  = 0;
    int    sscanf_checking = 0;

    while (str_num < count_of_lines && 
           pointers_array[str_num] != NULL) 
    {
        sscanf_checking = sscanf( (const char*) pointers_array[str_num], "%32s %32d", command, &argument); // COMMAND_MAX_LEN = 32

        int code_of_com = command_identify( (const char*) command); // TODO: UNKNOW_COM err analise
        
        switch (sscanf_checking)
        {
            case 1: 
                argument = POISON;
                break;

            case 2:
                break;

            default:
                fprintf(stderr, "Incorrect ASM-code");
                return NULL;       
        }

        bit_code_buf[ bit_code_index++ ] = code_of_com;
        bit_code_buf[ bit_code_index++ ] = argument;

        ++str_num;
    }

    return bit_code_buf;
}

int command_identify (const char* command) {
    
    if (strcmp(command, "PUSH") == 0) {
        return PUSH;

    } else if (strcmp(command, "ADD") == 0) {
        return ADD;

    } else if (strcmp(command, "SUB") == 0) {
        return SUB;

    } else if (strcmp(command, "MUL") == 0) {
        return MUL;

    } else if (strcmp(command, "DIV") == 0) {
        return DIV;
    
    } else if (strcmp(command, "OUT") == 0) {
        return OUT;
    
    } else if (strcmp(command, "HLT") == 0) {
        return HLT;
    
    } else 
        return UNKNOWN_COM;
}


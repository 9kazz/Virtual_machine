#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>

#include "assembler.h"
#include "utils.h"

int* assembler (char** pointers_array, size_t count_of_lines, size_t* byte_code_capacity) {
    assert(pointers_array);
    assert(fill_byte_code_buf);

    *byte_code_capacity = 2 * count_of_lines;

    int* byte_code_buf = (int*) calloc(*byte_code_capacity, sizeof(char)); //??

    int* label_array = create_label_array();

    fill_label_array(pointers_array, count_of_lines, label_array);

    fill_byte_code_buf (pointers_array, count_of_lines, byte_code_buf, label_array);  // TODO: обработка ошибок

    return byte_code_buf;
}

int* fill_byte_code_buf (char** pointers_array, size_t count_of_lines, int* byte_code_buf, int* label_array) {  // TODO: возврат ошибок
    assert(pointers_array);
    assert(byte_code_buf);

    char command_str [COMMAND_MAX_LEN] = {0}; 
    char argument_str[COMMAND_MAX_LEN] = {0};

    size_t str_num         = 0;
    size_t byte_code_index  = 0;
    int    count_of_arg    = 0;

    while (str_num < count_of_lines && 
           pointers_array[str_num] != NULL) 
    {
        count_of_arg = sscanf( (const char*) pointers_array[str_num], "%32s %32s", command_str, argument_str); // COMMAND_MAX_LEN = 32

        int  command_int = command_identify( (const char*) command_str); // TODO: UNKNOW_COM err analise
        int argument_int = argument_identify(count_of_arg, command_int, (const char*) argument_str, label_array);

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

        byte_code_buf[ byte_code_index++ ] = command_int;
        byte_code_buf[ byte_code_index++ ] = argument_int;

        ++str_num;
    }

    return byte_code_buf;
}

int command_identify (const char* command_str) {
    assert(command_str);
    
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

    } else if (strcmp(command_str, "JMP") == 0) {
        return CMD_JMP;

    } else if (strcmp(command_str, "JB") == 0) {
        return CMD_JB;
        
    } else if (strcmp(command_str, "JBE") == 0) {
        return CMD_JBE;
        
    } else if (strcmp(command_str, "JA") == 0) {
        return CMD_JA;
        
    } else if (strcmp(command_str, "JAE") == 0) {
        return CMD_JAE;
        
    } else if (strcmp(command_str, "JE") == 0) {
        return CMD_JE;
        
    } else if (strcmp(command_str, "JNE") == 0) {
        return CMD_JNE;

    } else 
        return UNKNOWN_COM;
}

int argument_identify (int count_of_arg, int command_int, const char* argument_str, int* label_array) {
    assert(argument_str);

    switch (count_of_arg)
    {
        case 1:  // one argument
            return POISON;
            break;

        case 2: // two arguments

            if (command_int >= 33 &&                                 // CMD_PUSHR = 33, CMD_POPR = 34, CMD_IN = 35
                command_int <= 35)
                return register_num(argument_str);

            if (command_int >= 64 &&                                 // CMD_JMP = 64, CMD_JB  = 65, CMD_JBE = 66 
                command_int <= 70)                                   // CMD_JA  = 67, CMD_JAE = 68, CMD_JE  = 69, CMD_JNE = 70
                return identify_label(argument_str, label_array);

            return atoi(argument_str);

        default:
            break;
    }

    fprintf(stderr, "Incorrect ASM-code");
    return POISON;
}

int register_num (const char* argument_str) {
    assert(argument_str);

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

int* create_label_array (void) {

    int* label_array = (int*) calloc(LABEL_BUF_SIZE, sizeof(int));

    if (label_array == NULL) {
        fprintf(stderr, "create_label_array: Allocation error");
        return label_array;
    }
    
    return label_array;
}

int* fill_label_array (char** pointers_array, size_t count_of_lines, int* label_array) {
    assert(pointers_array);
    assert(label_array);

    int    label    = 0;
    size_t line_num = 0;

    while (line_num < count_of_lines && 
           pointers_array[line_num] != NULL)
    {
        if ( * (pointers_array[line_num] + 1) == ':') 
        {
            label = *pointers_array[line_num] - '0';
            label_array[label] = line_num + 1;
        }

        ++line_num;
    }

    return label_array;
}

int identify_label (const char* argument_str, int* label_array) {
    assert(argument_str);

    int label_number = atoi(argument_str + 1);

    if (label_number < 0 ||
        label_number > 9) 
    {
        fprintf(stderr, "Incorrect label");
        return -1;
    }

    int argument_int = label_array[label_number];

    return argument_int;
}



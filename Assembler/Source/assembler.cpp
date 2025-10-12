#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>

#include "assembler.h"
#include "utils.h"

int* assembler (char** pointers_array, asm_sruct* Assembler) {
    assert(pointers_array);
    assert(Assembler);

    size_t byte_code_capacity = 2 * (Assembler -> count_of_commands); // each command has argument (it can be fictive (POISON))

    SAFE_CALLOC(byte_code_pointer, byte_code_capacity, int)

    SAFE_CALLOC(label_array, LABEL_BUF_SIZE, int)

    size_t count_of_commands_without_labels = fill_byte_code_buf (pointers_array, Assembler, byte_code_pointer, label_array); // 1st compilation
    Assembler -> ind_counter = 0;

    listing_labeles_array(stdout, label_array);
    
    fill_byte_code_buf (pointers_array, Assembler, byte_code_pointer, label_array); // 2nd compilation with lables
    
    free(label_array);

    if (count_of_commands_without_labels == 0) {
        fprintf(stderr, "fill_byte_code_buf: error");
        return byte_code_pointer;
    }

    Assembler -> count_of_commands = count_of_commands_without_labels;

    return byte_code_pointer;
}

size_t fill_byte_code_buf (char** pointers_array, asm_sruct* Assembler, int* byte_code_pointer, int* label_array) {
    assert(pointers_array);

    char command_str [COMMAND_MAX_LEN] = {0}; 
    char argument_str[COMMAND_MAX_LEN] = {0};

    size_t cmd_num         = 0;
    int    count_of_arg    = 0;

    size_t count_of_commands_without_labeles = Assembler -> count_of_commands;

    while (cmd_num < Assembler -> count_of_commands && 
           pointers_array[cmd_num] != NULL) 
    {
        count_of_arg = sscanf( (const char*) pointers_array[cmd_num], "%32s %32s", command_str, argument_str); // COMMAND_MAX_LEN = 32

        if (count_of_arg == 0) {
            fprintf(stderr, "Incorrect ASM-code");
            return 0;

        } else if (count_of_arg == 1) {
            *argument_str = '\0';
        }

        int label_check = fill_label_array(command_str, &count_of_commands_without_labeles, &cmd_num, label_array);
        if (label_check == IS_LABEL)
            continue;

        int  command_int = command_identify( (const char*) command_str); // each command has argument (it can be fictive (POISON))
        int argument_int = argument_identify(count_of_arg, command_int, (const char*) argument_str, label_array);

        byte_code_pointer[ Assembler -> ind_counter ++ ] = command_int;
        byte_code_pointer[ Assembler -> ind_counter ++ ] = argument_int;

        listing_byte_code(stdout,     *Assembler,
                          command_str, argument_str,
                          command_int, argument_int);

        ++cmd_num;
    }

    return count_of_commands_without_labeles;
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
            return POISON;   // each command has argument (it can be fictive (POISON))
            break;

        case 2: // two arguments

            if (command_int >= 33 &&                                  // CMD_PUSHR = 33, CMD_POPR = 34, CMD_IN = 35
                command_int <= 35)
                return register_num(argument_str);

            if (command_int >= 64 &&                                  // CMD_JMP = 64, CMD_JB  = 65, CMD_JBE = 66 
                command_int <= 70)  {                                 // CMD_JA  = 67, CMD_JAE = 68, CMD_JE  = 69, CMD_JNE = 70
                return identify_label(argument_str, label_array);
            }

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

int fill_label_array (char* command_str, size_t* count_of_commands_without_labeles, size_t* cmd_num, int* label_array) {
    assert(command_str);
    assert(count_of_commands_without_labeles);
    assert(cmd_num);
    assert(label_array);

    if (is_label(command_str)) {

        int label = *command_str - '0';

        int byte_code_ind = 2 * (*cmd_num);
        label_array[label] = byte_code_ind;

        --(*count_of_commands_without_labeles);
        ++(*cmd_num);      
              
        return IS_LABEL;
    }

    return NOT_LABEL;
}

int identify_label (const char* argument_str, int* label_array) {
    assert(argument_str);

    int argument_int    = 0;

    if (*argument_str == ':') 
    {
        int label_number = atoi(argument_str + 1);

        if (label_number < 0 ||
            label_number > LABEL_BUF_SIZE - 1) 
        {
            fprintf(stderr, "Incorrect label");
            return NOT_LABEL;
        }

        argument_int = label_array[label_number];
    }

    else
        argument_int = 2 * (atoi(argument_str) - 1);

    return argument_int;
}

int is_label(char* string) {
    assert(string);

    if ( *(string + 1) == ':')
        return 1;

    return 0;
}
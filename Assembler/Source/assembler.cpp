#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>

#include "assembler.h"
#include "utils.h"

int* assembler (char** pointers_array, asm_struct* Assembler) {
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

size_t fill_byte_code_buf (char** pointers_array, asm_struct* Assembler, int* byte_code_pointer, int* label_array) {
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

        if (count_of_arg == 0) {                            // TODO: new func
            fprintf(stderr, "Incorrect ASM-code");
            return 0;

        } else if (count_of_arg == 1) {
            *argument_str = '\0';
        }

        int label_check = fill_label_array(command_str, &count_of_commands_without_labeles, *Assembler, label_array);
        if (label_check == IS_LABEL) {
            ++cmd_num;      
            continue;
        }

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

#define ELSE_IF_CMD(cmd_name)                              \
    else if (strcmp(command_str, #cmd_name) == 0)          \
        return CMD_##cmd_name; 

int command_identify (const char* command_str) {
    assert(command_str);
    
    if (strcmp(command_str, "PUSH") == 0)
        return CMD_PUSH;

    ELSE_IF_CMD(OUT)
    ELSE_IF_CMD(HLT)

    ELSE_IF_CMD(ADD)
    ELSE_IF_CMD(SUB)
    ELSE_IF_CMD(MUL)
    ELSE_IF_CMD(DIV)
    ELSE_IF_CMD(SQRT)

    ELSE_IF_CMD(PUSHR)
    ELSE_IF_CMD(POPR)
    ELSE_IF_CMD(IN)

    ELSE_IF_CMD(JMP)
    ELSE_IF_CMD(JB)
    ELSE_IF_CMD(JBE)
    ELSE_IF_CMD(JA)
    ELSE_IF_CMD(JAE)
    ELSE_IF_CMD(JE)
    ELSE_IF_CMD(JNE)

    ELSE_IF_CMD(CALL)
    ELSE_IF_CMD(RET)

    ELSE_IF_CMD(PUSHM)
    ELSE_IF_CMD(POPM)

    else 
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

            if  (command_int >= CMD_PUSHR &&                                // CMD_PUSHR = 33, CMD_POPR = 34, CMD_IN = 35
                command_int <= CMD_IN)
                return register_num(argument_str);

            else if (command_int >= CMD_JMP &&                              // CMD_JMP = 64, CMD_JB  = 65, CMD_JBE = 66, CMD_JA    = 67
                    command_int <= CMD_CALL)                                // CMD_JAE = 68, CMD_JE  = 69, CMD_JNE = 70, CMD_CALL = 71
                return identify_label(argument_str, label_array);

            else if (command_int >= CMD_PUSHM &&                            // CMD_PUSHM = 73, CMD_POPM = 74
                    command_int <= CMD_POPM)
                return indentify_register_RAM( (char*) argument_str);

            else
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
        return -1;
    }

    return offset_from_first_reg;
}

int fill_label_array (char* command_str, size_t* count_of_commands_without_labeles, asm_struct Assembler, int* label_array) {
    assert(command_str);
    assert(count_of_commands_without_labeles);
    assert(label_array);

    if (is_label(command_str)) {

        int label = *command_str - '0';

        label_array[label] = Assembler.ind_counter;

        --(*count_of_commands_without_labeles);
              
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

int indentify_register_RAM(char* argument_str) {
    assert(argument_str);

    int Register = -1;

    if (argument_str[0] == '[' &&
        argument_str[4] == ']') 
    {
        argument_str[4] = '\0';
        Register = register_num(argument_str + 1);
        argument_str[4] = ']';
    }

    else 
        fprintf(stderr, "Incorrect syntacsis of RAM commans");

    return Register;
}
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>

#include "assembler.h"
#include "utils.h"

int assembler (asm_struct* Assembler) {
    assert(Assembler);

    size_t byte_code_capacity = 2 * (Assembler -> count_of_commands); // each command has argument (it can be fictive (POISON))

    SAFE_CALLOC(temp_byte_code_pointer, byte_code_capacity, int)
    Assembler -> byte_code_buf = temp_byte_code_pointer;

    SAFE_CALLOC(temp_label_array, LABEL_BUF_SIZE, int)
    Assembler -> labeles_array = temp_label_array;

    size_t count_of_commands_without_labels = fill_byte_code_buf (Assembler); // 1st compilation
    Assembler -> ind_counter = 0;

    listing_labeles_array(stdout, Assembler);
    
    fill_byte_code_buf (Assembler); // 2nd compilation with lables
    
    if (count_of_commands_without_labels == 0) {
        fprintf(stderr, "fill_byte_code_buf: error");
        return END_SUCCESS;
    }
    
    Assembler -> count_of_commands = count_of_commands_without_labels;

    return END_SUCCESS;
}


#define ONE_CMD_INFO(cmd_name)                                                                   \
                                                                                                 \
CmdStruct cmd_##cmd_name {};                                                                     \
    cmd_##cmd_name.name     = #cmd_name;                                                         \
    cmd_##cmd_name.code     = CMD_##cmd_name;                                                    \
                                                                                                 \
    const char* str_##cmd_name = #cmd_name;                                                      \
    int hash_##cmd_name = 0;                                                                     \
                                                                                                 \
    for (size_t char_num = 0; str_##cmd_name[char_num] != '\0'; char_num++)                      \
        hash_##cmd_name = hash_##cmd_name * PRIME_COEF_HASH + str_##cmd_name[char_num];          \
                                                                                                 \
    cmd_##cmd_name.hash = hash_##cmd_name;                                                       \
                                                                                                 \
cmd_info_arr[CMD_##cmd_name] = cmd_##cmd_name;


CmdStruct* create_cmd_info_arr(void) {
    
    SAFE_CALLOC(cmd_info_arr, MAX_COUNT_OF_CMD, CmdStruct)

    ONE_CMD_INFO(HLT)
    ONE_CMD_INFO(PUSH)
    ONE_CMD_INFO(OUT)
    ONE_CMD_INFO(ADD)
    ONE_CMD_INFO(SUB)
    ONE_CMD_INFO(MUL)
    ONE_CMD_INFO(DIV)
    ONE_CMD_INFO(SQRT)
    ONE_CMD_INFO(PUSHR)
    ONE_CMD_INFO(POPR)
    ONE_CMD_INFO(IN)
    ONE_CMD_INFO(JMP)
    ONE_CMD_INFO(JB)
    ONE_CMD_INFO(JBE)
    ONE_CMD_INFO(JA)
    ONE_CMD_INFO(JAE)
    ONE_CMD_INFO(JE)
    ONE_CMD_INFO(JNE)
    ONE_CMD_INFO(CALL)
    ONE_CMD_INFO(RET)
    ONE_CMD_INFO(PUSHM)
    ONE_CMD_INFO(POPM)

    return cmd_info_arr;
}

#undef ONE_CMD_INFO

size_t fill_byte_code_buf (asm_struct* Assembler) {
    assert(Assembler);

    char command_str [COMMAND_MAX_LEN] = {0}; 
    char argument_str[COMMAND_MAX_LEN] = {0};

    size_t cmd_num         = 0;
    int    count_of_arg    = 0;

    size_t count_of_commands_without_labeles = Assembler -> count_of_commands;

    while (cmd_num < Assembler -> count_of_commands && 
           Assembler -> pointers_array[cmd_num] != NULL) 
    {
        count_of_arg = sscanf( (const char*) Assembler -> pointers_array[cmd_num], "%31s %31s", command_str, argument_str); // COMMAND_MAX_LEN = 32

        if (count_of_arg == 0) {                            // TODO: new func
            fprintf(stderr, "Incorrect ASM-code");
            return 0;

        } else if (count_of_arg == 1) {
            *argument_str = '\0';
        }

        int label_check = fill_label_array(Assembler, command_str, &count_of_commands_without_labeles);
        if (label_check == IS_LABEL) {
            ++cmd_num;      
            continue;
        }

        int  command_int = command_identify (Assembler, (const char*)  command_str); // each command has argument (it can be fictive (POISON))
        int argument_int = argument_identify(Assembler,  count_of_arg, command_int, (const char*) argument_str);

        Assembler -> byte_code_buf[ Assembler -> ind_counter ++ ] = command_int;
        Assembler -> byte_code_buf[ Assembler -> ind_counter ++ ] = argument_int;

        listing_byte_code(stdout,     *Assembler,
                          command_str, argument_str,
                          command_int, argument_int);

        ++cmd_num;
    }

    return count_of_commands_without_labeles;
}


int command_identify (asm_struct* Assembler, const char* command_str) {
    assert(command_str);
    assert(Assembler);

    int finding_hash = 0;                                                         

    for (size_t char_num = 0; command_str[char_num] != '\0'; char_num++)
        finding_hash = finding_hash * PRIME_COEF_HASH + command_str[char_num]; 

    for (size_t cmd_info_el = 0; cmd_info_el < MAX_COUNT_OF_CMD; cmd_info_el++) {

        if (Assembler -> cmd_info_arr[cmd_info_el].hash == finding_hash)
            return Assembler -> cmd_info_arr[cmd_info_el].code;

        else
            continue;
    }

    return UNKNOWN_COM;
}

int argument_identify (asm_struct* Assembler, int count_of_arg, int command_int, const char* argument_str) {
    assert(argument_str);

    switch (count_of_arg)
    {
        case 1:  // one argument
            return POISON;   // each command has argument (it can be fictive (POISON))
            break;

        case 2: // two arguments

            if  (command_int >= CMD_PUSHR &&                                 // CMD_PUSHR = 33, CMD_POPR = 34, CMD_IN = 35
                 command_int <= CMD_IN)
                return register_num(argument_str);

            else if (command_int >= CMD_JMP &&                               // CMD_JMP = 64, CMD_JB  = 65, CMD_JBE = 66, CMD_JA   = 67
                     command_int <= CMD_CALL)                                // CMD_JAE = 68, CMD_JE  = 69, CMD_JNE = 70, CMD_CALL = 71
                return identify_label(Assembler, argument_str);

            else if (command_int >= CMD_PUSHM &&                             // CMD_PUSHM = 73, CMD_POPM = 74
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

int fill_label_array (asm_struct* Assembler, char* command_str, size_t* count_of_commands_without_labeles) {
    assert(command_str);
    assert(count_of_commands_without_labeles);
    assert(Assembler);

    if (is_label(command_str)) {

        int label = *command_str - '0';

        Assembler -> labeles_array[label] = Assembler -> ind_counter;

        --(*count_of_commands_without_labeles);
              
        return IS_LABEL;
    }

    return NOT_LABEL;
}

int identify_label (asm_struct* Assembler, const char* argument_str) {
    assert(argument_str);
    assert(Assembler);

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

        argument_int = Assembler -> labeles_array[label_number];
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

    static int len = strlen ("[AX]");

    if (argument_str[0] == '[' &&
        argument_str[len] == ']') 
    {
        argument_str[len] = '\0';
        Register = register_num(argument_str + 1);
        argument_str[len] = ']';
    }

    else 
        fprintf(stderr, "Incorrect syntacsis of RAM commans");

    return Register;
}
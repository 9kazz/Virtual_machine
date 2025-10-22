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

    size_t byte_code_capacity = 2 * (Assembler -> count_of_commands); // each command has argument (it can be fake (POISON))

    SAFE_CALLOC(temp_byte_code_pointer, byte_code_capacity, int)
    Assembler -> byte_code_buf = temp_byte_code_pointer;

    SAFE_CALLOC(temp_label_array, LABEL_BUF_SIZE, LabelStruct)
    Assembler -> labels_array = temp_label_array;

    size_t count_of_commands_without_labels = fill_byte_code_buf (Assembler); // 1st compilation
    Assembler -> ind_counter = 0;

    listing_labels_array(stdout, Assembler);
    
    fill_byte_code_buf (Assembler); // 2nd compilation with labels
    
    if (count_of_commands_without_labels == 0) {
        fprintf(stderr, "fill_byte_code_buf: error");
        return END_SUCCESS;
    }
    
    Assembler -> count_of_commands = count_of_commands_without_labels;

    return END_SUCCESS;
}


#define ONE_CMD_INFO(cmd_name, count_of_args)                                                                       \
                                                                                                                    \
CmdStruct cmd_##cmd_name {};                                                                                        \
    cmd_##cmd_name.name     = #cmd_name;                                                                            \
    cmd_##cmd_name.code     = CMD_##cmd_name;                                                                       \
                                                                                                                    \
    const char* str_##cmd_name = #cmd_name;                                                                         \
    int hash_##cmd_name = 0;                                                                                        \
                                                                                                                    \
    hash_##cmd_name = hash_function(str_##cmd_name);                                                                \
                                                                                                                    \
    cmd_##cmd_name.hash = hash_##cmd_name;                                                                          \
                                                                                                                    \
    cmd_##cmd_name.arg_count = count_of_args;                                                                       \
                                                                                                                    \
    cmd_info_arr[CMD_##cmd_name] = cmd_##cmd_name;


CmdStruct* create_cmd_info_arr(void) {
    
    SAFE_CALLOC(cmd_info_arr, MAX_COUNT_OF_CMD, CmdStruct)

    ONE_CMD_INFO(HLT,   NO_ARGS)
    ONE_CMD_INFO(PUSH,  ONE_ARG)
    ONE_CMD_INFO(OUT,   NO_ARGS)
    ONE_CMD_INFO(ADD,   NO_ARGS)
    ONE_CMD_INFO(SUB,   NO_ARGS)
    ONE_CMD_INFO(MUL,   NO_ARGS)
    ONE_CMD_INFO(DIV,   NO_ARGS)
    ONE_CMD_INFO(SQRT,  NO_ARGS)
    ONE_CMD_INFO(PUSHR, ONE_ARG)
    ONE_CMD_INFO(POPR,  ONE_ARG)
    ONE_CMD_INFO(IN,    NO_ARGS)
    ONE_CMD_INFO(JMP,   ONE_ARG)
    ONE_CMD_INFO(JB,    ONE_ARG)
    ONE_CMD_INFO(JBE,   ONE_ARG)
    ONE_CMD_INFO(JA,    ONE_ARG)
    ONE_CMD_INFO(JAE,   ONE_ARG)
    ONE_CMD_INFO(JE,    ONE_ARG)
    ONE_CMD_INFO(JNE,   ONE_ARG)
    ONE_CMD_INFO(CALL,  ONE_ARG)
    ONE_CMD_INFO(RET,   NO_ARGS)
    ONE_CMD_INFO(PUSHM, ONE_ARG)
    ONE_CMD_INFO(POPM,  ONE_ARG)

    return cmd_info_arr;
}

#undef ONE_CMD_INFO

size_t fill_byte_code_buf (asm_struct* Assembler) {
    assert(Assembler);

    char command_str [COMMAND_MAX_LEN] = {0}; 
    char argument_str[COMMAND_MAX_LEN] = {0};

    size_t cmd_num         = 0;
    int    sscanf_check    = 0;

    size_t count_of_commands_without_labels = Assembler -> count_of_commands;

    while (cmd_num < Assembler -> count_of_commands && 
           Assembler -> pointers_array[cmd_num] != NULL) 
    {
        sscanf_check = sscanf( (const char*) Assembler -> pointers_array[cmd_num], "%31s %31s", command_str, argument_str); // COMMAND_MAX_LEN = 32

        if (sscanf_check == 0) {                           
            fprintf(stderr, "Incorrect ASM-code");
            return 0;
        }

        int label_check = fill_label_array(Assembler, command_str, &count_of_commands_without_labels);
        if (label_check == IS_LABEL) {
            ++cmd_num;      
            continue;
        }

        int  command_int = command_identify (Assembler, (const char*)  command_str); // each command has argument (it can be fictive (POISON))
        int argument_int = argument_identify(Assembler, command_int, argument_str);

        Assembler -> byte_code_buf[ Assembler -> ind_counter ++ ] = command_int;
        Assembler -> byte_code_buf[ Assembler -> ind_counter ++ ] = argument_int;

        listing_byte_code(stdout,     *Assembler,
                          command_str, argument_str,
                          command_int, argument_int);

        ++cmd_num;
    }

    return count_of_commands_without_labels;
}


int command_identify (asm_struct* Assembler, const char* command_str) {
    assert(command_str);
    assert(Assembler);

    int finding_hash = 0;                                                         

    finding_hash = hash_function(command_str);

    for (size_t cmd_info_el = 0; cmd_info_el < MAX_COUNT_OF_CMD; cmd_info_el++) {

        if (Assembler -> cmd_info_arr[cmd_info_el].hash == finding_hash)
            return Assembler -> cmd_info_arr[cmd_info_el].code;

        else
            continue;
    }

    return UNKNOWN_COM;
}

int argument_identify (asm_struct* Assembler, int command_int, char* argument_str) {
    assert(argument_str);

    switch (Assembler -> cmd_info_arr[command_int].arg_count)
    {
        case FAKE_ARG:
            *argument_str =  '\0';
            return POISON;   // each command has argument (it can be fictive (POISON))
            break;

        case TRUE_ARG:

            if  (command_int >= CMD_PUSHR &&                                 // CMD_PUSHR = 33, CMD_POPR = 34
                 command_int <= CMD_POPR)
                return register_num(argument_str);

            else if (command_int >= CMD_JMP &&                               // CMD_JMP = 64, CMD_JB  = 65, CMD_JBE = 66, CMD_JA   = 67
                     command_int <= CMD_CALL)                                // CMD_JAE = 68, CMD_JE  = 69, CMD_JNE = 70, CMD_CALL = 71
                return identify_label(Assembler, argument_str);

            else if (command_int >= CMD_PUSHM &&                             // CMD_PUSHM = 73, CMD_POPM = 74
                     command_int <= CMD_POPM)
                return identify_register_RAM( (char*) argument_str);

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

int fill_label_array (asm_struct* Assembler, char* command_str, size_t* count_of_commands_without_labels) {
    assert(command_str);
    assert(count_of_commands_without_labels);
    assert(Assembler);

    if (is_label(command_str) == IS_LABEL) {

        int hash = 0;
        
        size_t offset_to_last_el = strlen(command_str) - 1;

        *(command_str + offset_to_last_el) = '\0';

        if (Assembler -> label_ind_counter >= LABEL_BUF_SIZE) {
            printf("fill_label_array: labels buffer is overflow\n");
            return NOT_LABEL;
        }

        Assembler -> labels_array[Assembler->label_ind_counter].name = strdup(command_str);
    
        hash = hash_function(Assembler->labels_array[Assembler->label_ind_counter].name);
        
        Assembler -> labels_array[Assembler->label_ind_counter].hash  = hash;

        Assembler -> labels_array[Assembler->label_ind_counter].index = Assembler -> ind_counter;

        (Assembler -> label_ind_counter)++ ;
        --(*count_of_commands_without_labels);
              
        return IS_LABEL;
    }

    return NOT_LABEL;
}

int identify_label (asm_struct* Assembler, char* argument_str) {
    assert(argument_str);
    assert(Assembler);

    int argument_int = 0;
    int finding_hash = 0;

    if (*argument_str == ':') 
    {
        ++argument_str;

        for (size_t char_num = 0; argument_str[char_num] != '\0'; char_num++)
            finding_hash = (finding_hash * PRIME_COEF_HASH + argument_str[char_num]) % MAX_INT_VALUE;  

        for (size_t label_arr_el = 0; label_arr_el < LABEL_BUF_SIZE; label_arr_el++) {

            if (Assembler -> labels_array[label_arr_el].hash == finding_hash)
                argument_int = Assembler -> labels_array[label_arr_el].index;

            else
                continue;
        }
    }

    else
        argument_int = 2 * (atoi(argument_str) - 1);

    return argument_int;
}

int is_label(char* string) {
    assert(string);

    size_t offset_to_last_el = strlen(string) - 1;

    if ( *(string + offset_to_last_el) == ':')
        return IS_LABEL;

    return NOT_LABEL;
}

int identify_register_RAM(char* argument_str) {
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
        fprintf(stderr, "Incorrect syntax of RAM commans");

    return Register;
}
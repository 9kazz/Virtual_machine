#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>
#include <unistd.h>

#include "utils.h"
#include "processor.h"
#include "LogFile.h"
#include "type_struct_def.h"    
#include "stack_utils.h"
#include "stack.h"

#define ONE_CMD_INFO(cmd_name, function)                    \
                                                            \
CmdStruct cmd_##cmd_name {};                                \
    cmd_##cmd_name.name     = #cmd_name;                    \
    cmd_##cmd_name.code     = CMD_##cmd_name;               \
    cmd_##cmd_name.cmd_func = &function;                    \
                                                            \
cmd_info_arr[CMD_##cmd_name] = cmd_##cmd_name;

CmdStruct* create_cmd_info_arr(void) {
    
    SAFE_CALLOC(cmd_info_arr, MAX_COUNT_OF_CMD, CmdStruct)

    ONE_CMD_INFO(PUSH,  Stack_Push_Proc)
    ONE_CMD_INFO(OUT,   Stack_Pop_Proc)
    ONE_CMD_INFO(ADD,   Stack_Arif_Add)
    ONE_CMD_INFO(SUB,   Stack_Arif_Sub)
    ONE_CMD_INFO(MUL,   Stack_Arif_Mul)
    ONE_CMD_INFO(DIV,   Stack_Arif_Div)
    ONE_CMD_INFO(SQRT,  Stack_Arif_Sqrt)
    ONE_CMD_INFO(PUSHR, Stack_PushR)
    ONE_CMD_INFO(POPR,  Stack_PopR)
    ONE_CMD_INFO(IN,    Stack_In)
    ONE_CMD_INFO(JMP,   Jump_to_JMP)
    ONE_CMD_INFO(JB,    Jump_Below_JB)
    ONE_CMD_INFO(JBE,   Jump_Below_Equal_JBE)
    ONE_CMD_INFO(JA,    Jump_Above_JA)
    ONE_CMD_INFO(JAE,   Jump_Above_Equal_JAE)
    ONE_CMD_INFO(JE,    Jump_Equal_JE)
    ONE_CMD_INFO(JNE,   Jump_Not_Equal_JNE)
    ONE_CMD_INFO(CALL,  Call_command)
    ONE_CMD_INFO(RET,   Return_to_call_RET)
    ONE_CMD_INFO(PUSHM, Push_from_RAM_PUSHM)
    ONE_CMD_INFO(POPM,  Pop_to_RAM_POPM)

    return cmd_info_arr;
}

#undef ONE_CMD_INFO

int* create_bite_code_buf (FILE* input_file, size_t* bite_code_size) {
    assert(input_file);
    assert(bite_code_size);

    size_t count_of_cmd = 0;
    int    signature    = 0;
    int    version      = 0;

    fscanf(input_file, "%d %d %d", &count_of_cmd, &version, &signature);

    *bite_code_size = size_of_file(input_file);

    SAFE_CALLOC(bite_code_buf, *bite_code_size, int)

    int command  = 0;
    int argument = 0;

    size_t el_num = 0;

    while (el_num  < *bite_code_size) {

        int end_of_file_checking = fscanf(input_file, "%d %d", &command, &argument);

        if (end_of_file_checking == EOF)
            break;

        bite_code_buf[ el_num++ ] = command;
        bite_code_buf[ el_num++ ] = argument;
    }

    return bite_code_buf;
}

size_t size_of_file (FILE* file_input) {
    struct stat file_info;

    int descriptor = fileno(file_input);

    int check = fstat(descriptor, &file_info);

    assert(check != -1);
    
        if (check == -1) {
            fprintf(stderr, "size_of_file: error");
            return 0;
        }
    
    return file_info.st_size;         
}

CalcErr_t RAM_Draw_in_file (FILE* draw_file, ProcStruct* Proc_struct) {
    assert(draw_file);
    assert(Proc_struct);
    
    for (size_t el_num = 1; el_num < CAPASITY_OF_RAM; el_num++) {
        fprintf(draw_file, "%c ", Proc_struct -> RAM_buf[el_num - 1]);
        if (el_num % RAM_SIZE_X == 0)
            fprintf(draw_file, "\n");
    }

    return SUCCESS;
}

void command_line_flags(int argc, char* argv[], FILE** input_file) {

    int opt = 0;

    while ( (opt = getopt(argc, argv, "i:")) != -1) 
    {
        switch (opt) 
        {
            case 'i':
                *input_file = fopen(optarg, "r");
                break;

            case '?':
                fprintf(stderr, "command_line_flags: unknown flag (%c)\n", *optarg);
                break;

            default:
                fprintf(stderr, "command_line_flags: unexpectid error\n");
        }
    }
}
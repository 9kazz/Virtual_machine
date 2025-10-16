#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>

#include "utils.h"
#include "processor.h"
#include "LogFile.h"
#include "type_struct_def.h"    
#include "stack_utils.h"
#include "stack.h"

#define ONE_CMD_INFO(ind_in_arr, cmd_name, function)        \
                                                            \
CmdStruct cmd_##ind_in_arr {};                              \
    cmd_##ind_in_arr.name     = #cmd_name;                  \
    cmd_##ind_in_arr.code     = CMD_##cmd_name;             \
    cmd_##ind_in_arr.cmd_func = &function;                  \
                                                            \
cmd_info_arr[ind_in_arr] = cmd_##ind_in_arr;

CmdStruct* create_cmd_info_arr(void) {
    
    SAFE_CALLOC(cmd_info_arr, MAX_COUNT_OF_CMD, CmdStruct)

    ONE_CMD_INFO(0,  PUSH,  Stack_Push_Proc)
    ONE_CMD_INFO(1,  OUT,   Stack_Pop_Proc)
    ONE_CMD_INFO(2,  ADD,   Stack_Arif_Add)
    ONE_CMD_INFO(3,  SUB,   Stack_Arif_Sub)
    ONE_CMD_INFO(4,  MUL,   Stack_Arif_Mul)
    ONE_CMD_INFO(5,  DIV,   Stack_Arif_Div)
    ONE_CMD_INFO(6,  SQRT,  Stack_Arif_Sqrt)
    ONE_CMD_INFO(7,  PUSHR, Stack_PushR)
    ONE_CMD_INFO(8,  POPR,  Stack_PopR)
    ONE_CMD_INFO(9,  IN,    Stack_In)
    ONE_CMD_INFO(10, JMP,   Jump_to_JMP)
    ONE_CMD_INFO(11, JB,    Jump_Below_JB)
    ONE_CMD_INFO(12, JBE,   Jump_Below_Equal_JBE)
    ONE_CMD_INFO(13, JA,    Jump_Above_JA)
    ONE_CMD_INFO(14, JAE,   Jump_Above_Equal_JAE)
    ONE_CMD_INFO(15, JE,    Jump_Equal_JE)
    ONE_CMD_INFO(16, JNE,   Jump_Not_Equal_JNE)
    ONE_CMD_INFO(17, CALL,  Call_command)
    ONE_CMD_INFO(18, RET,   Return_to_call_RET)
    ONE_CMD_INFO(19, PUSHM, Push_from_RAM_PUSHM)
    ONE_CMD_INFO(20, POPM,  Pop_to_RAM_POPM)

    return cmd_info_arr;
}

CmdStruct* find_cmd_in_arr(ProcStruct* Proc_struct, int code_of_cmd) {
    assert(Proc_struct);

    int el_num = 0;

    while (el_num < MAX_COUNT_OF_CMD &&
           Proc_struct -> cmd_info_arr[el_num].code != 0)
    {
        if (Proc_struct -> cmd_info_arr[el_num].code == code_of_cmd)
            return &(Proc_struct -> cmd_info_arr[el_num]);

        ++el_num;
    }

    fprintf(stderr, "find_cmd_in_arr: can`t find this command (%d) in cmd_info_array\n", code_of_cmd);
    return NULL;
}

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
    
    for (size_t el_num = 1; el_num <= CAPASITY_OF_RAM; el_num++) {
        fprintf(draw_file, "%c ", Proc_struct -> RAM_buf[el_num - 1]);
        if (el_num % RAM_SIZE_X == 0)
            fprintf(draw_file, "\n");
    }

    return SUCCESS;
}
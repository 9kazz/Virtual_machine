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


FILE* LogFile = fopen("errors.log", "w");

int main(int argc, char* argv[]) {

    FILE* input_file = NULL;
    // FILE* draw_file  = fopen("../draw.txt", "w");

    command_line_flags(argc, argv, &input_file);

    assert(LogFile);
    assert(input_file);
    // assert(draw_file);

    CALC_CTOR(Proc_struct)

    // printf("cmd_info_arr:\n");
    // for (size_t i = 0; i < MAX_COUNT_OF_CMD; i++)
    //     printf("%s \t %d \n", Proc_struct.cmd_info_arr[i].name, Proc_struct.cmd_info_arr[i].code);

    processor(&Proc_struct);

    printf("calc_stack:\n");
    for (size_t i = 0; i < Proc_struct.calc_stack.cur_position; i++)
        printf("%d\n", Proc_struct.calc_stack.data[i]);
    
    Proc_Dump(LogFile, &Proc_struct);

    RAM_Draw_in_file(stdout, &Proc_struct);
    // printf("register_buf:\n");
    // for (size_t i = 0; i < COUNT_OF_REG; i++)
    //     printf("%d\n", Proc_struct.register_buf[i]);

    Proc_Dtor(&Proc_struct);

    fclose(input_file);
    // fclose(draw_file);
    fclose(LogFile);

    fprintf(stdout, "END WITH SUCCESS");

    return 0;
}
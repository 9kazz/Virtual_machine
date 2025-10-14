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

int main() {
    FILE* input_file  = fopen("input_file.txt",  "r");
    FILE* output_file = fopen("output_file.txt", "w");
    assert(LogFile);
    assert(input_file);
    assert(output_file);

    CALC_CTOR(calc_struct)

    // printf("cmd_info_arr:\n");
    // for (size_t i = 0; i < MAX_COUNT_OF_CMD; i++)
    //     printf("%s \t %d \n", calc_struct.cmd_info_arr[i].name, calc_struct.cmd_info_arr[i].code);

    processor(&calc_struct);

    printf("calc_stack:\n");
    for (size_t i = 0; i < calc_struct.calc_stack.cur_position; i++)
        printf("%d\n", calc_struct.calc_stack.data[i]);
    
    // printf("register_buf:\n");
    // for (size_t i = 0; i < COUNT_OF_REG; i++)
    //     printf("%d\n", calc_struct.register_buf[i]);

    Proc_Dtor(&calc_struct);

    fclose(input_file);
    fclose(output_file);
    fclose(LogFile);

    fprintf(stdout, "END WITH SUCCESS");

    return 0;
}
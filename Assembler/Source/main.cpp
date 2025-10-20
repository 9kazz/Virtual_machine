#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>

#include "utils.h"
#include "assembler.h"

int main(int argc, char* argv[]) {

    FILE* input_file  = NULL;
    FILE* output_file = NULL;
    
    command_line_flags(argc, argv, &input_file, &output_file);

    assert(input_file);
    assert(output_file);

    ASM_STRUCT_INIT(Assembler)

    // for (int i = 0; i < MAX_COUNT_OF_CMD; i++)
    //     printf("%s: %d\n", Assembler.cmd_info_arr[i].name, Assembler.cmd_info_arr[i].hash);

    create_pointers_array(input_file, &Assembler);

    assembler (&Assembler);
    
    fprint_byte_code(output_file, Assembler);

    // size_t el_num = 0;
    // while (el_num < 10) 
    // {
    //     fprintf(stdout, "%d\t", label_array[el_num]);
    // }

    fclose(input_file);
    fclose(output_file);

    Assembler_struct_Dtor(&Assembler);

    fprintf(stdout, "END WITH SUCCESS");

    return 0;
}
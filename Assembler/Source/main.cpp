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

    asm_struct Assembler;
        Assembler.count_of_commands = 0;
        Assembler.byte_code_buf     = NULL;
        Assembler.ind_counter       = 0;

    char** pointers_array = create_pointers_array(input_file, &(Assembler.count_of_commands));

    Assembler.byte_code_buf = assembler (pointers_array, &Assembler);
    
    fprint_byte_code(output_file, Assembler);

    // size_t el_num = 0;
    // while (el_num < 10) 
    // {
    //     fprintf(stdout, "%d\t", label_array[el_num]);
    // }

    free(pointers_array);

    fclose(input_file);
    fclose(output_file);

    fprintf(stdout, "END WITH SUCCESS");

    return 0;
}
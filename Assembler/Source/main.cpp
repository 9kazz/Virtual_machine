#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>

#include "utils.h"
#include "assembler.h"

int main() {
    FILE* input_file  = fopen("input_file.txt",  "r");
    FILE* output_file = fopen("output_file.txt", "w");
    assert(input_file);
    assert(output_file);

    size_t count_of_lines = 0;
    char** pointers_array = create_pointers_array(input_file, &count_of_lines);

    size_t byte_code_capacity = 0;
    int* byte_code_buf = assembler (pointers_array, count_of_lines, &byte_code_capacity);
    
    fprint_byte_code(output_file, byte_code_buf, byte_code_capacity);

    size_t el_num = 0;
    while (el_num < byte_code_capacity) 
    {
        fprintf(stdout, "%d\t", byte_code_buf[ el_num++ ]);
        fprintf(stdout, "%d\n", byte_code_buf[ el_num++ ]);
    }

    fclose(input_file);
    fclose(output_file);

    fprintf(stdout, "END WITH SUCCESS");

    return 0;
}
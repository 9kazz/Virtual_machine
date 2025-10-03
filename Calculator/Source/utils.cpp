#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>

#include "utils.h"
#include "calculator.h"

int* create_bit_code_buf (FILE* input_file, size_t* bit_code_size) {

    size_t count_of_lines = 0;
    fscanf(input_file, "%d", &count_of_lines); // TODO: обработка ошибок ...=scanf
    
    *bit_code_size = 2 * count_of_lines;

    int* bit_code_buf = (int*) calloc(*bit_code_size, sizeof(int));

    int command  = 0;
    int argument = 0;

    for (size_t line_num = 0, el_num = 0; line_num < count_of_lines; line_num++) {

        fscanf(input_file, "%d %d", &command, &argument);

        bit_code_buf[ el_num++ ] = command;
        bit_code_buf[ el_num++ ] = argument;
    }

    return bit_code_buf;
}
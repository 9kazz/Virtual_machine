#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>


char** create_pointers_array (FILE* file_input,       size_t* count_of_lines);

size_t size_of_file          (FILE* file_input);

char*  fill_buffer           (char* asm_code_buf,    FILE* file_input,         size_t fsize);

size_t str_change_char       (char*  asm_code_buf,   char  original_char_code, char   new_char_code);

char** fill_point_ar         (char** pointers_array, char* asm_code_buf,       size_t count_of_lines);

void   fprint_bite_code      (FILE*  output_file,    int*  bit_code_buf,       size_t bit_code_capacity, size_t count_of_lines);

#endif
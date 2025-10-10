#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>

struct asm_sruct {
    int*   byte_code_buf;
    size_t count_of_commands;
};

char** create_pointers_array (FILE* file_input,       size_t* count_of_lines);

size_t size_of_file          (FILE* file_input);

char*  fill_buffer           (char* asm_code_buf,    FILE* file_input,         size_t fsize);

size_t str_change_char       (char*  asm_code_buf,   char  original_char_code, char   new_char_code);

char** fill_point_ar         (char** pointers_array, char* asm_code_buf,       size_t count_of_lines);

void   fprint_byte_code      (FILE*  output_file,    asm_sruct Assembler);

int    my_strcmp             (const char* str1,      const char* str2);


#define SAFE_CALLOC(name, size_of_buf, el_type)                                     \
    el_type* temp_##name = (el_type*) calloc(size_of_buf, sizeof(el_type));         \
                                                                                    \
    if (temp_##name == NULL)                                                        \
        fprintf(stderr, "Allocation error of" #name);                               \
                                                                                    \
    el_type* name = temp_##name;


#endif
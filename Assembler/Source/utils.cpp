#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>

#include "utils.h"
#include "assembler.h"

char** create_pointers_array (FILE* file_input, size_t* count_of_lines) {

    size_t fsize = size_of_file(file_input);

    char* asm_code_buf = (char*) calloc(fsize, sizeof(char));
 
    fill_buffer(asm_code_buf, file_input, fsize);

    *count_of_lines = str_change_char(asm_code_buf, '\n', '\0') + 1;

    char** pointers_array = (char**) calloc(*count_of_lines + 1, sizeof(asm_code_buf));

    pointers_array = fill_point_ar(pointers_array, asm_code_buf, *count_of_lines);
    
    return pointers_array;
}

size_t size_of_file (FILE* file_input) {
    struct stat file_info;

    int descriptor = fileno(file_input);

    int check = fstat(descriptor, &file_info);

    assert(check != -1);
    
        if (check == -1)
            return 0;
    
    return file_info.st_size;         
}

char* fill_buffer (char* asm_code_buf, FILE* file_input, size_t fsize) {
    assert(asm_code_buf);
    assert(file_input);

    if ( ! asm_code_buf)
        return NULL;

    size_t new_size_buffer = fread (asm_code_buf, 1, fsize, file_input) + 1;

    asm_code_buf = (char*) realloc(asm_code_buf, new_size_buffer);
    asm_code_buf [new_size_buffer - 1] = '\0';

    return asm_code_buf; 
}

size_t str_change_char (char* asm_code_buf, char original_char_code, char new_char_code) {

    size_t count_of_changes = 0;
    char* cur_char = asm_code_buf;

    while (cur_char != NULL) 
    {
        cur_char = strchr((const char*) cur_char + 1, original_char_code);
        if (cur_char) 
        {
            *cur_char = new_char_code;
            ++count_of_changes;
        }
    }

    return count_of_changes;
}

char** fill_point_ar (char** pointers_array, char* asm_code_buf, size_t count_of_lines) {
    
    char* end_of_str = asm_code_buf;
    
    size_t i = 0;

    while (i < count_of_lines && 
           end_of_str != NULL) 
    {
        *(pointers_array + (i++)) = end_of_str; 
        end_of_str = strchr(end_of_str, '\0') + 1;
    }

    *(pointers_array + i) = NULL; 

    return pointers_array;
}


void fprint_byte_code (FILE* output_file, asm_struct Assembler) {
    
    size_t byte_code_size = 2 * Assembler.count_of_commands; // each command has argument (it can be fictive (POISON))

    size_t el_num = 0;

    fprintf(output_file, "%d 2 110407 \n", Assembler.count_of_commands);

    while (el_num < byte_code_size) 
    {
        fprintf(output_file, "%d\t", Assembler.byte_code_buf[ el_num++ ]);
        fprintf(output_file, "%d\n", Assembler.byte_code_buf[ el_num++ ]);
    }
}

void listing_byte_code (FILE* listing_file, asm_struct Assembler,
                        char* command_str,  char*  argument_str,
                        int   command_int,  int    argument_int) {

    fprintf(listing_file, "[%3d] [%3d] \t",   Assembler.ind_counter - 2, Assembler.ind_counter - 1);
    fprintf(listing_file, "%8s %8s \t",       command_str,                  argument_str);

    if (argument_int == POISON)
        fprintf(listing_file, "%8d %8s\n", command_int, "POISON"); // each command has argument (it can be fictive (POISON))

    else                    
        fprintf(listing_file, "%8d %8d\n", command_int,  argument_int);
}

void listing_labeles_array (FILE* listing_file, int* label_array) {

    fprintf(listing_file, "\n-------------------------------------------------------------------------------\n");

    for (int el_num = 0; el_num < LABEL_BUF_SIZE; el_num++)
        fprintf(listing_file, "[%d]\t", el_num);

    fprintf(listing_file, "\n\n");

    for (int el_num = 0; el_num < LABEL_BUF_SIZE; el_num++)
        fprintf(listing_file, "[%d]\t", label_array[el_num]);

    fprintf(listing_file, "\n-------------------------------------------------------------------------------\n\n");
}   

int my_strcmp(const char* str1, const char* str2) {

    while (*str1 != '\0' &&
           *str1 == *str2) 
    {
        ++str1;
        ++str2;
    }

    return *str1 - *str2;
}
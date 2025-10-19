#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>

struct asm_struct;
struct CmdStruct;

struct CmdStruct {
    const char* name;
    int         code;
    int         hash;
};

struct asm_struct {
    char*  asm_code_buf;
    char** pointers_array;
    int*   byte_code_buf;
    int*   labeles_array;
    size_t count_of_commands;
    size_t ind_counter;
    CmdStruct* cmd_info_arr;
};

int    create_pointers_array (FILE* file_input,     asm_struct* Assembler);

size_t size_of_file          (FILE* file_input);

char*  fill_buffer           (asm_struct* Assembler,    FILE* file_input,         size_t fsize);

size_t str_change_char       (asm_struct* Assembler, char  original_char_code, char   new_char_code);

int    fill_point_ar         (asm_struct* Assembler);

void   fprint_byte_code      (FILE*  output_file,    asm_struct Assembler);

void   listing_byte_code     (FILE* listing_file, asm_struct Assembler,
                              char* command_str,  char*     argument_str,
                              int   command_int,  int       argument_int);

void  listing_labeles_array  (FILE* listing_file, asm_struct* Assembler);        

int   my_strcmp              (const char* str1,      const char* str2);

void  command_line_flags     (int argc, char* argv[], FILE** input_file, FILE** output_file);

int   Assembler_struct_Dtor  (asm_struct* Assembler);


#define SAFE_CALLOC(name, size_of_buf, el_type)                                     \
    el_type* temp_##name = (el_type*) calloc(size_of_buf, sizeof(el_type));         \
                                                                                    \
    if (temp_##name == NULL)                                                        \
        fprintf(stderr, "Allocation error of" #name);                               \
                                                                                    \
    el_type* name = temp_##name;


#endif
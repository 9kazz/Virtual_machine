#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>
#include <unistd.h>

#include "utils.h"
#include "assembler.h"

int create_pointers_array (FILE* file_input, asm_struct* Assembler) {

    size_t fsize = size_of_file(file_input);

    Assembler -> asm_code_buf = (char*) calloc(fsize, sizeof(char));
 
    fill_buffer(Assembler, file_input, fsize);

    Assembler -> count_of_commands = str_change_char(Assembler, '\n', '\0') + 1;

    Assembler -> pointers_array = (char**) calloc(Assembler -> count_of_commands + 1, sizeof(Assembler -> asm_code_buf));

    fill_point_ar(Assembler);

    // for (size_t i = 0; i < Assembler -> count_of_commands; i++)
    //     printf("%d: %s\n", i, Assembler -> pointers_array[i]);
    
    return END_SUCCESS;
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

char* fill_buffer (asm_struct* Assembler, FILE* file_input, size_t fsize) {
    assert(file_input);

    size_t new_size_buffer = fread (Assembler -> asm_code_buf, 1, fsize, file_input) + 2; // 2 last elements are '\0' and EOF. It`s necessary to detect end of buffer 

    Assembler -> asm_code_buf = (char*) realloc(Assembler -> asm_code_buf, new_size_buffer);
    Assembler -> asm_code_buf [new_size_buffer - 2] = '\0';
    Assembler -> asm_code_buf [new_size_buffer - 1] = EOF;

    for (size_t i = 0; i < new_size_buffer; i++)
        printf("%d ", Assembler->asm_code_buf[i]);

    return Assembler -> asm_code_buf; 
}

size_t str_change_char (asm_struct* Assembler, char original_char_code, char new_char_code) {

    size_t count_of_changes = 0;
    char* cur_char = Assembler -> asm_code_buf;

    while (cur_char != NULL) 
    {
        cur_char = strchr(cur_char + 1, original_char_code);
        if (cur_char) 
        {
            *cur_char = new_char_code;
            ++count_of_changes;
        }
    }

    return count_of_changes;
}

int fill_point_ar (asm_struct* Assembler) {
    
    char* start_of_str = Assembler -> asm_code_buf;
    size_t i = 0;

    while (i < Assembler -> count_of_commands && 
           start_of_str != NULL) 
    {

        *(Assembler -> pointers_array + i) = start_of_str; 
        
        if (*start_of_str == EOF)
            break;

        if (*start_of_str == '\r') {
            start_of_str = strchr(start_of_str, '\0') + 1;
            (Assembler -> count_of_commands)-- ;
            continue;
        }
        
        ++i;

        start_of_str = strchr(start_of_str, '\0') + 1;
    }

    *(Assembler -> pointers_array + i) = NULL; 

    return END_SUCCESS;
}


void fprint_byte_code (FILE* output_file, asm_struct Assembler) {
    
    size_t byte_code_size = 2 * Assembler.count_of_commands; // each command has argument (it can be fake (POISON))

    size_t el_num = 0;

    fprintf(output_file, "%ld 2 110407 \n", Assembler.count_of_commands);

    while (el_num < byte_code_size) 
    {
        fprintf(output_file, "%d\t", Assembler.byte_code_buf[ el_num++ ]);
        fprintf(output_file, "%d\n", Assembler.byte_code_buf[ el_num++ ]);
    }
}

void listing_byte_code (FILE* listing_file, asm_struct Assembler,
                        char* command_str,  char*  argument_str,
                        int   command_int,  int    argument_int) {

    fprintf(listing_file, "[%3ld] [%3ld] \t",   Assembler.ind_counter - 2, Assembler.ind_counter - 1);
    fprintf(listing_file, "%8s %8s \t",       command_str,                  argument_str);

    if (argument_int == POISON)
        fprintf(listing_file, "%8d %8s\n", command_int, "POISON"); // each command has argument (it can be fake (POISON))

    else                    
        fprintf(listing_file, "%8d %8d\n", command_int,  argument_int);
}

void listing_labels_array (FILE* listing_file, asm_struct* Assembler) {

    fprintf(listing_file, "\n-------------------------------------------------------------------------------\n");

    fprintf(listing_file, "  Index of label\t" "Label name\t" "Index in byte-code\t" "    " "Hash\n\n");
    for (int el_num = 0; el_num < LABEL_BUF_SIZE; el_num++) {
        
        if (el_num < Assembler -> label_ind_counter) 
        {
            fprintf(listing_file, "\t\b*[%2d]\t", el_num);
            fprintf(listing_file, "%16s:\t", Assembler -> labels_array[el_num].name);
        } 
        
        else 
        {
            fprintf(listing_file, "\t[%2d]\t", el_num);
            fprintf(listing_file, "%16s:\t", Assembler -> labels_array[el_num].name); 
        }

        fprintf(listing_file, "%8d\t",       Assembler -> labels_array[el_num].index);
        fprintf(listing_file, "%16d\n",      Assembler -> labels_array[el_num].hash);
    }     

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

void command_line_flags(int argc, char* argv[], FILE** input_file, FILE** output_file) {

    int opt = 0;

    while ( (opt = getopt(argc, argv, "i:o:")) != -1) 
    {
        switch (opt) 
        {
            case 'i':
                *input_file = fopen(optarg, "r");
                break;

            case 'o':
                *output_file = fopen(optarg, "w");
                break;

            case '?':
                fprintf(stderr, "command_line_flags: unknown flag (%s)\n", optarg);
                break;

            default:
                fprintf(stderr, "command_line_flags: unexpected error\n");
        }
    }
}

int Assembler_struct_Dtor (asm_struct* Assembler) {
    assert(Assembler);

    free(Assembler -> asm_code_buf);
    free(Assembler -> byte_code_buf);
    free(Assembler -> pointers_array);
    free(Assembler -> cmd_info_arr);

    for (size_t labels_arr_el = 0; labels_arr_el < Assembler -> label_ind_counter; labels_arr_el++)
        free(Assembler -> labels_array[labels_arr_el].name);

    free(Assembler -> labels_array);

    return DESTROY_SUC;
}
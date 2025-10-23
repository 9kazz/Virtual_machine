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

    SAFE_CALLOC(temp_asm_code_buf, fsize, char)
    Assembler -> asm_code_buf = temp_asm_code_buf;
 
    fill_buffer(Assembler, file_input, fsize);

    Assembler -> count_of_commands = str_change_char(Assembler, '\n', '\0') + 1;

    SAFE_CALLOC(temp_pointers_arr, Assembler -> count_of_commands + 1, char*)
    Assembler -> pointers_array = temp_pointers_arr;

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

    // for (size_t i = 0; i < new_size_buffer; i++)
    //     printf("%d ", Assembler->asm_code_buf[i]);

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
    for (size_t el_num = 0; el_num < LABEL_BUF_SIZE; el_num++) {
        
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

    for (size_t labels_arr_el = 0; labels_arr_el < LABEL_BUF_SIZE; labels_arr_el++)
        free(Assembler -> labels_array[labels_arr_el].name);

    free(Assembler -> labels_array);

    return DESTROY_SUC;
}

int hash_function (const char* hashing_str) {
    assert(hashing_str);

    int hash = 0;
    int hash_constant = 0;

    for (size_t char_num = 0; hashing_str[char_num] != '\0' ; char_num++) {
            hash_constant = hash * PRIME_COEF_HASH;
            hash = hash_constant + hashing_str[char_num];

            if ( (hash_constant > 0 && hashing_str[char_num] > 0 && hash < 0) ||   // overflow of int
                 (hash_constant < 0 && hashing_str[char_num] < 0 && hash > 0) )  
            {
                fprintf(stderr, "Overflow of hash\n");
            }
    }


    return hash;
}

int comp_sort_int(const void* struct_a, const void* struct_b) {

    const CmdStruct* cmd_a = (const CmdStruct*) struct_a;
    const CmdStruct* cmd_b = (const CmdStruct*) struct_b;

    int hash_a = cmd_a -> hash;
    int hash_b = cmd_b -> hash;

    if (hash_a < hash_b) 
        return -1;

    if (hash_a > hash_b) 
        return 1;

    return 0;
}

int comp_sort_int_label(const void* struct_a, const void* struct_b) {

    const LabelStruct* cmd_a = (const LabelStruct*) struct_a;
    const LabelStruct* cmd_b = (const LabelStruct*) struct_b;

    int hash_a = cmd_a -> hash;
    int hash_b = cmd_b -> hash;

    if (hash_a < hash_b) 
        return -1;

    if (hash_a > hash_b) 
        return 1;

    return 0;
}

#define BIN_SEARCH_STRUCT_ARR(name, array_type, arr_size, finding_value)                            \
                                                                                                    \
size_t bin_search_##name(array_type* array_type##_arr, int finding_int) {                           \
    assert(array_type##_arr);                                                                       \
                                                                                                    \
    size_t left  = 0;                                                                               \
    size_t right = arr_size - 1;                                                                    \
    size_t mid   = 0;                                                                               \
                                                                                                    \
    while (left <= right) {                                                                         \
                                                                                                    \
        mid = left + (right - left) / 2;                                                            \
                                                                                                    \
        if (array_type##_arr[mid]finding_value == finding_int)                                      \
            return mid;                                                                             \
                                                                                                    \
        if (array_type##_arr[mid]finding_value < finding_int)                                       \
            left = mid + 1;                                                                         \
                                                                                                    \
        if (array_type##_arr[mid]finding_value > finding_int)                                       \
            right = mid - 1;                                                                        \
    }                                                                                               \
                                                                                                    \
    return NOT_FIND;                                                                                \
}

BIN_SEARCH_STRUCT_ARR(LabelStruct_hash,    LabelStruct, LABEL_BUF_SIZE,    .hash)
BIN_SEARCH_STRUCT_ARR(CmdStruct_hash,      CmdStruct,   MAX_COUNT_OF_CMD,  .hash)

#undef BIN_SEARCH_STRUCT_ARR
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>

#include "processor.h"
#include "stack_utils.h"
#include "stack.h"
#include "type_struct_def.h"    
#include "utils.h"
#include "LogFile.h"

int* create_bite_code_buf (FILE* input_file, size_t* bite_code_size) {

    *bite_code_size = size_of_file(input_file);

    int* bite_code_buf = (int*) calloc(*bite_code_size, sizeof(int));

    int command  = 0;
    int argument = 0;

    size_t el_num = 0;

    while (el_num  < *bite_code_size) {

        int end_of_file_checking = fscanf(input_file, "%d %d", &command, &argument);

        if (end_of_file_checking == EOF)
            break;

        bite_code_buf[ el_num++ ] = command;
        bite_code_buf[ el_num++ ] = argument;
    }

    return bite_code_buf;
}

size_t size_of_file (FILE* file_input) {
    struct stat file_info;

    int descriptor = fileno(file_input);

    int check = fstat(descriptor, &file_info);

    assert(check != -1);
    
        if (check == -1) {
            fprintf(stderr, "size_of_file: error");
            return 0;
        }
    
    return file_info.st_size;         
}

CalcErr_t Calc_Dtor (CalcStruct* calc_struct) {
    assert(calc_struct);

    size_t size_of_struct  = sizeof(*calc_struct);

    stack_struct stack     = calc_struct -> calc_stack;
    int* bite_code_pointer = calc_struct -> bite_code.buffer;

    Stack_Dtor(&stack);

    free(bite_code_pointer);
    
    memset(calc_struct, POISON, size_of_struct);

    return DESTROY_SUC;
}

CalcErr_t Calc_Verify (CalcStruct* calc_struct, const char* checking_function) {

    int error_code = 0;

    #ifdef DEBUG_STACK_VERIFY

        if (calc_struct == NULL) {

            fprintf(LogFile, "Calc_Verify: NULL pointer to CalcStruct" "\n");
            fprintf(LogFile, "Error code: %d (NULL_POINT_CALC)" "\n\n", NULL_POINT_CALC);

            Calc_Dump(calc_struct);

            error_code = error_code | NULL_POINT_CALC;

            fprintf(LogFile, "Calc_Verify: verification ended with error code: %d\n", error_code);
            return error_code;
        }
        
        if (calc_struct -> bite_code.size < 0) {

            fprintf(LogFile, "Calc_Verify: negative size of bite code buffer in %s" "\n", checking_function);
            fprintf(LogFile, "Error code: %d (NEG_BC_SIZE)" "\n\n", NEG_BC_SIZE);

            Calc_Dump(calc_struct);

            error_code = error_code | NEG_BC_SIZE;   
            
            fprintf(LogFile, "Calc_Verify: verification ended with error code: %d\n", error_code);
            return error_code;
        }
        
        if (calc_struct -> bite_code.buffer == NULL) {

            fprintf(LogFile, "Calc_Verify: NULL pointer to bite code buffer in %s" "\n", checking_function);
            fprintf(LogFile, "Error code: %d (NULL_POINT_BC_BUF)" "\n\n", NULL_POINT_BC_BUF);

            Calc_Dump(calc_struct);

            error_code = error_code | NULL_POINT_BC_BUF;     
            
            fprintf(LogFile, "Calc_Verify: verification ended with error code: %d\n", error_code);
            return error_code;
        }


        if (calc_struct -> register_buf == NULL) {

            fprintf(LogFile, "Calc_Verify: NULL poiner to register buffer in %s" "\n", checking_function);
            fprintf(LogFile, "Error code: %d (NULL_POINT_REG)" "\n\n", NULL_POINT_REG);

            Calc_Dump(calc_struct);

            error_code = error_code | NULL_POINT_REG;       
            
            fprintf(LogFile, "Calc_Verify: verification ended with error code: %d\n", error_code);
            return error_code;
        }        

    #endif
    
    Stack_Verify( &(calc_struct -> calc_stack), checking_function);

    return error_code;
}

CalcErr_t Calc_Dump(CalcStruct* calc_struct) {

    #if defined(DEBUG_STACK_VERIFY) || defined(DEBUG_CANARY) || defined(DEBUG_HASH)

        if (calc_struct == NULL) {
            fprintf(LogFile, "Calc_Dump: NULL pointer to CalcStruct" "\n\n");
            return DUMP_FAILED;
        }

        if (calc_struct -> bite_code.buffer == NULL) {
            fprintf(LogFile, "Calc_Dump: NULL pointer to bite code buffer" "\n\n");
            return DUMP_FAILED;
        }        

        if (calc_struct -> register_buf == NULL) {
            fprintf(LogFile, "Calc_Dump: NULL pointer to register buffer" "\n\n");
            return DUMP_FAILED;
        }

        fprintf(LogFile, "Calc_Dump prints:" "\n");
        fprintf(LogFile, "[%p]" "\n", calc_struct);
        fprintf(LogFile, "Bite code structure:" "\n");
        fprintf(LogFile, "{" "\n");
        fprintf(LogFile, "size = %d" "\n", calc_errors -> bite_code.size);
        fprintf(LogFile, "buffer:" "\n");
            fprintf(LogFile, "\t" "{" "\n");
        
                for (size_t el_num = 0; el_num < calc_struct -> bite_code.size; el_num++) 
                    fprintf(LogFile, "\t\t" "[%d] = %d" "\n", el_num, calc_struct -> bite_code.buffer[el_num]);
        
            fprintf(LogFile, "\t" "}" "\n");
        
        fprintf(LogFile, "Register buffer:" "\n");
            fprintf(LogFile, "\t" "{" "\n");
        
                for (size_t el_num = 0; el_num < COUNT_OF_REG; el_num++) 
                    fprintf(LogFile, "\t\t" "[%d] = %d" "\n", el_num, calc_struct -> register_buf[el_num]);
        
            fprintf(LogFile, "\t" "}" "\n");

        Stack_Dump( &(calculator -> calc_stack));
        
        fprintf(LogFile, "\n\n");
        
    #else
        
        return RELEASE_MODE;
        
    #endif
    
        return DUMP_SUCCESS;
}

#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>

#include "utils.h"
#include "calculator.h"
#include "LogFile.h"
#include "type_struct_def.h"    
#include "stack_utils.h"
#include "stack.h"

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
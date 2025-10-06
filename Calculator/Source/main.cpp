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


FILE* LogFile = fopen("errors.log", "w");

int main() {
    FILE* input_file  = fopen("input_file.txt",  "r");
    FILE* output_file = fopen("output_file.txt", "w");
    assert(LogFile);
    assert(input_file);
    assert(output_file);

    CALC_CTOR(calc_struct)

    // for (size_t i = 0; i < bite_code_size; i += 2)
    //     printf("%d\t%d\n", bite_code_buf[i], bite_code_buf[i + 1]);

    // INIT_STK(calc_stack);
    // Stack_Ctor(&calc_stack, bite_code_size);

    calculator(&calc_struct);

    for (int i = 0; i < calc_struct.calc_stack.cur_position; i++)
        printf("%d ", calc_struct.calc_stack.data[i]);
    // printf("\n%d", calc_struct.calc_stack.data[0]);

    fclose(input_file);
    fclose(output_file);
    fclose(LogFile);

    return 0;
}
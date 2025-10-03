#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>

#include "utils.h"
#include "calculator.h"

int main() {
    FILE* input_file  = fopen("input_file.txt",  "r");
    FILE* output_file = fopen("output_file.txt", "w");
    assert(input_file);
    assert(output_file);

    size_t bit_code_size = 0;
    int* bit_code_buf = create_bit_code_buf(input_file, &bit_code_size);

    for (size_t i = 0; i < bit_code_size; i++) {
        printf("%d\n", bit_code_buf[i]);
    }

    fclose(input_file);
    fclose(output_file);

    return 0;
}
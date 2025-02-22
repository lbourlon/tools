#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define LB_INCLUDE_IMPLEMENTATION
#include "b64.h"


int main(int argc, char** argv) {
    encoding encoding_type;
    bool is_encoding_mode = false;

    _b64_arg_parse(argc, argv, &encoding_type, &is_encoding_mode);

    switch (encoding_type) {
        case BASE_64:
            if (is_encoding_mode) {
                b64encode_stream(stdin, stdout);
            } else {
                b64decode_stream(stdin, stdout);
            }
            break;

        default:
            printf("Reached default switch case fixme :)\n");            
            return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

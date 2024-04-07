#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define LB_INCLUDE_IMPLEMENTATION
#include "b64.h"

typedef enum encoding {
    UNSUPPORTED,
    BASE_16,
    BASE_32,
    BASE_64,
} encoding;


void print_help_and_exit();
void arg_parse(int argc, char** argv, encoding* encoding_type, bool* is_encoding_mode);

int main(int argc, char** argv) {
    encoding encoding_type;
    bool is_encoding_mode = false;

    arg_parse(argc, argv, &encoding_type, &is_encoding_mode);

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

void print_help_and_exit(){
    printf("Usage: messiah [MODE] [ENCODING]\n");
    printf("This program reads from stdin and writes to stdout\n");
    printf("\n");
    printf(" Modes:\n");
    printf("  -e, --encode     encoding mode\n");
    printf("  -d, --decode     decoding mode\n");
    printf("  -h, --help       get some help\n");

    printf("\n");
    printf(" Encodings:\n");
    printf("  -b64            base64 encoding\n");

    printf("\n");
    printf(" Examples:\n");
    printf("  echo -n \"Example string for encoding\" | messiah -e -b64\n");
    printf("  echo -n \"VGhlIG1lc3NpYWggY29udmVydHM=\" | messiah -d -b64\n");

    exit(EXIT_SUCCESS);
}

void print_get_help_and_exit() {
    printf("run with --help to get some help\n");
    exit(EXIT_FAILURE);
}

void print_issue_with_mode(){
    printf("encoding '-e' and decoding '-d' are mutually exclsuive\n");
    printf("and can only be set one\n");
    print_get_help_and_exit();
};


void print_issue_with_encoding(){
    printf("encoding '-e' and decoding '-d' are mutually exclsuive\n");
    printf("and can only be set one\n");
    print_get_help_and_exit();
};

void arg_parse(int argc, char** argv, encoding* encoding_type, bool* is_encoding_mode) {
    int mode_flag = 0;
    int enc_flag = 0;

    // Loops through once checking if help is needed
    for (int i = 1; i < argc; i++) {
        if (strncmp("-h", argv[i], 2) == 0 || strncmp("--help", argv[i], 6) == 0) {
            print_help_and_exit();
        }
    }

    // Loops again to get other options
    for (int i = 1; i < argc; i++) {
        if (strncmp("-e", argv[i], 2) == 0 || strncmp("--encode", argv[i], 8) == 0) {
            if (enc_flag) {
                print_issue_with_mode();
            }
            enc_flag = 1;
            *is_encoding_mode = true;
        }
        
        else if (strncmp("-d", argv[i], 2) == 0 || strncmp("--decode", argv[i], 8) == 0) {
            if (enc_flag) {
                print_issue_with_mode();
            }
            enc_flag = 1;
            *is_encoding_mode = false;
        }

        else if (strncmp("-b64", argv[i], 3) == 0) {
            if (mode_flag) {
                print_issue_with_encoding();
            }
            mode_flag = 1;
            *encoding_type = BASE_64;
        }

        else {
            printf("unkown options '%s'\n", argv[i]);
            print_get_help_and_exit();
        }
    }

    if ( !enc_flag || !mode_flag) {
        printf("Received too few arguments\n");
        print_get_help_and_exit();
    }
}




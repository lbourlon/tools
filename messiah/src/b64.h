#ifndef LB_B64
#define LB_B64

#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// As a library
void b64encode(char* plain, int len_plain, char* encoded);
void b64decode(char* encoded, int len_encoded, char* decoded);
void b64encode_stream(FILE* in_stream, FILE* out_stream);
void b64decode_stream(FILE* in_stream, FILE* out_stream);

// As full cli app
typedef enum encoding {
    UNSUPPORTED,
    BASE_16,
    BASE_32,
    BASE_64,
} encoding;

void _b64_print_help_and_exit();
void _b64_print_get_help_and_exit();
void _b64_print_issue_with_mode();
void _b64_print_issue_with_encoding();
void _b64_arg_parse(int argc, char** argv, encoding* encoding_type, bool* is_encoding_mode);

int b64_cli_main(int argc, char** argv);



#endif
#define LB_INCLUDE_IMPLEMENTATION
#ifdef LB_INCLUDE_IMPLEMENTATION

char* table64 = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

uint8_t _get_six_bits_from_char(char c) {
    uint8_t six_bits = 65;
    if ( c >= '0' && c <= '9') { six_bits = c - 48 + 52; }
    else if ('a' <= c && c <= 'z') { six_bits = c - 65 - 32 + 26; }
    else if ('A' <= c && c <= 'Z') { six_bits = c - 'A'; }
    else if (c == '+')  { six_bits = 62; }
    else if (c == '/')  { six_bits = 63; }
    else if (c == '=') { six_bits = 0;} 
    return six_bits;
}

int _read_nbytes_from_stream(FILE* stream, int nbytes, char* out_batch) {
    char c;
    int read_bytes = 0;

    while (read_bytes < nbytes) {
        c = fgetc(stream);
        if (c == EOF) {
            return read_bytes;
        } else {
            out_batch[read_bytes] = c;
            read_bytes++;
        }
    }
    return read_bytes;
}

void b64encode_stream(FILE* in, FILE* out) {
    uint8_t first_bits = 0;
    uint8_t last_bits = 0;

    const int batch_size = 3;
    char plain[batch_size];
    int read_bytes = 0;
    uint8_t c;
    while( ( read_bytes = _read_nbytes_from_stream(in, batch_size, plain) ) > 0) {
        // printf("bytes_read %d\n", read_bytes);
        // first byte [001100][00]
        c = plain[0]; 
        first_bits = c >> 2;
        last_bits  = c << 4 & 63;
        fputc(table64[first_bits], out);

        // Add padding if needed
        if (read_bytes == 1) {
            fputc(table64[last_bits], out);
            fputc('=', out); fputc('=', out);
            break;
        }

        // second byte [0011][0000]
        c = plain[1];
        first_bits = c >> 4 | last_bits;
        last_bits  = c << 2 & 63;
        fputc(table64[first_bits], out);

        // Add padding if needed
        if (read_bytes == 2) {
            fputc(table64[last_bits], out);
            fputc('=', out);
            break;
        }

        // third byte [00][110000]
        c = plain[2];
        first_bits = c >> 6 | last_bits;
        last_bits  = c & 63; 
        fputc(table64[first_bits], out);
        fputc(table64[last_bits], out);
    }
}


void b64decode_stream(FILE* in, FILE* out) {
    const int batch_size = 4;
    char encoded[batch_size];
    int read_bytes = 0;

    while ( ( read_bytes = _read_nbytes_from_stream(in, batch_size, encoded) ) > 0) {
        if (read_bytes != batch_size) {
            fprintf(stderr, 
                "Error in base64 decoding tried to read %d bytes got %d!\n",
                batch_size, read_bytes
            );
            return;
        }
        uint8_t a = _get_six_bits_from_char(encoded[0]);
        uint8_t b = _get_six_bits_from_char(encoded[1]);
        uint8_t c = _get_six_bits_from_char(encoded[2]);
        uint8_t d = _get_six_bits_from_char(encoded[3]);

        fputc(a << 2 | b >> 4, out);
        fputc(b << 4 | c >> 2, out);
        fputc(c << 6 | d     , out);
    }
    fputc('\0', out);
}


void _b64_print_help_and_exit(){
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

void _b64_print_get_help_and_exit() {
    printf("run with --help to get some help\n");
    exit(EXIT_FAILURE);
}

void _b64_print_issue_with_mode(){
    printf("encoding '-e' and decoding '-d' are mutually exclsuive\n");
    printf("and can only be set one\n");
    _b64_print_get_help_and_exit();
}


void _b64_print_issue_with_encoding(){
    printf("encoding '-e' and decoding '-d' are mutually exclsuive\n");
    printf("and can only be set one\n");
    _b64_print_get_help_and_exit();
}

void _b64_arg_parse(int argc, char** argv, encoding* encoding_type, bool* is_encoding_mode) {
    int mode_flag = 0;
    int enc_flag = 0;

    // Loops through once checking if help is needed
    for (int i = 1; i < argc; i++) {
        if (strncmp("-h", argv[i], 2) == 0 || strncmp("--help", argv[i], 6) == 0) {
            _b64_print_help_and_exit();
        }
    }

    // Loops again to get other options
    for (int i = 1; i < argc; i++) {
        if (strncmp("-e", argv[i], 2) == 0 || strncmp("--encode", argv[i], 8) == 0) {
            if (enc_flag) {
                _b64_print_issue_with_mode();
            }
            enc_flag = 1;
            *is_encoding_mode = true;
        }
        
        else if (strncmp("-d", argv[i], 2) == 0 || strncmp("--decode", argv[i], 8) == 0) {
            if (enc_flag) {
                _b64_print_issue_with_mode();
            }
            enc_flag = 1;
            *is_encoding_mode = false;
        }

        else if (strncmp("-b64", argv[i], 3) == 0) {
            if (mode_flag) {
                _b64_print_issue_with_encoding();
            }
            mode_flag = 1;
            *encoding_type = BASE_64;
        }

        else {
            printf("unkown options '%s'\n", argv[i]);
            _b64_print_get_help_and_exit();
        }
    }

    if ( !enc_flag || !mode_flag) {
        printf("Received too few arguments\n");
        _b64_print_get_help_and_exit();
    }
}


#endif

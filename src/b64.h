#ifndef LB_B64
#define LB_B64

#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

void b64encode(char* plain, int len_plain, char* encoded);
void b64decode(char* encoded, int len_encoded, char* decoded);
void b64encode_stream(FILE* in_stream, FILE* out_stream);
void b64decode_stream(FILE* in_stream, FILE* out_stream);

#endif

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

#endif

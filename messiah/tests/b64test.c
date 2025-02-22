#include <criterion/criterion.h>
#include <criterion/internal/assert.h>
#include <stdio.h>
#include <stdbool.h>

#define LB_INCLUDE_IMPLEMENTATION
#include "../src/b64.h"

void suite_setup(void){ };
void suite_teardown(void){ };

void b64_encode_test(char* input, char* output, bool enc){
    char temp_string[300];
    FILE* out_stream = tmpfile();
    FILE* in_stream = tmpfile();

    fputs(input, in_stream);
    rewind(in_stream);

    if (enc) {
        b64encode_stream(in_stream, out_stream);
    } else {
        b64decode_stream(in_stream, out_stream);
    }
    rewind(out_stream);

    fgets(temp_string, strlen(output)+1, out_stream);

    cr_expect_str_eq(output, temp_string, "expected '%s'\n--> '%s'\ngot '%s'",
                     input, output, temp_string);

    fclose(in_stream);
    fclose(out_stream);
}

TestSuite(b64, .init=suite_setup, .fini=suite_teardown);
Test(b64, encoding_various_lenghts) {
    b64_encode_test("a", "YQ==", true);
    b64_encode_test("aa", "YWE=", true);
    b64_encode_test("aaa", "YWFh", true);
    b64_encode_test("test strings are good", "dGVzdCBzdHJpbmdzIGFyZSBnb29k", true);
    b64_encode_test("test strings are goo", "dGVzdCBzdHJpbmdzIGFyZSBnb28=", true);
    b64_encode_test("test strings are go", "dGVzdCBzdHJpbmdzIGFyZSBnbw==", true);
    b64_encode_test("test strings are g", "dGVzdCBzdHJpbmdzIGFyZSBn", true);
}

Test(b64, decoding_various_lenghts) {
    b64_encode_test("YQ==", "a", false);
    b64_encode_test("YWE=", "aa", false);
    b64_encode_test("YWFh", "aaa", false);
    b64_encode_test("dGVzdCBzdHJpbmdzIGFyZSBnb29k", "test strings are good", false);
    b64_encode_test("dGVzdCBzdHJpbmdzIGFyZSBnb28=", "test strings are goo", false);
    b64_encode_test("dGVzdCBzdHJpbmdzIGFyZSBnbw==", "test strings are go", false);
    b64_encode_test("dGVzdCBzdHJpbmdzIGFyZSBn", "test strings are g", false);
}

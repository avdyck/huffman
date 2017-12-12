//
// Created by avdyck on 07.10.17.
//

#include "../src/util/util.h"

#include <stdio.h>
#include "minunit.h"
#include "compression/test_huffman.h"
#include <time.h>

int tests_run = 0;

static char *all_tests() {
    TIMER_INIT();

    for (int i = 1; i <= 5; i++) {
        TIMER_START();
        mu_run_test(test_huffman, i, "../tests/files/Small.txt");
        mu_run_test(test_huffman, i, "../tests/files/SingleChar.txt");
        mu_run_test(test_huffman, i, "../tests/files/Kapot.txt");
        mu_run_test(test_huffman, i, "../tests/files/OneChar.txt");
        mu_run_test(test_huffman, i, "../tests/files/LoremIpsum.txt");
        mu_run_test(test_huffman, i, "../tests/files/DoubleChars.txt");
        mu_run_test(test_huffman, i, "../tests/files/DA3.pdf");
        mu_run_test(test_huffman, i, "../tests/files/Empty.txt");
        TIMER_STOP();
    }

    return NULL;
}

int main(int argc, char **argv) {
    char *result;

    INIT_MEMLEAK_TESTING();

    result = all_tests();

    if (result != 0) {
        printf("%s\n", result);
    } else {
        printf("ALL TESTS PASSED\n");
    }
    printf("Tests run: %d\n", tests_run);

    GET_MEMLEAKS();

    return result != 0;
}

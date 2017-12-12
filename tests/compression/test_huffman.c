//
// Created by avdyck on 10.10.17.
//

#include "../../src/io/file_read.h"
#include "../../src/util/error.h"
#include "../minunit.h"
#include "../../src/_main.h"

char *test_huffman(int huffman_type, char *file) {
    // https://en.wikipedia.org/wiki/File:Physics_matter_state_transition_1_en.svg
    // Symbolizes the state of the file
    char *solid       = file;
    char *melted      = "../tests/files/melted.txt";
    char *vaporized   = "../tests/files/vaporized.txt";
    char *condensated = "../tests/files/condensated.txt";
    char *frozen      = "../tests/files/frozen.txt";

    char type[5];

    snprintf(type, sizeof(type), "%i", huffman_type);

    {
        char *args_1[] = {"huffman", "-t", type, "-c", "-i", solid,       "-o", melted };
        char *args_2[] = {"huffman", "-t", type, "-c", "-i", melted,      "-o", vaporized };
        char *args_3[] = {"huffman", "-t", type, "-d", "-i", vaporized,   "-o", condensated };
        char *args_4[] = {"huffman", "-t", type, "-d", "-i", condensated, "-o", frozen };

        _main(8, args_1);
        _main(8, args_2);
        _main(8, args_3);
        _main(8, args_4);
    }

    {
        bool bool1, bool2;
        FILE *initial, *result;
        int b1, b2;

//        printf("\n%s\n", file);

        initial = fopen(solid, "rb"); if (initial == NULL) throw("Input file not defined.\n");
        result  = fopen(frozen, "rb"); if (result == NULL) throw("Output file not defined.\n");

        while ((bool1 = ((b1 = getc(initial)) != EOF))  | (bool2 = ((b2 = getc(result)) != EOF))) {
//            printf("%c%c", b1, b2);

            mu_assert("Files are not encoded correctly.", b1 == b2);
            mu_assert("One file was finished prematurely.", bool1 && bool2);
        }

        mu_assert("EOF not reached", (b1 = getc(initial) == EOF)  && (b2 = getc(result) == EOF));

        fclose(initial);
        fclose(result);
    }



    return NULL;
}

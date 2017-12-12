//
// Created by avdyck on 07.10.17.
//

#ifndef HUFFMAN_MINUNIT_H
#define HUFFMAN_MINUNIT_H

char * formatted_message(const char *message, const char *fun);

#define mu_assert(message, test) do { if (!(test)) return formatted_message(message, __PRETTY_FUNCTION__); } while (0)
#define mu_run_test(test, ...) do { char *message = test(__VA_ARGS__); tests_run++; \
                                if (message) return message; } while (0)

extern int tests_run;

#endif //HUFFMAN_MINUNIT_H

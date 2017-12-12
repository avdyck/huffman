//
// Created by avdyck on 07.10.17.
//

#include <stdio.h>
#include "minunit.h"

char *formatted_message(const char *message, const char *fun) {
    static char buf[512];

    snprintf(buf, sizeof(buf), "%s%s%s%s", "A test has failed: ", fun, "\nError message: ", message);

    return buf;
}
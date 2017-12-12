//
// Created by avdyck on 07.10.17.
//

#include <stdio.h>
#include <stdlib.h>
#include "error.h"

void THROW(const char *fun, const char *file, int line, char *message)  {
    fprintf(stderr, "Function '%s' has failed ('%s' at line %i):\n%s", fun, file, line, message);
    exit(EXIT_FAILURE);
}

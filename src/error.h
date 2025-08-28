#pragma once
#include <stdio.h>
#include <stdlib.h>

#define PANIC(...)                        \
    do {                                  \
        fprintf(stderr, "Fatal error: "); \
        fprintf(stderr, __VA_ARGS__);     \
        fprintf(stderr, "\n");            \
        exit(42);                         \
    } while (0);

#pragma once
#include <stdlib.h>

static inline void EXIT_Error(const char *a1, ...)
{
     exit(0);
}

static inline void EXIT_Clean(void)
{
    exit(0);
}

static inline void EXIT_Install(void (*a1)(int a1))
{

}

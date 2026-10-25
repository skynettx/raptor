#pragma once
#include <stdlib.h>

#ifdef __vita__
#include <limits.h>
#ifndef PATH_MAX
#define PATH_MAX 1024
#endif // PATH_MAX
#endif // __vita__

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

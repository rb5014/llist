#pragma once
#include <stdio.h>
#include <time.h>

#define TEST_LOG(...) do { \
    time_t now = time(NULL); \
    char timestr[20]; \
    strftime(timestr, 20, "%H:%M:%S", localtime(&now)); \
    fprintf(stderr, "[%s] %s:%d: ", timestr, __FILE__, __LINE__); \
    fprintf(stderr, __VA_ARGS__); \
    fputc('\n', stderr); \
} while(0)

#if __STDC_VERSION__ >= 202311L
// C23's #embed can be used here if needed
#define TEST_LOG(fmt, ...) \
    fprintf(stderr, "[%s] %s:%d: " fmt "\n", \
            _Generic(0, default: (char[]){0}, \
            time_format()), __FILE__, __LINE__ __VA_OPT__(,) __VA_ARGS__)
#endif

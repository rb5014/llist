#pragma once
#include <stdio.h>
#include <time.h>

#define TEST_LOG(fmt, ...) do { \
    time_t now = time(NULL); \
    char timestr[20]; \
    strftime(timestr, 20, "%H:%M:%S", localtime(&now)); \
    fprintf(stderr, "[%s] %s:%d: " fmt "\n", \
            timestr, __FILE__, __LINE__, ##__VA_ARGS__); \
} while(0)

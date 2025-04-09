#pragma once
#include <stdio.h>

#define TEST_START printf("Running %s... ", __func__)
#define TEST_PASS printf("✓\n")
#define ASSERT_EQ(a, b) if ((a) != (b)) { printf("FAIL: %s:%d (%d != %d)\n", __FILE__, __LINE__, (a), (b)); exit(1); }
#define ASSERT_NOT_NULL(p) if ((p) == NULL) { printf("FAIL: %s:%d (NULL)\n", __FILE__, __LINE__); exit(1); }
#define ASSERT_NULL(p) if ((p) != NULL) { printf("FAIL: %s:%d (not NULL)\n", __FILE__, __LINE__); exit(1); }
#define ASSERT_TRUE(cond) if (!(cond)) { printf("FAIL: %s:%d (%s)\n", __FILE__, __LINE__, #cond); exit(1); }
#define ASSERT_FALSE(cond) if (cond) { printf("FAIL: %s:%d (%s)\n", __FILE__, __LINE__, #cond); exit(1); }
#define ASSERT_STR_EQ(a, b) if (strcmp((a), (b)) != 0) { printf("FAIL: %s:%d (%s != %s)\n", __FILE__, __LINE__, (a), (b)); exit(1); }
#define ASSERT_INT_EQ(a, b) if ((a) != (b)) { printf("FAIL: %s:%d (%d != %d)\n", __FILE__, __LINE__, (a), (b)); exit(1); }
#define ASSERT_FLOAT_EQ(a, b) if (fabs((a) - (b)) > 0.0001) { printf("FAIL: %s:%d (%f != %f)\n", __FILE__, __LINE__, (a), (b)); exit(1); }
#define ASSERT_DOUBLE_EQ(a, b) if (fabs((a) - (b)) > 0.0001) { printf("FAIL: %s:%d (%lf != %lf)\n", __FILE__, __LINE__, (a), (b)); exit(1); }
#define ASSERT_PTR_EQ(a, b) if ((a) != (b)) { printf("FAIL: %s:%d (%p != %p)\n", __FILE__, __LINE__, (a), (b)); exit(1); }
#define ASSERT_SIZE_T_EQ(a, b) if ((a) != (b)) { printf("FAIL: %s:%d (%zu != %zu)\n", __FILE__, __LINE__, (a), (b)); exit(1); }
#define ASSERT_ERRNO(err) if (errno != (err)) { printf("FAIL: %s:%d (%d != %d)\n", __FILE__, __LINE__, errno, (err)); exit(1); }
#define ASSERT_ERRNO_EQ(err, expected) if ((err) != (expected)) { printf("FAIL: %s:%d (%d != %d)\n", __FILE__, __LINE__, (err), (expected)); exit(1); }
#define ASSERT_ERRNO_NOT_EQ(err, expected) if ((err) == (expected)) { printf("FAIL: %s:%d (%d == %d)\n", __FILE__, __LINE__, (err), (expected)); exit(1); }
#define ASSERT_ERRNO_NOT_SET() if (errno != 0) { printf("FAIL: %s:%d (errno != 0)\n", __FILE__, __LINE__); exit(1); }
#define ASSERT_ERRNO_SET() if (errno == 0) { printf("FAIL: %s:%d (errno == 0)\n", __FILE__, __LINE__); exit(1); }
#define ASSERT_ERRNO_SET_EQ(err) if (errno != (err)) { printf("FAIL: %s:%d (errno != %d)\n", __FILE__, __LINE__, (err)); exit(1); }
#define ASSERT_ERRNO_SET_NOT_EQ(err) if (errno == (err)) { printf("FAIL: %s:%d (errno == %d)\n", __FILE__, __LINE__, (err)); exit(1); }

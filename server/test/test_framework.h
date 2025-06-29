/*
** EPITECH PROJECT, 2025
** Zappy
** File description:
** Simple unit testing framework
*/

#ifndef TEST_FRAMEWORK_H_
#define TEST_FRAMEWORK_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

// Test framework macros
#define TEST_ASSERT(condition) \
    do { \
        if (!(condition)) { \
            fprintf(stderr, "ASSERTION FAILED: %s:%d: %s\n", __FILE__, __LINE__, #condition); \
            return 0; \
        } \
    } while(0)

#define TEST_ASSERT_EQUAL(expected, actual) \
    do { \
        if ((expected) != (actual)) { \
            fprintf(stderr, "ASSERTION FAILED: %s:%d: Expected %d, got %d\n", __FILE__, __LINE__, (int)(expected), (int)(actual)); \
            return 0; \
        } \
    } while(0)

#define TEST_ASSERT_STRING_EQUAL(expected, actual) \
    do { \
        if (strcmp((expected), (actual)) != 0) { \
            fprintf(stderr, "ASSERTION FAILED: %s:%d: Expected '%s', got '%s'\n", __FILE__, __LINE__, (expected), (actual)); \
            return 0; \
        } \
    } while(0)

#define TEST_ASSERT_NULL(ptr) \
    do { \
        if ((ptr) != NULL) { \
            fprintf(stderr, "ASSERTION FAILED: %s:%d: Expected NULL, got non-NULL\n", __FILE__, __LINE__); \
            return 0; \
        } \
    } while(0)

#define TEST_ASSERT_NOT_NULL(ptr) \
    do { \
        if ((ptr) == NULL) { \
            fprintf(stderr, "ASSERTION FAILED: %s:%d: Expected non-NULL pointer\n", __FILE__, __LINE__); \
            return 0; \
        } \
    } while(0)

// Test function signature
typedef int (*test_func_t)(void);

// Test structure
typedef struct {
    const char *name;
    test_func_t func;
} test_case_t;

// Global test counters
extern int tests_run;
extern int tests_passed;
extern int tests_failed;

// Function prototypes
void run_test(const char *test_name, test_func_t test_func);
void run_test_suite(const char *suite_name, test_case_t *tests, int num_tests);
void print_test_results(void);

#endif /* !TEST_FRAMEWORK_H_ */

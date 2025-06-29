/*
** EPITECH PROJECT, 2025
** Zappy
** File description:
** Simple unit testing framework implementation
*/

#include "test_framework.h"

int tests_run = 0;
int tests_passed = 0;
int tests_failed = 0;

void run_test(const char *test_name, test_func_t test_func)
{
    printf("Running test: %s... ", test_name);
    fflush(stdout);
    
    tests_run++;
    int result = test_func();
    
    if (result) {
        printf("PASSED\n");
        tests_passed++;
    } else {
        printf("FAILED\n");
        tests_failed++;
    }
}

void run_test_suite(const char *suite_name, test_case_t *tests, int num_tests)
{
    printf("\n=== Running Test Suite: %s ===\n", suite_name);
    
    for (int i = 0; i < num_tests; i++) {
        run_test(tests[i].name, tests[i].func);
    }
    
    printf("=== End of Test Suite: %s ===\n", suite_name);
}

void print_test_results(void)
{
    printf("\n==================================================\n");
    printf("TEST RESULTS\n");
    printf("==================================================\n");
    printf("Tests run: %d\n", tests_run);
    printf("Tests passed: %d\n", tests_passed);
    printf("Tests failed: %d\n", tests_failed);
    printf("Success rate: %.2f%%\n", tests_run > 0 ? (float)tests_passed / tests_run * 100 : 0);
    printf("==================================================\n");
}

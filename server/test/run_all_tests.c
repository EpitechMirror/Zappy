/*
** EPITECH PROJECT, 2025
** Zappy
** File description:
** Main test runner for all server unit tests
*/

#include "test_framework.h"

// Function prototypes for all test suites
extern void run_parse_tests(void);
extern void run_socket_tests(void);
extern void run_client_management_tests(void);
extern void run_client_data_tests(void);
extern void run_connection_tests(void);
extern void run_accept_tests(void);
extern void run_main_tests(void);

int main(void)
{
    printf("===============================================\n");
    printf("         ZAPPY SERVER UNIT TESTS\n");
    printf("===============================================\n");
    
    // Run all test suites
    run_parse_tests();
    run_socket_tests();
    run_client_management_tests();
    run_client_data_tests();
    run_connection_tests();
    run_accept_tests();
    run_main_tests();
    
    // Print final results
    print_test_results();
    
    // Return appropriate exit code
    return tests_failed > 0 ? 1 : 0;
}

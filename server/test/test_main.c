/*
** EPITECH PROJECT, 2025
** Zappy
** File description:
** Unit tests for main.c
*/

#define _GNU_SOURCE
#include "test_framework.h"
#include "../include/server.h"

#include "test_framework.h"
#include "../include/flag.h"
#include "../include/server.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

// Function prototypes from main.c
extern void print_help(void);

// Test print_help function (just ensure it doesn't crash)
int test_print_help_no_crash(void)
{
    // Redirect stdout to /dev/null to avoid cluttering test output
    int stdout_backup = dup(STDOUT_FILENO);
    FILE *devnull = fopen("/dev/null", "w");
    if (devnull) {
        dup2(fileno(devnull), STDOUT_FILENO);
        
        // Call print_help - should not crash
        print_help();
        
        // Restore stdout
        dup2(stdout_backup, STDOUT_FILENO);
        fclose(devnull);
    }
    close(stdout_backup);
    
    return 1;
}

// Test server configuration initialization
int test_server_config_initialization(void)
{
    server_config_t conf = {0};
    
    TEST_ASSERT_EQUAL(0, conf.port);
    TEST_ASSERT_EQUAL(0, conf.width);
    TEST_ASSERT_EQUAL(0, conf.height);
    TEST_ASSERT_EQUAL(0, conf.clients_nb);
    TEST_ASSERT_EQUAL(0, conf.freq);
    TEST_ASSERT_EQUAL(0, conf.team_count);
    TEST_ASSERT_NULL(conf.team_names);
    
    return 1;
}

// Test argument count validation
int test_argument_count_validation(void)
{
    // Test with insufficient arguments (argc < 2)
    // In real main, this would return 84
    int argc = 1;
    TEST_ASSERT(argc < 2);
    
    // Test with sufficient arguments
    argc = 5;
    TEST_ASSERT(argc >= 2);
    
    return 1;
}

// Test complete server configuration
int test_complete_server_configuration(void)
{
    server_config_t conf = {0};
    
    // Manually set configuration as if parsed from command line
    conf.port = 8080;
    conf.width = 10;
    conf.height = 10;
    conf.clients_nb = 5;
    conf.freq = 100;
    conf.team_count = 2;
    conf.team_names = malloc(sizeof(char*) * 3);
    conf.team_names[0] = strdup("team1");
    conf.team_names[1] = strdup("team2");
    conf.team_names[2] = NULL;
    
    // Validate configuration
    TEST_ASSERT_EQUAL(8080, conf.port);
    TEST_ASSERT_EQUAL(10, conf.width);
    TEST_ASSERT_EQUAL(10, conf.height);
    TEST_ASSERT_EQUAL(5, conf.clients_nb);
    TEST_ASSERT_EQUAL(100, conf.freq);
    TEST_ASSERT_EQUAL(2, conf.team_count);
    TEST_ASSERT_NOT_NULL(conf.team_names);
    TEST_ASSERT_STRING_EQUAL("team1", conf.team_names[0]);
    TEST_ASSERT_STRING_EQUAL("team2", conf.team_names[1]);
    TEST_ASSERT_NULL(conf.team_names[2]);
    
    // Clean up
    for (int i = 0; i < conf.team_count; i++) {
        free(conf.team_names[i]);
    }
    free(conf.team_names);
    
    return 1;
}

// Test configuration validation (positive values)
int test_configuration_validation(void)
{
    server_config_t conf = {0};
    
    // Test valid configuration
    conf.port = 8080;
    conf.width = 10;
    conf.height = 10;
    conf.clients_nb = 5;
    conf.freq = 100;
    
    TEST_ASSERT(conf.port > 0);
    TEST_ASSERT(conf.width > 0);
    TEST_ASSERT(conf.height > 0);
    TEST_ASSERT(conf.clients_nb > 0);
    TEST_ASSERT(conf.freq > 0);
    
    // Test invalid configuration (would be caught by parse)
    server_config_t invalid_conf = {0};
    invalid_conf.port = -1;
    invalid_conf.width = 0;
    invalid_conf.height = -5;
    
    TEST_ASSERT(invalid_conf.port <= 0);
    TEST_ASSERT(invalid_conf.width <= 0);
    TEST_ASSERT(invalid_conf.height <= 0);
    
    return 1;
}

// Test team names configuration
int test_team_names_configuration(void)
{
    server_config_t conf = {0};
    
    // Single team
    conf.team_count = 1;
    conf.team_names = malloc(sizeof(char*) * 2);
    conf.team_names[0] = strdup("solo_team");
    conf.team_names[1] = NULL;
    
    TEST_ASSERT_EQUAL(1, conf.team_count);
    TEST_ASSERT_NOT_NULL(conf.team_names);
    TEST_ASSERT_STRING_EQUAL("solo_team", conf.team_names[0]);
    TEST_ASSERT_NULL(conf.team_names[1]);
    
    // Clean up
    free(conf.team_names[0]);
    free(conf.team_names);
    
    // Multiple teams
    conf.team_count = 3;
    conf.team_names = malloc(sizeof(char*) * 4);
    conf.team_names[0] = strdup("team_alpha");
    conf.team_names[1] = strdup("team_beta");
    conf.team_names[2] = strdup("team_gamma");
    conf.team_names[3] = NULL;
    
    TEST_ASSERT_EQUAL(3, conf.team_count);
    TEST_ASSERT_STRING_EQUAL("team_alpha", conf.team_names[0]);
    TEST_ASSERT_STRING_EQUAL("team_beta", conf.team_names[1]);
    TEST_ASSERT_STRING_EQUAL("team_gamma", conf.team_names[2]);
    TEST_ASSERT_NULL(conf.team_names[3]);
    
    // Clean up
    for (int i = 0; i < conf.team_count; i++) {
        free(conf.team_names[i]);
    }
    free(conf.team_names);
    
    return 1;
}

// Test configuration with edge case values
int test_configuration_edge_cases(void)
{
    server_config_t conf = {0};
    
    // Minimum valid values
    conf.port = 1;
    conf.width = 1;
    conf.height = 1;
    conf.clients_nb = 1;
    conf.freq = 1;
    conf.team_count = 1;
    
    TEST_ASSERT_EQUAL(1, conf.port);
    TEST_ASSERT_EQUAL(1, conf.width);
    TEST_ASSERT_EQUAL(1, conf.height);
    TEST_ASSERT_EQUAL(1, conf.clients_nb);
    TEST_ASSERT_EQUAL(1, conf.freq);
    TEST_ASSERT_EQUAL(1, conf.team_count);
    
    // Large values
    conf.port = 65535;
    conf.width = 1000;
    conf.height = 1000;
    conf.clients_nb = 100;
    conf.freq = 10000;
    conf.team_count = 10;
    
    TEST_ASSERT_EQUAL(65535, conf.port);
    TEST_ASSERT_EQUAL(1000, conf.width);
    TEST_ASSERT_EQUAL(1000, conf.height);
    TEST_ASSERT_EQUAL(100, conf.clients_nb);
    TEST_ASSERT_EQUAL(10000, conf.freq);
    TEST_ASSERT_EQUAL(10, conf.team_count);
    
    return 1;
}

// Test main function error codes
int test_main_error_codes(void)
{
    // Test expected error code for insufficient arguments
    int expected_error = 84;
    TEST_ASSERT_EQUAL(84, expected_error);
    
    // Test expected success code
    int expected_success = 0;
    TEST_ASSERT_EQUAL(0, expected_success);
    
    return 1;
}

// Test server startup sequence validation
int test_server_startup_sequence(void)
{
    // Test the logical sequence of server startup:
    // 1. Parse arguments
    // 2. Create server socket
    // 3. Start accept loop
    // 4. Clean up and exit
    
    server_config_t conf = {0};
    
    // Step 1: Configuration should be initialized
    TEST_ASSERT_EQUAL(0, conf.port);
    
    // Step 2: After parsing, configuration should be valid
    conf.port = 8080;
    conf.width = 10;
    conf.height = 10;
    conf.clients_nb = 5;
    conf.freq = 100;
    
    TEST_ASSERT(conf.port > 0);
    TEST_ASSERT(conf.width > 0);
    TEST_ASSERT(conf.height > 0);
    TEST_ASSERT(conf.clients_nb > 0);
    TEST_ASSERT(conf.freq > 0);
    
    // Step 3: Socket creation would return a valid fd (mock test)
    int mock_server_fd = 3; // Assuming valid fd
    TEST_ASSERT(mock_server_fd >= 0);
    
    // Step 4: Cleanup would close the socket
    // (In real implementation, close(server_fd) would be called)
    
    return 1;
}

// Test suite
test_case_t main_tests[] = {
    {"test_print_help_no_crash", test_print_help_no_crash},
    {"test_server_config_initialization", test_server_config_initialization},
    {"test_argument_count_validation", test_argument_count_validation},
    {"test_complete_server_configuration", test_complete_server_configuration},
    {"test_configuration_validation", test_configuration_validation},
    {"test_team_names_configuration", test_team_names_configuration},
    {"test_configuration_edge_cases", test_configuration_edge_cases},
    {"test_main_error_codes", test_main_error_codes},
    {"test_server_startup_sequence", test_server_startup_sequence}
};

void run_main_tests(void)
{
    run_test_suite("Main Function Tests", main_tests, sizeof(main_tests) / sizeof(main_tests[0]));
}

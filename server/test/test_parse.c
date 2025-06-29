/*
** EPITECH PROJECT, 2025
** Zappy
** File description:
** Unit tests for parse.c
*/

#include "test_framework.h"
#include "../include/flag.h"
#include <stdlib.h>
#include <string.h>

// Function prototypes from parse.c
extern int is_number(const char *str);
extern int handle_int(int *field, int *i, char **argv);
extern int handle_teams(server_config_t *conf, int *i, char **argv);
extern int strcmp_flag(char **argv, server_config_t *conf, int *i, flag_entry_t *flag_table);

// Test is_number function
int test_is_number_valid(void)
{
    TEST_ASSERT(is_number("123") == 1);
    TEST_ASSERT(is_number("0") == 1);
    TEST_ASSERT(is_number("999") == 1);
    return 1;
}

int test_is_number_invalid(void)
{
    TEST_ASSERT(is_number("abc") == 0);
    TEST_ASSERT(is_number("-123") == 0);
    TEST_ASSERT(is_number("12.3") == 0);
    TEST_ASSERT(is_number("") == 0);
    TEST_ASSERT(is_number(NULL) == 0);
    TEST_ASSERT(is_number("123abc") == 0);
    return 1;
}

// Test handle_int function
int test_handle_int_valid(void)
{
    char *argv[] = {"program", "-p", "8080", NULL};
    int field = 0;
    int i = 1;
    int result = handle_int(&field, &i, argv);
    
    TEST_ASSERT_EQUAL(0, result);
    TEST_ASSERT_EQUAL(8080, field);
    TEST_ASSERT_EQUAL(2, i);
    return 1;
}

int test_handle_int_missing_value(void)
{
    char *argv[] = {"program", "-p", NULL};
    int field = 0;
    int i = 1;
    int result = handle_int(&field, &i, argv);
    
    TEST_ASSERT_EQUAL(84, result);
    return 1;
}

int test_handle_int_invalid_value(void)
{
    char *argv[] = {"program", "-p", "abc", NULL};
    int field = 0;
    int i = 1;
    int result = handle_int(&field, &i, argv);
    
    TEST_ASSERT_EQUAL(84, result);
    return 1;
}

int test_handle_int_negative_value(void)
{
    char *argv[] = {"program", "-p", "-123", NULL};
    int field = 0;
    int i = 1;
    int result = handle_int(&field, &i, argv);
    
    TEST_ASSERT_EQUAL(84, result);
    return 1;
}

int test_handle_int_zero_value(void)
{
    char *argv[] = {"program", "-p", "0", NULL};
    int field = 0;
    int i = 1;
    int result = handle_int(&field, &i, argv);
    
    TEST_ASSERT_EQUAL(84, result);
    return 1;
}

// Test handle_teams function
int test_handle_teams_valid(void)
{
    char *argv[] = {"program", "-n", "team1", "team2", "team3", "-p", "8080", NULL};
    server_config_t conf = {0};
    int i = 1;
    int result = handle_teams(&conf, &i, argv);
    
    TEST_ASSERT_EQUAL(0, result);
    TEST_ASSERT_EQUAL(3, conf.team_count);
    TEST_ASSERT_NOT_NULL(conf.team_names);
    TEST_ASSERT_STRING_EQUAL("team1", conf.team_names[0]);
    TEST_ASSERT_STRING_EQUAL("team2", conf.team_names[1]);
    TEST_ASSERT_STRING_EQUAL("team3", conf.team_names[2]);
    TEST_ASSERT_NULL(conf.team_names[3]);
    
    // Clean up
    for (int j = 0; j < conf.team_count; j++) {
        free(conf.team_names[j]);
    }
    free(conf.team_names);
    return 1;
}

int test_handle_teams_single_team(void)
{
    char *argv[] = {"program", "-n", "solo_team", "-p", "8080", NULL};
    server_config_t conf = {0};
    int i = 1;
    int result = handle_teams(&conf, &i, argv);
    
    TEST_ASSERT_EQUAL(0, result);
    TEST_ASSERT_EQUAL(1, conf.team_count);
    TEST_ASSERT_NOT_NULL(conf.team_names);
    TEST_ASSERT_STRING_EQUAL("solo_team", conf.team_names[0]);
    TEST_ASSERT_NULL(conf.team_names[1]);
    
    // Clean up
    for (int j = 0; j < conf.team_count; j++) {
        free(conf.team_names[j]);
    }
    free(conf.team_names);
    return 1;
}

int test_handle_teams_no_teams(void)
{
    char *argv[] = {"program", "-n", "-p", "8080", NULL};
    server_config_t conf = {0};
    int i = 1;
    int result = handle_teams(&conf, &i, argv);
    
    TEST_ASSERT_EQUAL(0, result);
    TEST_ASSERT_EQUAL(0, conf.team_count);
    TEST_ASSERT_NOT_NULL(conf.team_names);
    TEST_ASSERT_NULL(conf.team_names[0]);
    
    // Clean up
    free(conf.team_names);
    return 1;
}

// Test parse function with full command line
int test_parse_valid_complete(void)
{
    char *argv[] = {"program", "-p", "8080", "-x", "10", "-y", "10", 
                   "-n", "team1", "team2", "-c", "5", "-f", "100", NULL};
    server_config_t conf = {0};
    int result = parse(13, argv, &conf);
    
    TEST_ASSERT_EQUAL(0, result);
    TEST_ASSERT_EQUAL(8080, conf.port);
    TEST_ASSERT_EQUAL(10, conf.width);
    TEST_ASSERT_EQUAL(10, conf.height);
    TEST_ASSERT_EQUAL(5, conf.clients_nb);
    TEST_ASSERT_EQUAL(100, conf.freq);
    TEST_ASSERT_EQUAL(2, conf.team_count);
    TEST_ASSERT_STRING_EQUAL("team1", conf.team_names[0]);
    TEST_ASSERT_STRING_EQUAL("team2", conf.team_names[1]);
    
    // Clean up
    for (int j = 0; j < conf.team_count; j++) {
        free(conf.team_names[j]);
    }
    free(conf.team_names);
    return 1;
}

int test_parse_invalid_flag(void)
{
    char *argv[] = {"program", "-z", "invalid", NULL};
    server_config_t conf = {0};
    int result = parse(3, argv, &conf);
    
    TEST_ASSERT_EQUAL(84, result);
    return 1;
}

int test_parse_missing_port_value(void)
{
    char *argv[] = {"program", "-p", NULL};
    server_config_t conf = {0};
    int result = parse(2, argv, &conf);
    
    TEST_ASSERT_EQUAL(84, result);
    return 1;
}

// Test suite
test_case_t parse_tests[] = {
    {"test_is_number_valid", test_is_number_valid},
    {"test_is_number_invalid", test_is_number_invalid},
    {"test_handle_int_valid", test_handle_int_valid},
    {"test_handle_int_missing_value", test_handle_int_missing_value},
    {"test_handle_int_invalid_value", test_handle_int_invalid_value},
    {"test_handle_int_negative_value", test_handle_int_negative_value},
    {"test_handle_int_zero_value", test_handle_int_zero_value},
    {"test_handle_teams_valid", test_handle_teams_valid},
    {"test_handle_teams_single_team", test_handle_teams_single_team},
    {"test_handle_teams_no_teams", test_handle_teams_no_teams},
    {"test_parse_valid_complete", test_parse_valid_complete},
    {"test_parse_invalid_flag", test_parse_invalid_flag},
    {"test_parse_missing_port_value", test_parse_missing_port_value}
};

void run_parse_tests(void)
{
    run_test_suite("Parse Functions", parse_tests, sizeof(parse_tests) / sizeof(parse_tests[0]));
}

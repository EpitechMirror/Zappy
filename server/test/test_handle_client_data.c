/*
** EPITECH PROJECT, 2025
** Zappy
** File description:
** Unit tests for handle_client_data.c
*/

#include "test_framework.h"
#include "../include/server.h"
#include "../include/Client_Info.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>

// Function prototypes from handle_client_data.c
extern bool handle_client_data(client_t **clients, int fd, server_config_t *conf);
extern bool handle_auth(client_t **clients, client_t *client, int fd, server_config_t *conf, char *buffer);

// Helper function to create a test client
client_t *create_test_client_for_data(int fd, const char *team_name, client_state_t state)
{
    client_t *client = malloc(sizeof(client_t));
    if (!client) return NULL;
    
    client->fd = fd;
    client->connected = true;
    client->is_ready = false;
    client->is_alive = true;
    client->x = 0;
    client->y = 0;
    client->id = fd;
    client->team_name = team_name ? strdup(team_name) : NULL;
    client->is_graphic = false;
    client->state = state;
    client->next = NULL;
    
    return client;
}

// Helper function to create test server config
server_config_t create_test_config(void)
{
    server_config_t conf = {0};
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
    return conf;
}

// Helper function to clean up server config
void cleanup_test_config(server_config_t *conf)
{
    if (conf->team_names) {
        for (int i = 0; i < conf->team_count; i++) {
            free(conf->team_names[i]);
        }
        free(conf->team_names);
    }
}

// Test handle_auth with valid team name
int test_handle_auth_valid_team(void)
{
    client_t *client = create_test_client_for_data(1, NULL, WAITING_NAME);
    client_t *clients = client;
    server_config_t conf = create_test_config();
    
    char buffer[] = "team1\n";
    bool result = handle_auth(&clients, client, 1, &conf, buffer);
    
    TEST_ASSERT(result == true);
    TEST_ASSERT_EQUAL(AUTHENTICATED, client->state);
    TEST_ASSERT_STRING_EQUAL("team1", client->team_name);
    TEST_ASSERT(client->is_graphic == false);
    
    // Clean up
    free(client->team_name);
    free(client);
    cleanup_test_config(&conf);
    return 1;
}

// Test handle_auth with GRAPHIC team
int test_handle_auth_graphic_team(void)
{
    client_t *client = create_test_client_for_data(1, NULL, WAITING_NAME);
    client_t *clients = client;
    server_config_t conf = create_test_config();
    
    char buffer[] = "GRAPHIC\n";
    bool result = handle_auth(&clients, client, 1, &conf, buffer);
    
    TEST_ASSERT(result == true);
    TEST_ASSERT_EQUAL(AUTHENTICATED, client->state);
    TEST_ASSERT_STRING_EQUAL("GRAPHIC", client->team_name);
    TEST_ASSERT(client->is_graphic == true);
    
    // Clean up
    free(client->team_name);
    free(client);
    cleanup_test_config(&conf);
    return 1;
}

// Test handle_auth with team name containing carriage return
int test_handle_auth_with_carriage_return(void)
{
    client_t *client = create_test_client_for_data(1, NULL, WAITING_NAME);
    client_t *clients = client;
    server_config_t conf = create_test_config();
    
    char buffer[] = "team1\r\n";
    bool result = handle_auth(&clients, client, 1, &conf, buffer);
    
    TEST_ASSERT(result == true);
    TEST_ASSERT_EQUAL(AUTHENTICATED, client->state);
    TEST_ASSERT_STRING_EQUAL("team1", client->team_name);
    
    // Clean up
    free(client->team_name);
    free(client);
    cleanup_test_config(&conf);
    return 1;
}

// Test handle_auth with empty team name
int test_handle_auth_empty_team(void)
{
    client_t *client = create_test_client_for_data(1, NULL, WAITING_NAME);
    client_t *clients = client;
    server_config_t conf = create_test_config();
    
    char buffer[] = "\n";
    bool result = handle_auth(&clients, client, 1, &conf, buffer);
    
    TEST_ASSERT(result == true);
    TEST_ASSERT_EQUAL(AUTHENTICATED, client->state);
    TEST_ASSERT_STRING_EQUAL("", client->team_name);
    
    // Clean up
    free(client->team_name);
    free(client);
    cleanup_test_config(&conf);
    return 1;
}

// Test handle_auth with unknown team name
int test_handle_auth_unknown_team(void)
{
    client_t *client = create_test_client_for_data(1, NULL, WAITING_NAME);
    client_t *clients = client;
    server_config_t conf = create_test_config();
    
    char buffer[] = "unknown_team\n";
    bool result = handle_auth(&clients, client, 1, &conf, buffer);
    
    TEST_ASSERT(result == true);
    TEST_ASSERT_EQUAL(AUTHENTICATED, client->state);
    TEST_ASSERT_STRING_EQUAL("unknown_team", client->team_name);
    
    // Clean up
    free(client->team_name);
    free(client);
    cleanup_test_config(&conf);
    return 1;
}

// Test handle_client_data with non-existent client
int test_handle_client_data_non_existent_client(void)
{
    client_t *clients = create_test_client_for_data(1, "team1", AUTHENTICATED);
    server_config_t conf = create_test_config();
    
    // Try to handle data for non-existent client fd 99
    bool result = handle_client_data(&clients, 99, &conf);
    
    TEST_ASSERT(result == true); // Should return true (disconnect)
    
    // Clean up
    free(clients->team_name);
    free(clients);
    cleanup_test_config(&conf);
    return 1;
}

// Test client state transitions
int test_client_state_waiting_to_authenticated(void)
{
    client_t *client = create_test_client_for_data(1, NULL, WAITING_NAME);
    TEST_ASSERT_EQUAL(WAITING_NAME, client->state);
    
    // Simulate authentication
    client->state = AUTHENTICATED;
    client->team_name = strdup("team1");
    
    TEST_ASSERT_EQUAL(AUTHENTICATED, client->state);
    TEST_ASSERT_STRING_EQUAL("team1", client->team_name);
    
    // Clean up
    free(client->team_name);
    free(client);
    return 1;
}

// Test client creation with different states
int test_client_creation_states(void)
{
    client_t *waiting_client = create_test_client_for_data(1, NULL, WAITING_NAME);
    client_t *auth_client = create_test_client_for_data(2, "team1", AUTHENTICATED);
    
    TEST_ASSERT_EQUAL(WAITING_NAME, waiting_client->state);
    TEST_ASSERT_NULL(waiting_client->team_name);
    
    TEST_ASSERT_EQUAL(AUTHENTICATED, auth_client->state);
    TEST_ASSERT_STRING_EQUAL("team1", auth_client->team_name);
    
    // Clean up
    free(waiting_client);
    free(auth_client->team_name);
    free(auth_client);
    return 1;
}

// Test buffer handling edge cases
int test_buffer_handling_long_team_name(void)
{
    client_t *client = create_test_client_for_data(1, NULL, WAITING_NAME);
    client_t *clients = client;
    server_config_t conf = create_test_config();
    
    // Very long team name
    char buffer[256];
    memset(buffer, 'A', 250);
    buffer[250] = '\n';
    buffer[251] = '\0';
    
    bool result = handle_auth(&clients, client, 1, &conf, buffer);
    
    TEST_ASSERT(result == true);
    TEST_ASSERT_EQUAL(AUTHENTICATED, client->state);
    TEST_ASSERT_NOT_NULL(client->team_name);
    
    // Clean up
    free(client->team_name);
    free(client);
    cleanup_test_config(&conf);
    return 1;
}

// Test suite
test_case_t client_data_tests[] = {
    {"test_handle_auth_valid_team", test_handle_auth_valid_team},
    {"test_handle_auth_graphic_team", test_handle_auth_graphic_team},
    {"test_handle_auth_with_carriage_return", test_handle_auth_with_carriage_return},
    {"test_handle_auth_empty_team", test_handle_auth_empty_team},
    {"test_handle_auth_unknown_team", test_handle_auth_unknown_team},
    {"test_handle_client_data_non_existent_client", test_handle_client_data_non_existent_client},
    {"test_client_state_waiting_to_authenticated", test_client_state_waiting_to_authenticated},
    {"test_client_creation_states", test_client_creation_states},
    {"test_buffer_handling_long_team_name", test_buffer_handling_long_team_name}
};

void run_client_data_tests(void)
{
    run_test_suite("Client Data Handling Functions", client_data_tests, sizeof(client_data_tests) / sizeof(client_data_tests[0]));
}

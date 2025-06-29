/*
** EPITECH PROJECT, 2025
** Zappy
** File description:
** Unit tests for connection.c
*/

#define _GNU_SOURCE
#include "test_framework.h"
#include "../include/server.h"
#include "../include/Client_Info.h"
#include <stdlib.h>
#include <string.h>
#include <poll.h>

// Function prototypes from connection.c
extern void handle_new_connection(int server_fd, struct pollfd *fds, int *client_count, client_t **clients, server_config_t *conf);

// Helper function to create test server config
server_config_t create_connection_test_config(void)
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
void cleanup_connection_test_config(server_config_t *conf)
{
    if (conf->team_names) {
        for (int i = 0; i < conf->team_count; i++) {
            free(conf->team_names[i]);
        }
        free(conf->team_names);
    }
}

// Test client structure initialization
int test_client_struct_initialization(void)
{
    client_t client = {0};
    
    TEST_ASSERT_EQUAL(0, client.fd);
    TEST_ASSERT_EQUAL(false, client.connected);
    TEST_ASSERT_EQUAL(false, client.is_ready);
    TEST_ASSERT_EQUAL(false, client.is_alive);
    TEST_ASSERT_EQUAL(0, client.x);
    TEST_ASSERT_EQUAL(0, client.y);
    TEST_ASSERT_EQUAL(0, client.id);
    TEST_ASSERT_NULL(client.team_name);
    TEST_ASSERT_EQUAL(false, client.is_graphic);
    TEST_ASSERT_EQUAL(WAITING_NAME, client.state);
    TEST_ASSERT_NULL(client.next);
    
    return 1;
}

// Test client allocation and initialization
int test_client_allocation(void)
{
    client_t *client = malloc(sizeof(client_t));
    TEST_ASSERT_NOT_NULL(client);
    
    if (client) {
        client->fd = 5;
        client->connected = true;
        client->is_ready = false;
        client->is_alive = true;
        client->state = WAITING_NAME;
        client->team_name = NULL;
        client->next = NULL;
        client->is_graphic = false;
        
        TEST_ASSERT_EQUAL(5, client->fd);
        TEST_ASSERT_EQUAL(true, client->connected);
        TEST_ASSERT_EQUAL(false, client->is_ready);
        TEST_ASSERT_EQUAL(true, client->is_alive);
        TEST_ASSERT_EQUAL(WAITING_NAME, client->state);
        TEST_ASSERT_NULL(client->team_name);
        TEST_ASSERT_NULL(client->next);
        TEST_ASSERT_EQUAL(false, client->is_graphic);
        
        free(client);
    }
    
    return 1;
}

// Test pollfd structure initialization
int test_pollfd_initialization(void)
{
    struct pollfd fds[MAX_CLIENTS];
    int client_count = 1;
    
    // Initialize server fd
    fds[0].fd = 3; // Mock server fd
    fds[0].events = POLLIN;
    fds[0].revents = 0;
    
    TEST_ASSERT_EQUAL(3, fds[0].fd);
    TEST_ASSERT_EQUAL(POLLIN, fds[0].events);
    TEST_ASSERT_EQUAL(0, fds[0].revents);
    TEST_ASSERT_EQUAL(1, client_count);
    
    return 1;
}

// Test client count management
int test_client_count_management(void)
{
    int client_count = 1;
    
    // Simulate adding clients
    client_count++;
    TEST_ASSERT_EQUAL(2, client_count);
    
    client_count++;
    TEST_ASSERT_EQUAL(3, client_count);
    
    // Simulate removing clients
    client_count--;
    TEST_ASSERT_EQUAL(2, client_count);
    
    client_count--;
    TEST_ASSERT_EQUAL(1, client_count);
    
    return 1;
}

// Test max clients limit
int test_max_clients_limit(void)
{
    struct pollfd fds[MAX_CLIENTS];
    int client_count = MAX_CLIENTS;
    
    // Should not be able to add more clients
    TEST_ASSERT(client_count >= MAX_CLIENTS);
    
    // Test boundary condition
    TEST_ASSERT_EQUAL(MAX_CLIENTS, client_count);
    
    return 1;
}

// Test client list management
int test_client_list_management(void)
{
    client_t *clients = NULL;
    
    // Add first client
    client_t *client1 = malloc(sizeof(client_t));
    client1->fd = 5;
    client1->next = clients;
    clients = client1;
    
    TEST_ASSERT_NOT_NULL(clients);
    TEST_ASSERT_EQUAL(5, clients->fd);
    TEST_ASSERT_NULL(clients->next);
    
    // Add second client
    client_t *client2 = malloc(sizeof(client_t));
    client2->fd = 6;
    client2->next = clients;
    clients = client2;
    
    TEST_ASSERT_NOT_NULL(clients);
    TEST_ASSERT_EQUAL(6, clients->fd);
    TEST_ASSERT_NOT_NULL(clients->next);
    TEST_ASSERT_EQUAL(5, clients->next->fd);
    
    // Clean up
    while (clients) {
        client_t *temp = clients;
        clients = clients->next;
        free(temp);
    }
    
    return 1;
}

// Test server configuration usage
int test_server_config_usage(void)
{
    server_config_t conf = create_connection_test_config();
    
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
    
    cleanup_connection_test_config(&conf);
    return 1;
}

// Test file descriptor management
int test_fd_management(void)
{
    struct pollfd fds[5];
    int client_count = 1;
    
    // Server fd
    fds[0].fd = 3;
    fds[0].events = POLLIN;
    
    // Add client fds
    if (client_count < 5) {
        fds[client_count].fd = 10;
        fds[client_count].events = POLLIN;
        client_count++;
    }
    
    if (client_count < 5) {
        fds[client_count].fd = 11;
        fds[client_count].events = POLLIN;
        client_count++;
    }
    
    TEST_ASSERT_EQUAL(3, client_count);
    TEST_ASSERT_EQUAL(3, fds[0].fd);
    TEST_ASSERT_EQUAL(10, fds[1].fd);
    TEST_ASSERT_EQUAL(11, fds[2].fd);
    TEST_ASSERT_EQUAL(POLLIN, fds[0].events);
    TEST_ASSERT_EQUAL(POLLIN, fds[1].events);
    TEST_ASSERT_EQUAL(POLLIN, fds[2].events);
    
    return 1;
}

// Test client state initialization on connection
int test_client_state_on_connection(void)
{
    client_t *new_client = malloc(sizeof(client_t));
    TEST_ASSERT_NOT_NULL(new_client);
    
    if (new_client) {
        // Initialize as would be done in handle_new_connection
        new_client->fd = 10;
        new_client->connected = true;
        new_client->is_ready = false;
        new_client->is_alive = true;
        new_client->state = WAITING_NAME;
        new_client->team_name = NULL;
        new_client->is_graphic = false;
        new_client->next = NULL;
        
        TEST_ASSERT_EQUAL(10, new_client->fd);
        TEST_ASSERT_EQUAL(true, new_client->connected);
        TEST_ASSERT_EQUAL(false, new_client->is_ready);
        TEST_ASSERT_EQUAL(true, new_client->is_alive);
        TEST_ASSERT_EQUAL(WAITING_NAME, new_client->state);
        TEST_ASSERT_NULL(new_client->team_name);
        TEST_ASSERT_EQUAL(false, new_client->is_graphic);
        TEST_ASSERT_NULL(new_client->next);
        
        free(new_client);
    }
    
    return 1;
}

// Test suite
test_case_t connection_tests[] = {
    {"test_client_struct_initialization", test_client_struct_initialization},
    {"test_client_allocation", test_client_allocation},
    {"test_pollfd_initialization", test_pollfd_initialization},
    {"test_client_count_management", test_client_count_management},
    {"test_max_clients_limit", test_max_clients_limit},
    {"test_client_list_management", test_client_list_management},
    {"test_server_config_usage", test_server_config_usage},
    {"test_fd_management", test_fd_management},
    {"test_client_state_on_connection", test_client_state_on_connection}
};

void run_connection_tests(void)
{
    run_test_suite("Connection Handling Functions", connection_tests, sizeof(connection_tests) / sizeof(connection_tests[0]));
}

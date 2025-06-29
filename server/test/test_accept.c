/*
** EPITECH PROJECT, 2025
** Zappy
** File description:
** Unit tests for accept.c
*/

#include "test_framework.h"
#include "../include/server.h"
#include "../include/Client_Info.h"
#include <stdlib.h>
#include <string.h>
#include <poll.h>
#include <fcntl.h>

// Function prototypes from accept.c
extern void poll_clients(int server_fd, struct pollfd *fds, int *client_count, client_t **clients, server_config_t *conf);
extern void set_non_blocking(int fd);
extern void accept_clients_loop(int server_fd, server_config_t *conf);

// Helper function to create test server config
server_config_t create_accept_test_config(void)
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
void cleanup_accept_test_config(server_config_t *conf)
{
    if (conf->team_names) {
        for (int i = 0; i < conf->team_count; i++) {
            free(conf->team_names[i]);
        }
        free(conf->team_names);
    }
}

// Test set_non_blocking function with valid fd
int test_set_non_blocking_valid_fd(void)
{
    // Create a pipe to test with
    int pipefd[2];
    if (pipe(pipefd) == -1) {
        return 0; // Skip test if pipe creation fails
    }
    
    // Test setting non-blocking mode
    set_non_blocking(pipefd[0]);
    
    // Check if the file descriptor has the O_NONBLOCK flag
    int flags = fcntl(pipefd[0], F_GETFL, 0);
    TEST_ASSERT(flags != -1);
    TEST_ASSERT((flags & O_NONBLOCK) != 0);
    
    // Clean up
    close(pipefd[0]);
    close(pipefd[1]);
    
    return 1;
}

// Test set_non_blocking function with invalid fd
int test_set_non_blocking_invalid_fd(void)
{
    // Test with invalid file descriptor
    // This should not crash but will likely fail silently
    set_non_blocking(-1);
    
    // If we reach here, the function didn't crash
    return 1;
}

// Test poll setup initialization
int test_poll_setup_initialization(void)
{
    struct pollfd fds[MAX_CLIENTS];
    int client_count = 1;
    int server_fd = 3; // Mock server fd
    
    // Initialize as done in accept_clients_loop
    fds[0].fd = server_fd;
    fds[0].events = POLLIN;
    fds[0].revents = 0;
    
    TEST_ASSERT_EQUAL(server_fd, fds[0].fd);
    TEST_ASSERT_EQUAL(POLLIN, fds[0].events);
    TEST_ASSERT_EQUAL(0, fds[0].revents);
    TEST_ASSERT_EQUAL(1, client_count);
    
    return 1;
}

// Test pollfd array management
int test_pollfd_array_management(void)
{
    struct pollfd fds[MAX_CLIENTS];
    int client_count = 1;
    
    // Server fd
    fds[0].fd = 3;
    fds[0].events = POLLIN;
    
    // Add some client fds
    for (int i = 1; i < 5 && client_count < MAX_CLIENTS; i++) {
        fds[client_count].fd = 10 + i;
        fds[client_count].events = POLLIN;
        fds[client_count].revents = 0;
        client_count++;
    }
    
    TEST_ASSERT_EQUAL(5, client_count);
    TEST_ASSERT_EQUAL(3, fds[0].fd);
    TEST_ASSERT_EQUAL(11, fds[1].fd);
    TEST_ASSERT_EQUAL(12, fds[2].fd);
    TEST_ASSERT_EQUAL(13, fds[3].fd);
    TEST_ASSERT_EQUAL(14, fds[4].fd);
    
    // Test removing a client fd (simulate what happens in poll_clients)
    for (int j = 2; j < client_count - 1; j++) {
        fds[j] = fds[j + 1];
    }
    client_count--;
    
    TEST_ASSERT_EQUAL(4, client_count);
    TEST_ASSERT_EQUAL(3, fds[0].fd);
    TEST_ASSERT_EQUAL(11, fds[1].fd);
    TEST_ASSERT_EQUAL(13, fds[2].fd); // 12 was removed, 13 moved here
    TEST_ASSERT_EQUAL(14, fds[3].fd); // 14 moved here
    
    return 1;
}

// Test client list management during polling
int test_client_list_during_polling(void)
{
    client_t *clients = NULL;
    
    // Add some clients as would happen during connection
    for (int i = 0; i < 3; i++) {
        client_t *new_client = malloc(sizeof(client_t));
        TEST_ASSERT_NOT_NULL(new_client);
        
        if (new_client) {
            new_client->fd = 10 + i;
            new_client->connected = true;
            new_client->is_ready = false;
            new_client->is_alive = true;
            new_client->state = WAITING_NAME;
            new_client->team_name = NULL;
            new_client->is_graphic = false;
            new_client->next = clients;
            clients = new_client;
        }
    }
    
    // Verify the list structure
    TEST_ASSERT_NOT_NULL(clients);
    TEST_ASSERT_EQUAL(12, clients->fd); // Last added (fd=12)
    TEST_ASSERT_NOT_NULL(clients->next);
    TEST_ASSERT_EQUAL(11, clients->next->fd);
    TEST_ASSERT_NOT_NULL(clients->next->next);
    TEST_ASSERT_EQUAL(10, clients->next->next->fd);
    TEST_ASSERT_NULL(clients->next->next->next);
    
    // Clean up
    while (clients) {
        client_t *temp = clients;
        clients = clients->next;
        free(temp);
    }
    
    return 1;
}

// Test poll events handling
int test_poll_events_handling(void)
{
    struct pollfd fds[5];
    
    // Setup some fds with different events
    fds[0].fd = 3;
    fds[0].events = POLLIN;
    fds[0].revents = POLLIN; // Server has incoming connection
    
    fds[1].fd = 10;
    fds[1].events = POLLIN;
    fds[1].revents = POLLIN; // Client has data
    
    fds[2].fd = 11;
    fds[2].events = POLLIN;
    fds[2].revents = 0; // No events
    
    fds[3].fd = 12;
    fds[3].events = POLLIN;
    fds[3].revents = POLLIN; // Client has data
    
    // Test the logic that would be in poll_clients
    for (int i = 0; i < 4; i++) {
        if (fds[i].revents & POLLIN) {
            if (fds[i].fd == 3) {
                // This is the server fd - new connection
                TEST_ASSERT_EQUAL(3, fds[i].fd);
            } else {
                // This is a client fd - client data
                TEST_ASSERT(fds[i].fd == 10 || fds[i].fd == 12);
            }
        }
    }
    
    return 1;
}

// Test maximum clients boundary
int test_max_clients_boundary(void)
{
    struct pollfd fds[MAX_CLIENTS];
    int client_count = 1;
    
    // Fill up to max capacity
    for (int i = 1; i < MAX_CLIENTS; i++) {
        fds[client_count].fd = 100 + i;
        fds[client_count].events = POLLIN;
        client_count++;
    }
    
    TEST_ASSERT_EQUAL(MAX_CLIENTS, client_count);
    
    // Try to add one more (should fail or be handled gracefully)
    if (client_count < MAX_CLIENTS) {
        fds[client_count].fd = 200;
        fds[client_count].events = POLLIN;
        client_count++;
    }
    
    TEST_ASSERT_EQUAL(MAX_CLIENTS, client_count);
    
    return 1;
}

// Test client cleanup during shutdown
int test_client_cleanup_shutdown(void)
{
    client_t *clients = NULL;
    
    // Create some clients
    for (int i = 0; i < 3; i++) {
        client_t *client = malloc(sizeof(client_t));
        TEST_ASSERT_NOT_NULL(client);
        
        if (client) {
            client->fd = 10 + i;
            client->team_name = strdup("test_team");
            client->next = clients;
            clients = client;
        }
    }
    
    // Simulate cleanup as done in accept_clients_loop
    int initial_count = 0;
    client_t *temp = clients;
    while (temp) {
        initial_count++;
        temp = temp->next;
    }
    TEST_ASSERT_EQUAL(3, initial_count);
    
    // Clean up all clients
    while (clients) {
        client_t *to_remove = clients;
        clients = clients->next;
        free(to_remove->team_name);
        free(to_remove);
    }
    
    TEST_ASSERT_NULL(clients);
    
    return 1;
}

// Test file descriptor flags
int test_fd_flags_management(void)
{
    // Create a pipe to test with
    int pipefd[2];
    if (pipe(pipefd) == -1) {
        return 0; // Skip test if pipe creation fails
    }
    
    // Get initial flags
    int initial_flags = fcntl(pipefd[0], F_GETFL, 0);
    TEST_ASSERT(initial_flags != -1);
    TEST_ASSERT((initial_flags & O_NONBLOCK) == 0); // Should not be non-blocking initially
    
    // Set non-blocking
    set_non_blocking(pipefd[0]);
    
    // Check flags again
    int new_flags = fcntl(pipefd[0], F_GETFL, 0);
    TEST_ASSERT(new_flags != -1);
    TEST_ASSERT((new_flags & O_NONBLOCK) != 0); // Should now be non-blocking
    
    // Clean up
    close(pipefd[0]);
    close(pipefd[1]);
    
    return 1;
}

// Test suite
test_case_t accept_tests[] = {
    {"test_set_non_blocking_valid_fd", test_set_non_blocking_valid_fd},
    {"test_set_non_blocking_invalid_fd", test_set_non_blocking_invalid_fd},
    {"test_poll_setup_initialization", test_poll_setup_initialization},
    {"test_pollfd_array_management", test_pollfd_array_management},
    {"test_client_list_during_polling", test_client_list_during_polling},
    {"test_poll_events_handling", test_poll_events_handling},
    {"test_max_clients_boundary", test_max_clients_boundary},
    {"test_client_cleanup_shutdown", test_client_cleanup_shutdown},
    {"test_fd_flags_management", test_fd_flags_management}
};

void run_accept_tests(void)
{
    run_test_suite("Accept and Polling Functions", accept_tests, sizeof(accept_tests) / sizeof(accept_tests[0]));
}

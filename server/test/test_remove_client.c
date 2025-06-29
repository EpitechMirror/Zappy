/*
** EPITECH PROJECT, 2025
** Zappy
** File description:
** Unit tests for remove_client.c
*/

#define _GNU_SOURCE
#include "test_framework.h"
#include "../include/server.h"
#include "../include/Client_Info.h"
#include <stdlib.h>
#include <string.h>
#include <poll.h>

// Function prototypes from remove_client.c
extern void remove_client(client_t **clients, int fd);
extern int remove_client_fd(struct pollfd *fds, int count, int idx);
extern int add_client_fd(struct pollfd *fds, int client_fd, int count);

// Helper function to create a test client
client_t *create_test_client(int fd, const char *team_name)
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
    client->state = WAITING_NAME;
    client->next = NULL;
    
    return client;
}

// Test remove_client with single client
int test_remove_client_single(void)
{
    client_t *clients = create_test_client(1, "team1");
    TEST_ASSERT_NOT_NULL(clients);
    
    remove_client(&clients, 1);
    TEST_ASSERT_NULL(clients);
    
    return 1;
}

// Test remove_client with multiple clients (remove first)
int test_remove_client_first_of_multiple(void)
{
    client_t *clients = create_test_client(1, "team1");
    clients->next = create_test_client(2, "team2");
    clients->next->next = create_test_client(3, "team3");
    
    remove_client(&clients, 1);
    
    TEST_ASSERT_NOT_NULL(clients);
    TEST_ASSERT_EQUAL(2, clients->fd);
    TEST_ASSERT_NOT_NULL(clients->next);
    TEST_ASSERT_EQUAL(3, clients->next->fd);
    
    // Clean up remaining clients
    remove_client(&clients, 2);
    remove_client(&clients, 3);
    
    return 1;
}

// Test remove_client with multiple clients (remove middle)
int test_remove_client_middle_of_multiple(void)
{
    client_t *clients = create_test_client(1, "team1");
    clients->next = create_test_client(2, "team2");
    clients->next->next = create_test_client(3, "team3");
    
    remove_client(&clients, 2);
    
    TEST_ASSERT_NOT_NULL(clients);
    TEST_ASSERT_EQUAL(1, clients->fd);
    TEST_ASSERT_NOT_NULL(clients->next);
    TEST_ASSERT_EQUAL(3, clients->next->fd);
    TEST_ASSERT_NULL(clients->next->next);
    
    // Clean up remaining clients
    remove_client(&clients, 1);
    remove_client(&clients, 3);
    
    return 1;
}

// Test remove_client with multiple clients (remove last)
int test_remove_client_last_of_multiple(void)
{
    client_t *clients = create_test_client(1, "team1");
    clients->next = create_test_client(2, "team2");
    clients->next->next = create_test_client(3, "team3");
    
    remove_client(&clients, 3);
    
    TEST_ASSERT_NOT_NULL(clients);
    TEST_ASSERT_EQUAL(1, clients->fd);
    TEST_ASSERT_NOT_NULL(clients->next);
    TEST_ASSERT_EQUAL(2, clients->next->fd);
    TEST_ASSERT_NULL(clients->next->next);
    
    // Clean up remaining clients
    remove_client(&clients, 1);
    remove_client(&clients, 2);
    
    return 1;
}

// Test remove_client with non-existent fd
int test_remove_client_non_existent(void)
{
    client_t *clients = create_test_client(1, "team1");
    clients->next = create_test_client(2, "team2");
    
    // Try to remove non-existent client
    remove_client(&clients, 99);
    
    // List should remain unchanged
    TEST_ASSERT_NOT_NULL(clients);
    TEST_ASSERT_EQUAL(1, clients->fd);
    TEST_ASSERT_NOT_NULL(clients->next);
    TEST_ASSERT_EQUAL(2, clients->next->fd);
    
    // Clean up
    remove_client(&clients, 1);
    remove_client(&clients, 2);
    
    return 1;
}

// Test remove_client with empty list
int test_remove_client_empty_list(void)
{
    client_t *clients = NULL;
    
    // Should not crash
    remove_client(&clients, 1);
    TEST_ASSERT_NULL(clients);
    
    return 1;
}

// Test remove_client_fd function
int test_remove_client_fd_middle(void)
{
    struct pollfd fds[5];
    fds[0].fd = 10;
    fds[1].fd = 20;
    fds[2].fd = 30;
    fds[3].fd = 40;
    fds[4].fd = 50;
    
    int new_count = remove_client_fd(fds, 5, 2);
    
    TEST_ASSERT_EQUAL(4, new_count);
    TEST_ASSERT_EQUAL(10, fds[0].fd);
    TEST_ASSERT_EQUAL(20, fds[1].fd);
    TEST_ASSERT_EQUAL(40, fds[2].fd); // 30 was removed, 40 moved to index 2
    TEST_ASSERT_EQUAL(50, fds[3].fd); // 50 moved to index 3
    
    return 1;
}

// Test remove_client_fd function (remove last)
int test_remove_client_fd_last(void)
{
    struct pollfd fds[3];
    fds[0].fd = 10;
    fds[1].fd = 20;
    fds[2].fd = 30;
    
    int new_count = remove_client_fd(fds, 3, 2);
    
    TEST_ASSERT_EQUAL(2, new_count);
    TEST_ASSERT_EQUAL(10, fds[0].fd);
    TEST_ASSERT_EQUAL(20, fds[1].fd);
    
    return 1;
}

// Test add_client_fd function
int test_add_client_fd_normal(void)
{
    struct pollfd fds[MAX_CLIENTS];
    fds[0].fd = 10;
    fds[1].fd = 20;
    
    int new_count = add_client_fd(fds, 30, 2);
    
    TEST_ASSERT_EQUAL(3, new_count);
    TEST_ASSERT_EQUAL(30, fds[2].fd);
    TEST_ASSERT_EQUAL(POLLIN, fds[2].events);
    
    return 1;
}

// Test add_client_fd function (at max capacity)
int test_add_client_fd_max_capacity(void)
{
    struct pollfd fds[MAX_CLIENTS];
    
    int new_count = add_client_fd(fds, 30, MAX_CLIENTS);
    
    TEST_ASSERT_EQUAL(MAX_CLIENTS, new_count); // Should not exceed max
    
    return 1;
}

// Test suite
test_case_t client_management_tests[] = {
    {"test_remove_client_single", test_remove_client_single},
    {"test_remove_client_first_of_multiple", test_remove_client_first_of_multiple},
    {"test_remove_client_middle_of_multiple", test_remove_client_middle_of_multiple},
    {"test_remove_client_last_of_multiple", test_remove_client_last_of_multiple},
    {"test_remove_client_non_existent", test_remove_client_non_existent},
    {"test_remove_client_empty_list", test_remove_client_empty_list},
    {"test_remove_client_fd_middle", test_remove_client_fd_middle},
    {"test_remove_client_fd_last", test_remove_client_fd_last},
    {"test_add_client_fd_normal", test_add_client_fd_normal},
    {"test_add_client_fd_max_capacity", test_add_client_fd_max_capacity}
};

void run_client_management_tests(void)
{
    run_test_suite("Client Management Functions", client_management_tests, sizeof(client_management_tests) / sizeof(client_management_tests[0]));
}

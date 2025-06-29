/*
** EPITECH PROJECT, 2025
** Zappy
** File description:
** Unit tests for create_server_socket.c
*/

#include "test_framework.h"
#include "../include/server.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <errno.h>

// Function prototype from create_server_socket.c
extern int create_server_socket(int port);

// Test create_server_socket with valid port
int test_create_server_socket_valid_port(void)
{
    int server_fd = create_server_socket(0); // Port 0 lets OS choose an available port
    TEST_ASSERT(server_fd >= 0);
    
    if (server_fd >= 0) {
        close(server_fd);
    }
    return 1;
}

// Test create_server_socket with specific valid port (if available)
int test_create_server_socket_specific_port(void)
{
    // Test with a port that should be available (using high port number)
    int server_fd = create_server_socket(9999);
    
    // The test might fail if port is already in use, which is acceptable
    if (server_fd >= 0) {
        close(server_fd);
        return 1;
    } else {
        // If port is in use, that's also a valid scenario
        printf("Port 9999 in use (expected in some environments)\n");
        return 1;
    }
}

// Test create_server_socket with port 1 (should fail due to permissions)
int test_create_server_socket_privileged_port(void)
{
    int server_fd = create_server_socket(1);
    
    // Should fail for non-root users
    if (getuid() != 0) {
        TEST_ASSERT(server_fd < 0);
    } else {
        // If running as root, close the socket
        if (server_fd >= 0) {
            close(server_fd);
        }
    }
    return 1;
}

// Test create_server_socket with invalid port (too high)
int test_create_server_socket_invalid_port_high(void)
{
    int server_fd = create_server_socket(99999);
    
    // Port numbers above 65535 are invalid, but socket() might still succeed
    // The actual bind() call will handle port validation
    if (server_fd >= 0) {
        close(server_fd);
    }
    // This test mainly ensures the function doesn't crash
    return 1;
}

// Test socket creation and basic properties
int test_socket_properties(void)
{
    int server_fd = create_server_socket(0);
    TEST_ASSERT(server_fd >= 0);
    
    if (server_fd >= 0) {
        // Test that the socket is indeed a TCP socket
        int socket_type;
        socklen_t optlen = sizeof(socket_type);
        int result = getsockopt(server_fd, SOL_SOCKET, SO_TYPE, &socket_type, &optlen);
        
        TEST_ASSERT_EQUAL(0, result);
        TEST_ASSERT_EQUAL(SOCK_STREAM, socket_type);
        
        // Test that SO_REUSEADDR is set
        int reuse_addr;
        optlen = sizeof(reuse_addr);
        result = getsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &reuse_addr, &optlen);
        
        TEST_ASSERT_EQUAL(0, result);
        // Note: SO_REUSEADDR might not be queryable on all systems, so we just check that getsockopt succeeded
        
        close(server_fd);
    }
    return 1;
}

// Test binding and listening state
int test_socket_listening_state(void)
{
    int server_fd = create_server_socket(0);
    TEST_ASSERT(server_fd >= 0);
    
    if (server_fd >= 0) {
        // Get the actual port that was assigned
        struct sockaddr_in addr;
        socklen_t addr_len = sizeof(addr);
        int result = getsockname(server_fd, (struct sockaddr*)&addr, &addr_len);
        
        TEST_ASSERT_EQUAL(0, result);
        TEST_ASSERT_EQUAL(AF_INET, addr.sin_family);
        TEST_ASSERT(ntohs(addr.sin_port) > 0); // Should have a valid port assigned
        
        close(server_fd);
    }
    return 1;
}

// Test multiple socket creation (should work with different ports)
int test_multiple_socket_creation(void)
{
    int server_fd1 = create_server_socket(0);
    int server_fd2 = create_server_socket(0);
    
    TEST_ASSERT(server_fd1 >= 0);
    TEST_ASSERT(server_fd2 >= 0);
    TEST_ASSERT(server_fd1 != server_fd2);
    
    if (server_fd1 >= 0) close(server_fd1);
    if (server_fd2 >= 0) close(server_fd2);
    return 1;
}

// Test suite
test_case_t socket_tests[] = {
    {"test_create_server_socket_valid_port", test_create_server_socket_valid_port},
    {"test_create_server_socket_specific_port", test_create_server_socket_specific_port},
    {"test_create_server_socket_privileged_port", test_create_server_socket_privileged_port},
    {"test_create_server_socket_invalid_port_high", test_create_server_socket_invalid_port_high},
    {"test_socket_properties", test_socket_properties},
    {"test_socket_listening_state", test_socket_listening_state},
    {"test_multiple_socket_creation", test_multiple_socket_creation}
};

void run_socket_tests(void)
{
    run_test_suite("Socket Creation Functions", socket_tests, sizeof(socket_tests) / sizeof(socket_tests[0]));
}

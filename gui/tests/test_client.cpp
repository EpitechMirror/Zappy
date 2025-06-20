// tests/test_client.cpp
#include <criterion/criterion.h>
#include <thread>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include "Client.hpp"

void fake_server() {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    addr.sin_port = htons(4242);

    bind(server_fd, (sockaddr*)&addr, sizeof(addr));
    listen(server_fd, 1);

    int client_fd = accept(server_fd, nullptr, nullptr);

    const char *msg = "10 20\n";
    send(client_fd, msg, strlen(msg), 0);

    char buffer[512];
    recv(client_fd, buffer, 512, 0);

    close(client_fd);
    close(server_fd);
}

Test(Client, create_and_disconnect) {
    Client client("localhost", 4242);
    client.disconnect();
    cr_assert(true);
}

Test(Client, map_access) {
    Client client("localhost", 4242);
    const Map& map = client.getMap();
    cr_assert_eq(map.getWidth(), 0);
}
/*
Test(Client, connect_and_receive_map_size) {
    std::thread server(fake_server);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    Client client("127.0.0.1", 4242);
    cr_assert(client.connectToServer());
    cr_assert(client.sendGraphicCommand());
    cr_assert(client.parseMapData());
    cr_assert_eq(client.getMapWidth(), 10);
    cr_assert_eq(client.getMapHeight(), 20);

    server.join();
}*/

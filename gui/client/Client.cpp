/*
** EPITECH PROJECT, 2025
** Zappy
** File description:
** Client
*/

#include "Client.hpp"

Client::Client(const std::string& host, int port)
    : _host(host), _port(port), _sock(-1), _connected(false) {}

Client::~Client() {
    disconnect();
}

void Client::createSocket() {
    _sock = socket(AF_INET, SOCK_STREAM, 0);
    if (_sock < 0)
        throw std::runtime_error("Failed to create socket");
}

void Client::connectToServer() {
    createSocket();

    struct sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(_port);

    struct hostent* server = gethostbyname(_host.c_str());
    if (!server)
        throw std::runtime_error("Host not found");

    std::memcpy(&serverAddr.sin_addr.s_addr, server->h_addr, server->h_length);

    if (connect(_sock, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0)
        throw std::runtime_error("Connection failed");

    _connected = true;
}

void Client::disconnect() {
    if (_connected) {
        close(_sock);
        _connected = false;
    }
}

void Client::sendMessage(const std::string& message) {
    if (!_connected) throw std::runtime_error("Not connected");
    if (send(_sock, message.c_str(), message.size(), 0) < 0)
        throw std::runtime_error("Failed to send message");
}

std::string Client::receiveMessage() {
    if (!_connected) throw std::runtime_error("Not connected");
    char buffer[1024] = {0};
    ssize_t len = recv(_sock, buffer, sizeof(buffer) - 1, 0);
    if (len < 0)
        throw std::runtime_error("Failed to receive message");
    return std::string(buffer, len);
}

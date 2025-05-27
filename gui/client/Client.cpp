/*
** EPITECH PROJECT, 2025
** Zappy
** File description:
** Client
*/

#include "Client.hpp"

Client::Client(const std::string &host, int port)
    : _host(host), _port(port), _socket(-1) {}

Client::~Client() {
    if (_socket != -1)
        close(_socket);
}

bool Client::connectToServer() {
    _socket = socket(AF_INET, SOCK_STREAM, 0);
    if (_socket < 0) {
        std::cerr << "Socket creation failed\n";
        return false;
    }

    sockaddr_in serv_addr {};
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(_port);

    if (inet_pton(AF_INET, _host.c_str(), &serv_addr.sin_addr) <= 0) {
        std::cerr << "Invalid address\n";
        return false;
    }

    if (connect(_socket, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        std::cerr << "Connection failed\n";
        return false;
    }

    return true;
}

bool Client::sendGraphicCommand() {
    const char *msg = "GRAPHIC\n";
    return send(_socket, msg, strlen(msg), 0) > 0;
}

bool Client::readLine(std::string &line) {
    line.clear();
    char ch;
    while (recv(_socket, &ch, 1, 0) > 0) {
        if (ch == '\n') break;
        line += ch;
    }
    return !line.empty();
}

bool Client::receiveMapSize() {
    std::string line;

    while (readLine(line)) {
        std::cout << "Received: [" << line << "]" << std::endl;
        std::istringstream iss(line);

        if (iss >> _mapWidth >> _mapHeight) {
            std::cout << "Parsed map size: " << _mapWidth << " x " << _mapHeight << std::endl;
            _map.setSize(_mapWidth, _mapHeight);
            return true;
        }
    }

    std::cerr << "Failed to receive map size\n";
    return false;
}

void Client::disconnect() {
    if (_socket != -1) {
        close(_socket);
        _socket = -1;
    }
}

const Map &Client::getMap() const {
    return _map;
}

int Client::getMapWidth() const {
    return _mapWidth;
}

int Client::getMapHeight() const {
    return _mapHeight;
}

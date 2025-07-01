/*
** EPITECH PROJECT, 2025
** Zappy
** File description:
** Client
*/

#include "Client.hpp"

Client::Client(const std::string &host, int port)
    : _host(host),
    _port(port),
    _socket(-1),
    _protocolHandler(_map),
    _hasMapSize(false) {}

Client::~Client() {
    if (_socket != -1)
        close(_socket);
}

bool Client::connectToServer() {
    _socket = socket(AF_INET, SOCK_STREAM, 0);
    if (_socket < 0) {
        Console::error("Socket creation failed: " + std::string(strerror(errno)));
        return false;
    }

    int flags = fcntl(_socket, F_GETFL, 0);
    if (flags < 0 || fcntl(_socket, F_SETFL, flags | O_NONBLOCK) < 0) {
        Console::error("Failed to set non-blocking mode: " + std::string(strerror(errno)));
        close(_socket);
        return false;
    }

    sockaddr_in serv_addr {};
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(_port);

    if (inet_pton(AF_INET, _host.c_str(), &serv_addr.sin_addr) <= 0) {
        Console::error("Invalid address: " + _host);
        close(_socket);
        return false;
    }

    if (connect(_socket, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        if (errno != EINPROGRESS) {
            Console::error("Connection failed: " + std::string(strerror(errno)));
            close(_socket);
            return false;
        }
    }

    return true;
}

bool Client::sendGraphicCommand() {
    const char *msg = "GRAPHIC\n";
    return send(_socket, msg, strlen(msg), 0) > 0;
}

bool Client::readLine(std::string &line) {
    size_t pos = _buffer.find('\n');
    if (pos == std::string::npos)
        return false;

    line = _buffer.substr(0, pos);
    _buffer.erase(0, pos + 1);
    return true;
}

void Client::parseData() {
    std::string line;
    while (readLine(line)) {
        Console::debug("Received line: " + line);
        _protocolHandler.handleLine(line);
    }
}

void Client::receiveData() {
    char temp[1024];
    ssize_t bytesRead = recv(_socket, temp, sizeof(temp) - 1, 0);

    if (bytesRead > 0) {
        temp[bytesRead] = '\0';
        _buffer += temp;
    }
    else if (bytesRead == 0) {
        Console::warning("Server closed connection");
        close(_socket);
        _socket = -1;
    }
    else {
        // bytesRead < 0
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            // Pas de données pour l'instant : c'est OK en mode non-bloquant
        } else {
            Console::error("Failed to receive data: " + std::string(strerror(errno)));
            close(_socket);
            _socket = -1;
        }
    }
}

void Client::update() {
    if (_socket != -1) {
        receiveData();
        parseData();
    }
}

bool Client::isMapReady() const {
    return _protocolHandler.isMapReady();
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

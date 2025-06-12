/*
** EPITECH PROJECT, 2025
** Zappy
** File description:
** Client
*/

#include "Client.hpp"

Client::Client(const std::string &host, int port)
    : _host(host), _port(port), _socket(-1), _hasMapSize(false) {}

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

    int flags = fcntl(_socket, F_GETFL, 0);
    if (flags < 0 || fcntl(_socket, F_SETFL, flags | O_NONBLOCK) < 0) {
        std::cerr << "Failed to set non-blocking mode\n";
        close(_socket);
        return false;
    }

    sockaddr_in serv_addr {};
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(_port);

    if (inet_pton(AF_INET, _host.c_str(), &serv_addr.sin_addr) <= 0) {
        std::cerr << "Invalid address\n";
        close(_socket);
        return false;
    }

    if (connect(_socket, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        if (errno != EINPROGRESS) {
            std::cerr << "Connection failed: " << strerror(errno) << "\n";
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
        std::cout << "Received: [" << line << "]" << std::endl;
        std::istringstream iss(line);
        std::string cmd;
        if (!(iss >> cmd)) continue;

        if (cmd == "msz") {
            if (iss >> _mapWidth >> _mapHeight) {
                _map.setSize(_mapWidth, _mapHeight);
                _hasMapSize = true;
            }
        }
        else if (cmd == "bct") {
            int x, y;
            Resources res;
            if (iss >> x >> y) {
                for (int i = 0; i < RESOURCE_COUNT; ++i) {
                    if (!(iss >> res.quantities[i])) {
                        std::cerr << "Invalid bct resource data\n";
                        break;
                    }
                }
                _map.setTileResources(x, y, res);
            }
        }
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
        std::cerr << "Server closed connection\n";
        close(_socket);
        _socket = -1;
    }
    else {
        // bytesRead < 0
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            // Pas de données pour l'instant : c'est OK en mode non-bloquant
        } else {
            std::cerr << "Recv error: " << strerror(errno) << "\n";
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
    return _hasMapSize && _map.isFullyInitialized();
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

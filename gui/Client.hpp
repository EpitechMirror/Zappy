/*
** EPITECH PROJECT, 2025
** Zappy
** File description:
** Client
*/

#include <raylib.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <cstring>
#include <thread>
#include <mutex>
#include <vector>
#include <SFML/Network.hpp>
#include <getopt.h>

constexpr int SCREEN_WIDTH = 1280;
constexpr int SCREEN_HEIGHT = 720;
constexpr char* WINDOW_TITLE = (char*)"ZAPPY 3D GUI";

#ifndef CLIENT_HPP_
#define CLIENT_HPP_

class ZappyClient {
public:
    ZappyClient(const std::string& host, unsigned short port)
    : _host(host), _port(port)
    {
        if (_socket.connect(_host, _port) != sf::Socket::Done) {
            throw std::runtime_error("Failed to connect to server");
        }
        _socket.setBlocking(false);
    }

    void authenticate() {
        const char* auth = "GRAPHIC\n";
        if (_socket.send(auth, strlen(auth)) != sf::Socket::Done) {
            throw std::runtime_error("Failed to send GRAPHIC authentication");
        }
    }

    // Thread-safe receive
    bool receive(std::string& out) {
        std::lock_guard<std::mutex> lock(_mtx);
        sf::Packet packet;
        if (_socket.receive(packet) == sf::Socket::Done) {
            packet >> out;
            return true;
        }
        return false;
    }

    // Thread-safe send
    void send(const std::string& msg) {
        std::lock_guard<std::mutex> lock(_mtx);
        sf::Packet packet;
        packet << msg;
        _socket.send(packet);
    }

private:
    std::string _host;
    unsigned short _port;
    sf::TcpSocket _socket;
    std::mutex _mtx;
};

#endif /* !CLIENT_HPP_ */

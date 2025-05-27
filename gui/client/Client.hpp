/*
** EPITECH PROJECT, 2025
** Zappy
** File description:
** Client
*/

#ifndef CLIENT_HPP_
#define CLIENT_HPP_

#pragma once

#include <string>
#include <getopt.h>
#include <stdexcept>
#include <iostream>
#include <unistd.h>
#include <sstream>
#include <cstring>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include "../map/Map.hpp"

class Client {
    public:
        Client(const std::string &host, int port);
        ~Client();

        bool connectToServer();
        bool sendGraphicCommand();
        bool receiveMapSize();
        int getMapWidth() const;
        int getMapHeight() const;
        const Map &getMap() const;
        void disconnect();

    private:
        std::string _host;
        int _port;
        int _socket;
        int _mapWidth;
        int _mapHeight;
        Map _map;

        bool readLine(std::string &line);
};

#endif /* !CLIENT_HPP_ */

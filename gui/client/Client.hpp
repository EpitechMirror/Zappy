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
#include <fcntl.h>
#include <errno.h>
#include "../map/Map.hpp"
#include "ProtocolHandler.hpp"

class Client {
    public:
        Client(const std::string &host, int port);
        ~Client();

        bool connectToServer();
        bool sendGraphicCommand();
        int getMapWidth() const;
        int getMapHeight() const;
        const Map &getMap() const;
        void disconnect();

        void update();
        bool isMapReady() const;

    private:
        std::string _host;
        int _port;
        int _socket;
        int _mapWidth;
        int _mapHeight;
        Map _map;
        bool _hasMapSize = false;
        std::string _buffer;
        ProtocolHandler _protocolHandler;

        void receiveData();
        void parseData();
        bool readLine(std::string &line);
};

#endif /* !CLIENT_HPP_ */

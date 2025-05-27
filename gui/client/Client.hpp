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
#include <cstring>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>

class Client {
public:
    Client(const std::string& host, int port);
    ~Client();

    void connectToServer();
    void disconnect();
    void sendMessage(const std::string& message);
    std::string receiveMessage();

private:
    std::string _host;
    int _port;
    int _sock;
    bool _connected;

    void createSocket();
};


#endif /* !CLIENT_HPP_ */

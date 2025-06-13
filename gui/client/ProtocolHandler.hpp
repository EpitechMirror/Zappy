/*
** EPITECH PROJECT, 2025
** Zappy
** File description:
** ProtocolHandler
*/

#ifndef PROTOCOLHANDLER_HPP_
#define PROTOCOLHANDLER_HPP_

#pragma once

#include <string>
#include <unordered_map>
#include <functional>
#include <sstream>
#include "../map/Map.hpp"

class ProtocolHandler {
public:
    ProtocolHandler(Map &map);

    void handleLine(const std::string &line);
    bool isMapReady() const;

private:
    Map &_map;
    bool _hasMapSize = false;

    using CommandHandler = std::function<void(std::istringstream &)>;
    std::unordered_map<std::string, CommandHandler> _handlers;

    void registerHandlers();

    void handleMsz(std::istringstream &iss);
    void handleBct(std::istringstream &iss);
    void handleTna(std::istringstream &iss);
    void handleEnw(std::istringstream &iss);
    void handleEboAndEdi(std::istringstream &iss);
};


#endif /* !PROTOCOLHANDLER_HPP_ */

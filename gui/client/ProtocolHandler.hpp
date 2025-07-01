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
#include "../player/Player.hpp"
#include <iostream>

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

    void handleSgt(std::istringstream &iss);
    void handleMsz(std::istringstream &iss);
    void handleBct(std::istringstream &iss);
    void handleTna(std::istringstream &iss);
    void handleEnw(std::istringstream &iss);
    void handleEboAndEdi(std::istringstream &iss);
    void handlePnw(std::istringstream &iss);
    void handlePpo(std::istringstream &iss);
    void handlePlv(std::istringstream &iss);
    void handlePin(std::istringstream &iss);
    void handlePdi(std::istringstream &iss);
    void handlePgt(std::istringstream &iss);
    void handlePdr(std::istringstream &iss);
    void handlePfk(std::istringstream &iss);
    void handlePic(std::istringstream &iss);
    void handlePie(std::istringstream &iss);
    void handlePbc(std::istringstream &iss);

    void handleSmg(std::istringstream &iss);
    void handleSuc(std::istringstream &iss);
    void handleSbp(std::istringstream &iss);

    void handleSeg(std::istringstream &iss);
};


#endif /* !PROTOCOLHANDLER_HPP_ */

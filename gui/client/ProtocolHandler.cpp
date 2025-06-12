/*
** EPITECH PROJECT, 2025
** Zappy
** File description:
** ProtocolHandler
*/

#include "ProtocolHandler.hpp"
#include "../renderer/Player/Player.hpp"
#include <iostream>

ProtocolHandler::ProtocolHandler(Map &map) : _map(map) {
    registerHandlers();
}

void ProtocolHandler::registerHandlers() {
    _handlers["msz"] = [this](std::istringstream &iss) { handleMsz(iss); };
    _handlers["bct"] = [this](std::istringstream &iss) { handleBct(iss); };
    _handlers["tna"] = [this](std::istringstream &iss) { handleTna(iss); };
    _handlers["enw"] = [this](std::istringstream &iss) { handleEnw(iss); };
}

void ProtocolHandler::handleLine(const std::string &line) {
    std::istringstream iss(line);
    std::string cmd;
    if (!(iss >> cmd))
        return;

    auto it = _handlers.find(cmd);
    if (it != _handlers.end()) {
        it->second(iss);
    } else {
        std::cerr << "Unknown command: " << cmd << "\n";
    }
}

bool ProtocolHandler::isMapReady() const {
    return _hasMapSize && _map.isFullyInitialized();
}

void ProtocolHandler::handleMsz(std::istringstream &iss) {
    int w, h;
    if (iss >> w >> h) {
        _map.setSize(w, h);
        _hasMapSize = true;
    }
}

void ProtocolHandler::handleBct(std::istringstream &iss) {
    int x, y;
    Resources res;
    if (iss >> x >> y) {
        for (int i = 0; i < RESOURCE_COUNT; ++i) {
            if (!(iss >> res.quantities[i])) {
                std::cerr << "Invalid bct resource data\n";
                return;
            }
        }
        _map.setTileResources(x, y, res);
    }
}

void ProtocolHandler::handleTna(std::istringstream &iss) {
    std::string teamName;
    if (iss >> teamName) {
        Player::addTeamName(teamName);
    }
}

void ProtocolHandler::handleEnw(std::istringstream &iss) {
    std::string eggIdStr, playerIdStr;
    int x, y;

    if (iss >> eggIdStr >> playerIdStr >> x >> y) {
        int eggId = std::stoi(eggIdStr.substr(1));
        int playerId = std::stoi(playerIdStr.substr(1));

        _map.addEgg(eggId, x, y);
    } else {
        std::cerr << "Invalid enw format\n";
    }
}

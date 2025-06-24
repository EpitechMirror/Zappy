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
    _handlers["ebo"] = [this](std::istringstream &iss) { handleEboAndEdi(iss); };
    _handlers["edi"] = [this](std::istringstream &iss) { handleEboAndEdi(iss); };
    _handlers["pnw"] = [this](std::istringstream &iss) { handlePnw(iss); };
    _handlers["ppo"] = [this](std::istringstream &iss) { handlePpo(iss); };
    _handlers["plv"] = [this](std::istringstream &iss) { handlePlv(iss); };
    _handlers["pin"] = [this](std::istringstream &iss) { handlePin(iss); };
    _handlers["pdi"] = [this](std::istringstream &iss) { handlePdi(iss); };
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
        // playerId non utilisé ici sauf si besoin
        _map.addEgg(eggId, x, y);
    } else {
        std::cerr << "Invalid enw format\n";
    }
}

void ProtocolHandler::handleEboAndEdi(std::istringstream &iss) {
    std::string eggIdStr;
    if (iss >> eggIdStr) {
        int eggId = std::stoi(eggIdStr.substr(1));
        _map.removeEgg(eggId);
    } else {
        std::cerr << "Invalid ebo/edi format\n";
    }
}

void ProtocolHandler::handlePnw(std::istringstream &iss) {
    std::string idStr;
    int x, y, orientation, level;
    std::string teamName;

    if (iss >> idStr >> x >> y >> orientation >> level >> teamName) {
        int id = std::stoi(idStr.substr(1));
        Player player(
            id,
            { static_cast<float>(x), 0.0f, static_cast<float>(y) },
            orientation,
            level,
            teamName
        );
        _map.addPlayer(player);
    } else {
        std::cerr << "Invalid pnw format\n";
    }
}

void ProtocolHandler::handlePpo(std::istringstream &iss) {
    std::string idStr;
    int x, y, orientation;

    if (iss >> idStr >> x >> y >> orientation) {
        int id = std::stoi(idStr.substr(1));
        _map.updatePlayerPosition(
            id,
            { static_cast<float>(x), 0.0f, static_cast<float>(y) },
            orientation
        );
    } else {
        std::cerr << "Invalid ppo format\n";
    }
}

void ProtocolHandler::handlePlv(std::istringstream &iss) {
    std::string idStr;
    int level;

    if (iss >> idStr >> level) {
        int id = std::stoi(idStr.substr(1));
        _map.updatePlayerLevel(id, level);
    } else {
        std::cerr << "Invalid plv format\n";
    }
}

void ProtocolHandler::handlePin(std::istringstream &iss) {
    std::string idStr;
    int x, y;
    int inventory[RESOURCE_COUNT];

    if (iss >> idStr >> x >> y) {
        for (int i = 0; i < RESOURCE_COUNT; ++i) {
            if (!(iss >> inventory[i])) {
                std::cerr << "Invalid pin inventory data\n";
                return;
            }
        }
        int id = std::stoi(idStr.substr(1));
        _map.updatePlayerInventory(id, inventory);
    } else {
        std::cerr << "Invalid pin format\n";
    }
}

void ProtocolHandler::handlePdi(std::istringstream &iss) {
    std::string idStr;
    if (iss >> idStr) {
        int id = std::stoi(idStr.substr(1));
        _map.removePlayerById(id);
    } else {
        std::cerr << "Invalid pdi format\n";
    }
}

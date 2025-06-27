/*
** EPITECH PROJECT, 2025
** Zappy
** File description:
** ProtocolHandler
*/

#include "ProtocolHandler.hpp"

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
    _handlers["pgt"] = [this](std::istringstream &iss) { handlePgt(iss); };
    _handlers["pdr"] = [this](std::istringstream &iss) { handlePdr(iss); };
    _handlers["pfk"] = [this](std::istringstream &iss) { handlePfk(iss); };
    _handlers["smg"] = [this](std::istringstream &iss) { handleSmg(iss); };
    _handlers["suc"] = [this](std::istringstream &iss) { handleSuc(iss); };
    _handlers["sbp"] = [this](std::istringstream &iss) { handleSbp(iss); };
    _handlers["pic"] = [this](std::istringstream &iss) { handlePic(iss); };
    _handlers["pie"] = [this](std::istringstream &iss) { handlePie(iss); };
    _handlers["pbc"] = [this](std::istringstream &iss) { handlePbc(iss); };
    _handlers["seg"] = [this](std::istringstream &iss) { handleSeg(iss); };
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

//---Map size
void ProtocolHandler::handleMsz(std::istringstream &iss) {
    int w, h;
    if (iss >> w >> h) {
        _map.setSize(w, h);
        _hasMapSize = true;
    }
}

//---Tile resources
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

//---Team name
void ProtocolHandler::handleTna(std::istringstream &iss) {
    std::string teamName;
    if (iss >> teamName) {
        Player::addTeamName(teamName);
    }
}

//---An egg was laid by a player
void ProtocolHandler::handleEnw(std::istringstream &iss) {
    std::string eggIdStr, playerIdStr;
    int x, y;

    if (iss >> eggIdStr >> playerIdStr >> x >> y) {
        int eggId = std::stoi(eggIdStr.substr(1));
        _map.addEgg(eggId, x, y);
    } else {
        std::cerr << "Invalid enw format\n";
    }
}

//---Egg die
void ProtocolHandler::handleEboAndEdi(std::istringstream &iss) {
    std::string eggIdStr;
    if (iss >> eggIdStr) {
        int eggId = std::stoi(eggIdStr.substr(1));
        _map.removeEgg(eggId);
    } else {
        std::cerr << "Invalid ebo/edi format\n";
    }
}

//---New player
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

//---Player's position
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

//---Player's level
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

//---Player's inventory
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

//---Player disconnect
void ProtocolHandler::handlePdi(std::istringstream &iss) {
    std::string idStr;
    if (iss >> idStr) {
        int id = std::stoi(idStr.substr(1));
        _map.removePlayerById(id);
    } else {
        std::cerr << "Invalid pdi format\n";
    }
}

//---Player get a resource
void ProtocolHandler::handlePgt(std::istringstream &iss) {
    std::string idStr;
    int resourceIndex;

    if (!(iss >> idStr >> resourceIndex)) {
        std::cerr << "Invalid pgt format\n";
        return;
    }

    int id = std::stoi(idStr.substr(1));
    Player* player = _map.getPlayerById(id);
    if (!player) {
        std::cerr << "Unknown player id in pgt: " << id << "\n";
        return;
    }

    if (resourceIndex < 0 || resourceIndex >= RESOURCE_COUNT) {
        std::cerr << "Invalid resource index in pgt: " << resourceIndex << "\n";
        return;
    }

    const int* currentInventory = player->getInventory();
    int updatedInventory[RESOURCE_COUNT];
    for (int i = 0; i < RESOURCE_COUNT; ++i) {
        updatedInventory[i] = currentInventory[i];
    }
    updatedInventory[resourceIndex]++;

    player->setInventory(updatedInventory);
}

//---Player drop a resource
void ProtocolHandler::handlePdr(std::istringstream &iss) {
    std::string idStr;
    int resourceIndex;

    if (!(iss >> idStr >> resourceIndex)) {
        std::cerr << "Invalid pdr format\n";
        return;
    }

    int id = std::stoi(idStr.substr(1));
    Player* player = _map.getPlayerById(id);
    if (!player) {
        std::cerr << "Unknown player id in pdr: " << id << "\n";
        return;
    }

    if (resourceIndex < 0 || resourceIndex >= RESOURCE_COUNT) {
        std::cerr << "Invalid resource index in pdr: " << resourceIndex << "\n";
        return;
    }

    const int* currentInventory = player->getInventory();
    int updatedInventory[RESOURCE_COUNT];
    for (int i = 0; i < RESOURCE_COUNT; ++i) {
        updatedInventory[i] = currentInventory[i];
    }

    if (updatedInventory[resourceIndex] > 0)
        updatedInventory[resourceIndex]--;
    else
        std::cerr << "Warning: Player #" << id << " tried to drop resource " << resourceIndex << " with none in inventory\n";

    player->setInventory(updatedInventory);
}

//----Player drop an egg
void ProtocolHandler::handlePfk(std::istringstream &iss) {
    //std::string idStr;
//
    //if (!(iss >> idStr)) {
    //    std::cerr << "Invalid pfk format\n";
    //    return;
    //}
//
    //int playerId = std::stoi(idStr.substr(1));
    //Player* player = _map.getPlayerById(playerId);
    //if (!player) {
    //    std::cerr << "Unknown player in pfk: #" << playerId << "\n";
    //    return;
    //}
//
    //Vector3 pos = player->getPosition();
    //int x = static_cast<int>(pos.x);
    //int y = static_cast<int>(pos.z);
//
    //_map.addEgg(-1, x, y);
//
    //std::cout << "Player #" << playerId << " laid an egg at (" << x << ", " << y << ")\n";
}

//----Player begin incantation
void ProtocolHandler::handlePic(std::istringstream &iss) {
    int x, y, level;
    if (!(iss >> x >> y >> level)) {
        std::cerr << "Invalid pic format\n";
        return;
    }

    std::vector<int> playerIds;
    std::string token;
    while (iss >> token) {
        if (token[0] == '#') {
            try {
                int id = std::stoi(token.substr(1));
                playerIds.push_back(id);
            } catch (...) {
                std::cerr << "Invalid player ID in pic: " << token << "\n";
            }
        } else {
            std::cerr << "Unexpected token in pic: " << token << "\n";
        }
    }

    if (playerIds.empty()) {
        std::cerr << "pic with no players\n";
        return;
    }

    _map.startIncantation(x, y, level, playerIds);

    std::cout << "Incantation started at (" << x << ", " << y << ") level " << level
              << " with players: ";
    for (int id : playerIds)
        std::cout << "#" << id << " ";
    std::cout << "\n";
}

//--- Player end incantation
void ProtocolHandler::handlePie(std::istringstream &iss) {
    int x, y;
    std::string result;

    if (!(iss >> x >> y >> result)) {
        std::cerr << "Invalid pie format\n";
        return;
    }

    if (result == "0") {
        _map.clearIncantationAt(x, y);
        std::cout << "Incantation at (" << x << ", " << y << ") failed\n";
    } else if (result == "1") {
        _map.clearIncantationAt(x, y);
        std::cout << "Incantation at (" << x << ", " << y << ") succeeded\n";
    } else {
        std::cerr << "Unknown result in pie: " << result << "\n";
    }
}

//--- Player broadcast
void ProtocolHandler::handlePbc(std::istringstream &iss) {
    std::string idStr, message;
    if (!(iss >> idStr)) {
        std::cerr << "Invalid pbc format\n";
        return;
    }

    int playerId = std::stoi(idStr.substr(1));
    if (!std::getline(iss, message)) {
        std::cerr << "No message provided in pbc\n";
        return;
    }

    if (message.empty() || message[0] == ' ')
        message.erase(0, 1);

    Player* player = _map.getPlayerById(playerId);
    if (!player) {
        std::cerr << "Unknown player in pbc: #" << playerId << "\n";
        return;
    }

    std::cout << "[Broadcast from #" << playerId << "] " << message << "\n";
}

//--- Server message
void ProtocolHandler::handleSmg(std::istringstream &iss) {
    std::string message;
    std::getline(iss, message);
    if (!message.empty() && message[0] == ' ')
        message.erase(0, 1);

    std::cout << "[Server Message] " << message << "\n";
    // Optionnel : stocker dans un historique, afficher?
}

//---Unknown command
void ProtocolHandler::handleSuc(std::istringstream &) {
    std::cerr << "Error: Unknown command sent to server (suc)\n";
    // Optionnel : Afficher dans l’interface ?
}

//---Bad parameter
void ProtocolHandler::handleSbp(std::istringstream &) {
    std::cerr << "Error: Invalid parameters in command (sbp)\n";
    // Optionnel : Afficher dans l’interface ?
}

//---Team winner
void ProtocolHandler::handleSeg(std::istringstream &iss) {
    std::string teamName;
    if (!(iss >> teamName)) {
        std::cerr << "Invalid seg format\n";
        return;
    }

    _map.setGameOver(true, teamName);

    std::cout << "Game Over! Team " << teamName << " wins!\n";
}

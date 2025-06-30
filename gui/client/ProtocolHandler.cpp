/*
** EPITECH PROJECT, 2025
** Zappy
** File description:
** ProtocolHandler
*/

#include "ProtocolHandler.hpp"
#include "../Console/Console.hpp"

ProtocolHandler::ProtocolHandler(Map &map) : _map(map) {
    registerHandlers();
}

void ProtocolHandler::registerHandlers() {
    _handlers["WELCOME"] = [this](std::istringstream &) {
        Console::debug("Received WELCOME from server");
    };
    _handlers["sgt"] = [this](std::istringstream &iss) { handleSgt(iss); };
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
        Console::error("Unknown command: " + cmd );
    }
}

bool ProtocolHandler::isMapReady() const {
    return _hasMapSize && _map.isFullyInitialized();
}

// --- Server game time
void ProtocolHandler::handleSgt(std::istringstream &iss) {
    int time = -1;
    if (!(iss >> time)) {
        Console::warning("handleSgt: Invalid sgt format received from server");
        return;
    }
    if (time <= 0) {
        Console::warning("handleSgt: Received non-positive game time value: " + std::to_string(time));
        return;
    }

    _map.setGameTime(time);
    Console::debug("handleSgt: Game time set to " + std::to_string(time));
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
                Console::warning("Invalid bct resource data");
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
        Console::warning("Invalid enw format");
    }
}

//---Egg die
void ProtocolHandler::handleEboAndEdi(std::istringstream &iss) {
    std::string eggIdStr;
    if (iss >> eggIdStr) {
        int eggId = std::stoi(eggIdStr.substr(1));
        _map.removeEgg(eggId);
    } else {
        Console::warning("Invalid ebo or edi format");
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
        Console::warning("Invalide pnw format");
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
        Console::warning("Invalid ppo format");
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
        Console::warning("Invalid plv format");
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
                Console::warning("Invalid pin inventory data");
                return;
            }
        }
        int id = std::stoi(idStr.substr(1));
        _map.updatePlayerInventory(id, inventory);
    } else {
        Console::warning("Invalid pin format");
    }
}

//---Player disconnect
void ProtocolHandler::handlePdi(std::istringstream &iss) {
    std::string idStr;
    if (iss >> idStr) {
        int id = std::stoi(idStr.substr(1));
        _map.removePlayerById(id);
    } else {
        Console::warning("Invalid pdi format");
    }
}

//---Player get a resource
void ProtocolHandler::handlePgt(std::istringstream &iss) {
    std::string idStr;
    int resourceIndex;

    if (!(iss >> idStr >> resourceIndex)) {
        Console::warning("Invalid pgt format");
        return;
    }

    int id = std::stoi(idStr.substr(1));
    Player* player = _map.getPlayerById(id);
    if (!player) {
        Console::warning("Unknown player id in pgt: " + id);
        return;
    }

    if (resourceIndex < 0 || resourceIndex >= RESOURCE_COUNT) {
        Console::warning("Invalid resource index in pgt " + std::to_string(resourceIndex));
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
        Console::warning("Invalid pdr format");
        return;
    }

    int id = std::stoi(idStr.substr(1));
    Player* player = _map.getPlayerById(id);
    if (!player) {
        Console::warning("Unknown player id in pdr: " + std::to_string(id));
        return;
    }

    if (resourceIndex < 0 || resourceIndex >= RESOURCE_COUNT) {
        Console::warning("Invalid resource index in pdr: " + std::to_string(resourceIndex));
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
        Console::warning("Warning: Player #" + std::to_string(id) + " tried to drop resource " + std::to_string(resourceIndex) + " with none in inventory");

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
        Console::warning("Invalid pic format");
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
                Console::warning("Invalid player ID in pic: " + token);
            }
        } else {
            Console::warning("Unexpected token in pic: " + token);
        }
    }

    if (playerIds.empty()) {
        Console::warning("pic with no players");
        return;
    }

    _map.startIncantation(x, y, level, playerIds);

    std::string playersStr;
    for (size_t i = 0; i < playerIds.size(); ++i) {
        playersStr += std::to_string(playerIds[i]);
        if (i != playerIds.size() - 1)
            playersStr += ", ";
    }
    Console::debug("Incantation started at (" + std::to_string(x) + ", " + std::to_string(y) + ") level " + std::to_string(level) + " with players : " + playersStr);
}

//--- Player end incantation
void ProtocolHandler::handlePie(std::istringstream &iss) {
    int x, y;
    std::string result;

    if (!(iss >> x >> y >> result)) {
        Console::warning("Invalid pie format");
        return;
    }

    if (result == "0") {
        _map.clearIncantationAt(x, y);
        Console::warning("Incantation at (" + std::to_string(x) + ", " + std::to_string(y) + ") failed");
    } else if (result == "1") {
        _map.clearIncantationAt(x, y);
        Console::warning("Incantation at (" + std::to_string(x) + ", " + std::to_string(y) + ") succeeded");
    } else {
        Console::warning("Unknown result in pie: " + result);
    }
}

//--- Player broadcast
void ProtocolHandler::handlePbc(std::istringstream &iss) {
    std::string idStr, message;
    if (!(iss >> idStr)) {
        Console::warning("Invalid pbc format");
        return;
    }

    int playerId = std::stoi(idStr.substr(1));
    if (!std::getline(iss, message)) {
        Console::warning("No message provided in pbc");
        return;
    }

    if (message.empty() || message[0] == ' ')
        message.erase(0, 1);

    Player* player = _map.getPlayerById(playerId);
    if (!player) {
        Console::warning("Unknown player in pbc: #" + std::to_string(playerId));
        return;
    }

    Console::debug("[Broadcast from #" + std::to_string(playerId) + "] " + message);
}

//--- Server message
void ProtocolHandler::handleSmg(std::istringstream &iss) {
    std::string message;
    std::getline(iss, message);
    if (!message.empty() && message[0] == ' ')
        message.erase(0, 1);

    Console::info("[Server Message] " + message);
    // Optionnel : stocker dans un historique, afficher?
}

//---Unknown command
void ProtocolHandler::handleSuc(std::istringstream &) {
    Console::debug("Error: Unknown command sent to server (suc)");
    // Optionnel : Afficher dans l’interface ?
}

//---Bad parameter
void ProtocolHandler::handleSbp(std::istringstream &) {
    Console::debug("Error: Invalid parameters in command (sbp)");
    // Optionnel : Afficher dans l’interface ?
}

//---Team winner
void ProtocolHandler::handleSeg(std::istringstream &iss) {
    std::string teamName;
    if (!(iss >> teamName)) {
        Console::warning("Invalid seg format");
        return;
    }

    _map.setGameOver(true, teamName);

    Console::info("Game Over! Team " + teamName + " wins!");
}

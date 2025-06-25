/*
** EPITECH PROJECT, 2025
** Zappy
** File description:
** Client
*/

#include "Renderer.hpp"

Renderer::Renderer(int width, int height, const Map &map)
    : 
      _assets(map.getWidth(), map.getHeight()),
      _screenWidth(width),
      _screenHeight(height),
      _map(map),
      _cameraController(map.getWidth(), map.getHeight()),
      _mapInitialized(false),
      _disconnectTimer(0.0f)
{}

float Renderer::getDesktopY()
{
    int width = _map.getWidth();
    int height = _map.getHeight();
    float cellSize = 1.0f;

    float roomWidth = width * cellSize;
    float roomDepth = height * cellSize;

    float deskModelWidth = 0.5f;
    float deskModelDepth = 1.0f;
    float deskModelHeight = 1.0f;

    float scaleX = roomWidth / deskModelWidth;
    float scaleZ = roomDepth / deskModelDepth;
    float scaleY = (scaleX + scaleZ) / 2.0f;

    float deskTopY = (deskModelHeight / 2.0f) * scaleY;

    return deskTopY;
}

void Renderer::drawRoomAndy() {
    int width = _map.getWidth();
    int height = _map.getHeight();
    float cellSize = 1.0f;

    float roomWidth = width * cellSize;
    float roomDepth = height * cellSize;

    float deskModelWidth = 0.5f;
    float deskModelDepth = 1.0f;
    float deskModelHeight = 1.0f;

    float scaleX = roomWidth / deskModelWidth;
    float scaleZ = roomDepth / deskModelDepth;
    float scaleY = (scaleX + scaleZ) / 2.0f;

    // Décale le bureau pour que son dessus soit à y=0
    float deskY = -(deskModelHeight / 2.0f) * scaleY;
    float besideOffset = 3.0f;

    rlPushMatrix();
        rlTranslatef(roomWidth/2, deskY + besideOffset, roomDepth/2);
        rlRotatef(180, 0, 1, 0); // si besoin selon ton modèle
        rlScalef(scaleX, scaleY, scaleZ);
        DrawModel(_assets.deskModel, {-0.5f, 0.0f, 1.3f}, 1.0f, WHITE);
    rlPopMatrix();
}

void Renderer::initLights() {
    Shader& pbr = _assets.shaders.getPBR();
    _lights.clear();
    _lights.push_back(Light(LIGHT_POINT, { -1.0f, 1.0f, -2.0f }, { 0, 0, 0 }, YELLOW, 4.0f, pbr, 0));
    _lights.push_back(Light(LIGHT_POINT, { 2.0f, 1.0f, 1.0f }, { 0, 0, 0 }, GREEN, 3.3f, pbr, 1));
    _lights.push_back(Light(LIGHT_POINT, { -2.0f, 1.0f, 1.0f }, { 0, 0, 0 }, RED, 8.3f, pbr, 2));
    _lights.push_back(Light(LIGHT_POINT, { 1.0f, 1.0f, -2.0f }, { 0, 0, 0 }, BLUE, 2.0f, pbr, 3));
}

void Renderer::drawItems() {
    int width = _map.getWidth();
    int height = _map.getHeight();
    float cellSize = 1.0f;

    const int gridSize = 3;
    const float gridStep = cellSize / gridSize;
    const float offset = gridStep / 2.0f;

    // pour placer en fonction du bureau
    float deskTopY = getDesktopY();
    //hash pour positionner les ressources de manière pseudo-aléatoire
    auto resourceHash = [](int x, int y, int type) -> int {
        return (x * 73856093) ^ (y * 19349663) ^ (type * 83492791);
    };

    for (int x = 0; x < width; ++x) {
        for (int y = 0; y < height; ++y) {
            const Resources& res = _map.getTileResources(x, y);
            
            for (int i = 0; i < RESOURCE_COUNT; ++i) {
                int quantity = res.quantities[i];
                if (quantity <= 0) continue;

                int baseHash = resourceHash(x, y, i);
                
                for (int q = 0; q < quantity; ++q) {
                    int slot = (baseHash + q) % (gridSize * gridSize);
                    int gridX = slot % gridSize;
                    int gridZ = slot / gridSize;
                    
                    float posX = x * cellSize + gridX * gridStep + offset;
                    float posZ = y * cellSize + gridZ * gridStep + offset;
                    
                    float posY = deskTopY + (q * 0.01f);
                    
                    Vector3 pos = {posX, posY, posZ};
                    DrawSphere(pos, 0.08f, getColorForResource(static_cast<ResourceType>(i)));
                }
            }
        }
    }
}

void Renderer::DrawEggs() {
    // pour placer en fonction du bureau
    float deskTopY = getDesktopY();
    // float deskTopY = 2.0f;

    for (const Egg& egg : _map.getEggs()) {
        Vector3 pos = { static_cast<float>(egg.x) + 0.5f, deskTopY +0.09f , static_cast<float>(egg.y) + 0.5f };
        DrawSphere(pos, 0.13f, WHITE);
    }
}

Color Renderer::getColorForResource(ResourceType type) {
    switch (type) {
        case FOOD:      return ORANGE;
        case LINEMATE:  return SKYBLUE;
        case DERAUMERE: return GOLD;
        case SIBUR:     return PURPLE;
        case MENDIANE:  return RED;
        case PHIRAS:    return GREEN;
        case THYSTAME:  return PINK;
        default:        return WHITE;
    }
}

// void Renderer::drawFloor() {
//     float cellSize = 1.0f;
//     int width = _map.getWidth();
//     int height = _map.getHeight();

//     // pour placer en fonction du bureau
//     float deskTopY = getDesktopY();

//     Shader& pbr = _assets.shaders.getPBR();
//     int tilingLoc = GetShaderLocation(pbr, "tiling");
//     Vector2 tiling = {0.5f, 0.5f};
//     SetShaderValue(pbr, tilingLoc, &tiling, SHADER_UNIFORM_VEC2);

//     for (int x = 0; x < width; ++x) {
//         for (int y = 0; y < height; ++y) {
//             Vector3 pos = { x * cellSize + cellSize/2, deskTopY, y * cellSize + cellSize/2 };
//             DrawModel(_assets.floorModel, pos, cellSize, WHITE);
//         }
//     }
//     _mapInitialized = true;
// }

void Renderer::showLoadingScreen(const std::string &message) {
    float duration = 11.0f;
    float startTime = GetTime();
    float nextTipTime = 3.0f; 
    
    int tipIndex = GetRandomValue(0, _loadingTips.size() - 1);
    std::string tip = _loadingTips[tipIndex];

    while ((GetTime() - startTime) < duration && !WindowShouldClose()) {
        float elapsed = GetTime() - startTime;
        float progress = elapsed / duration;

        if (elapsed >= nextTipTime) {
            tipIndex = GetRandomValue(0, _loadingTips.size() - 1);
            tip = _loadingTips[tipIndex];
            nextTipTime += 3.0f;
        }

        BeginDrawing();
            ClearBackground(BLACK);

            // === Titre principal : WOODY GUI ===
            const char* logo = "WOODY GUI";
            int sizeLogo = 100;
            float spacing = 5.0f;
            Vector2 logoTextSize = MeasureTextEx(_assets.toyFont, logo, sizeLogo, spacing);
            float correction = spacing * 2.0f;
            Vector2 logoPos;
            logoPos.x = (_screenWidth - logoTextSize.x + correction) / 3.4f;
            logoPos.y = (_screenHeight / 2) - 200;

            Color outline = BLUE;
            for (int dx = -3; dx <= 3; dx += 3) {
                for (int dy = -3; dy <= 3; dy += 3) {
                    if (dx == 0 && dy == 0) continue;
                    Vector2 offsetPos = { logoPos.x + dx, logoPos.y + dy };
                    DrawTextEx(_assets.toyFont, logo, offsetPos, sizeLogo, spacing, outline);
                }
            }
            DrawTextEx(_assets.toyFont, logo, logoPos, sizeLogo, spacing, YELLOW);

            // === Texte Loading animé ===
            int loadingFontSize = 40;
            int charsToShow = std::min((int)(elapsed * 10), (int)message.size());
            std::string animatedText = message.substr(0, charsToShow);
            int textWidth = MeasureText(animatedText.c_str(), loadingFontSize);
            int posX = (_screenWidth - textWidth) / 2;
            int posY = _screenHeight / 2 - 80;
            DrawText(animatedText.c_str(), posX, posY, loadingFontSize, WHITE);

            // === Barre de chargement ===
            int barWidth = 300;
            int barHeight = 20;
            int barX = (_screenWidth - barWidth) / 2;
            int barY = posY + 60;

            DrawRectangle(barX, barY, barWidth, barHeight, GRAY);
            DrawRectangle(barX, barY, progress * barWidth, barHeight, GREEN);
            DrawRectangleLines(barX, barY, barWidth, barHeight, DARKGRAY);

            // === Boîte TIPS ===
            int tipFontSize = 20;
            int tipBoxWidth = MeasureText(tip.c_str(), tipFontSize) + 40;
            int tipBoxHeight = 80;
            int tipBoxX = (_screenWidth - tipBoxWidth) / 2;
            int tipBoxY = barY + 60;

            DrawRectangle(tipBoxX, tipBoxY, tipBoxWidth, tipBoxHeight, Fade(SKYBLUE, 0.25f));
            DrawRectangleLines(tipBoxX, tipBoxY, tipBoxWidth, tipBoxHeight, BLUE);
            
            float tipAlpha = 1.0f;
            if (nextTipTime - elapsed < 0.5f) {
                tipAlpha = (nextTipTime - elapsed) * 2.0f;
            }

            DrawText("TIPS :", tipBoxX + 10, tipBoxY + 5, tipFontSize, ColorAlpha(DARKBLUE, tipAlpha));
            DrawText(tip.c_str(), tipBoxX + 10, tipBoxY + 35, tipFontSize, ColorAlpha(WHITE, tipAlpha));

        EndDrawing();
    }
}

void Renderer::DrawPlayers() {
    const auto& players = _map.getPlayers();

    float cellSize = 1.0f;

    // pour placer en fonction du bureau
    float deskTopY = getDesktopY();

    for (const Player& p : players) {
        float px = p.getPosition().x * cellSize + cellSize/2;
        float pz = p.getPosition().z * cellSize + cellSize/2;
        float py = deskTopY;

        float yaw = 0;
        switch (p.getOrientation()) {
            case 1: yaw =   0; break; // Nord
            case 2: yaw =  90; break; // Est
            case 3: yaw = 180; break; // Sud
            case 4: yaw = 270; break; // Ouest
        }

        float scale = 0.5f;
        switch (p.getLevel()) {
            case 1: scale = 1.5f; break; // Niveau 1
            case 2: scale = 1.8f; break; // Niveau 2
            case 3: scale = 2.1f; break; // Niveau 3
            case 4: scale = 2.4f; break; // Niveau 4
            case 5: scale = 2.7f; break; // Niveau 5
            case 6: scale = 3.0f; break; // Niveau 6
            case 7: scale = 3.3f; break; // Niveau 7
            case 8: scale = 3.6f; break; // Niveau 8
            default: scale = 1.0f; break;
        }

        size_t h = std::hash<std::string>{}(p.getTeam());
        Color teamColor = ColorFromHSV((h % 360), 0.6f, 0.9f);

        rlPushMatrix();
            rlTranslatef(px, py, pz);
            rlRotatef(180.0f, 1.0f, 0.0f, 0.0f);
            rlRotatef(yaw,   0.0f, 1.0f, 0.0f);
            rlScalef(scale, scale, scale);

            DrawModel(_assets.playerModel, { 0.0f, 0.0f, 0.0f }, 1.0f, teamColor);
        rlPopMatrix();
    }
}

void Renderer::InfoItemsBoard() {
    DrawRectangle(10, 10, 200, 235, Fade(SKYBLUE, 0.5f));
    DrawRectangleLines(10, 10, 200, 235, BLUE);

    int x = 20;
    int y = 20;
    int lineSpacing = 20;

    DrawText(("Map Size : " + std::to_string(_map.getWidth()) + " x " + std::to_string(_map.getHeight())).c_str(), x, y, 20, BLACK);
    y += 2 * lineSpacing;

    DrawText(("Food : " + std::to_string(_map.getFoodCount())).c_str(), x, y, 20, ORANGE);
    y += lineSpacing;
    DrawText(("Linemate : " + std::to_string(_map.getLinemateCount())).c_str(), x, y, 20, SKYBLUE);
    y += lineSpacing;
    DrawText(("Deraumere : " + std::to_string(_map.getDeraumereCount())).c_str(), x, y, 20, GOLD);
    y += lineSpacing;
    DrawText(("Sibur : " + std::to_string(_map.getSiburCount())).c_str(), x, y, 20, PURPLE);
    y += lineSpacing;
    DrawText(("Mendiane : " + std::to_string(_map.getMendianeCount())).c_str(), x, y, 20, RED);
    y += lineSpacing;
    DrawText(("Phiras : " + std::to_string(_map.getPhirasCount())).c_str(), x, y, 20, GREEN);
    y += lineSpacing;
    DrawText(("Thystame : " + std::to_string(_map.getThystameCount())).c_str(), x, y, 20, PINK);
    y += lineSpacing;
    y += lineSpacing;
    DrawText(("Eggs : " + std::to_string(_map.getEggsCount())).c_str(), x, y, 20, WHITE);

    int timeInt = static_cast<int>(GetTime());
    std::string timeStr = "Time : " + std::to_string(timeInt);
    int timeTextWidth = MeasureText(timeStr.c_str(), 20);
    int timeCenterX = (_screenWidth - timeTextWidth) / 2;
    DrawText(timeStr.c_str(), timeCenterX, 10, 20, WHITE);
}

void Renderer::InfoTeamsBoard() {
    const std::vector<std::string>& teamNames = Player::getTeamNames();

    int titleSize = 20;
    int lineSpacing = 20;
    int padding = 10;

    int maxTextWidth = MeasureText("Teams : ", titleSize);
    for (const std::string& teamName : teamNames) {
        int w = MeasureText(teamName.c_str(), titleSize);
        if (w > maxTextWidth)
            maxTextWidth = w;
    }

    int boxWidth = maxTextWidth + 2 * padding;
    int boxHeight = (1 + teamNames.size()) * lineSpacing + 2 * padding;

    int boxX = _screenWidth - boxWidth - 10;
    int boxY = 10;

    DrawRectangle(boxX, boxY, boxWidth, boxHeight, Fade(SKYBLUE, 0.5f));
    DrawRectangleLines(boxX, boxY, boxWidth, boxHeight, BLUE);

    int x = boxX + padding;
    int y = boxY + padding;

    DrawText("Teams : ", x, y, titleSize, BLACK);
    y += lineSpacing;

    for (const std::string& teamName : teamNames) {
        DrawText(teamName.c_str(), x, y, titleSize, WHITE);
        y += lineSpacing;
    }
}

void Renderer::InfoPlayersBoard() {
    int titleSize = 20;
    int lineSpacing = 20;
    int padding = 10;

    const Player* selectedPlayer = nullptr;
    if (_selectedPlayerId) {
        for (const Player& p : _map.getPlayers()) {
            if (p.getId() == *_selectedPlayerId) {
                selectedPlayer = &p;
                break;
            }
        }
    }

    std::vector<std::string> lines;
    std::string title = "Player Info";
    
    if (selectedPlayer) {
        title = "Player #" + std::to_string(selectedPlayer->getId());
        
        lines.push_back("Level: " + std::to_string(selectedPlayer->getLevel()));
        lines.push_back("Team: " + selectedPlayer->getTeam());
        lines.push_back("Position: " + 
                        std::to_string(static_cast<int>(selectedPlayer->getPosition().x)) + "," +
                        std::to_string(static_cast<int>(selectedPlayer->getPosition().z)));
        
        std::string orientation;
        switch (selectedPlayer->getOrientation()) {
            case 1: orientation = "North"; break;
            case 2: orientation = "East"; break;
            case 3: orientation = "South"; break;
            case 4: orientation = "West"; break;
            default: orientation = "Unknown";
        }
        lines.push_back("Orientation: " + orientation);
        
        lines.push_back("Inventory:");
        const int* inventory = selectedPlayer->getInventory();
        if (inventory) {
            if (inventory[0] > 0) lines.push_back("  Food: " + std::to_string(inventory[0]));
            if (inventory[1] > 0) lines.push_back("  Linemate: " + std::to_string(inventory[1]));
            if (inventory[2] > 0) lines.push_back("  Deraumere: " + std::to_string(inventory[2]));
            if (inventory[3] > 0) lines.push_back("  Sibur: " + std::to_string(inventory[3]));
            if (inventory[4] > 0) lines.push_back("  Mendiane: " + std::to_string(inventory[4]));
            if (inventory[5] > 0) lines.push_back("  Phiras: " + std::to_string(inventory[5]));
            if (inventory[6] > 0) lines.push_back("  Thystame: " + std::to_string(inventory[6]));
        }
    } else {
        lines.push_back("Click on a player");
    }

    int maxWidth = MeasureText(title.c_str(), titleSize);
    for (const auto& line : lines) {
        int w = MeasureText(line.c_str(), titleSize);
        if (w > maxWidth) maxWidth = w;
    }
    
    int boxWidth = maxWidth + 2 * padding;
    int boxHeight = (1 + lines.size()) * lineSpacing + 2 * padding;

    int boxX = _screenWidth - boxWidth - 10;
    const std::vector<std::string>& teamNames = Player::getTeamNames();
    int teamsBoxHeight = (1 + teamNames.size()) * lineSpacing + 2 * padding;
    int boxY = 10 + teamsBoxHeight + 10;

    DrawRectangle(boxX, boxY, boxWidth, boxHeight, Fade(SKYBLUE, 0.5f));
    DrawRectangleLines(boxX, boxY, boxWidth, boxHeight, BLUE);

    int x = boxX + padding;
    int y = boxY + padding;

    DrawText(title.c_str(), x, y, titleSize, BLACK);
    y += lineSpacing;

    for (const auto& line : lines) {
        Color color = WHITE;
        
        if (line == "Click on a player") {
            color = DARKGRAY;
        }
        else if (line.find("Level:") != std::string::npos) color = YELLOW;
        else if (line.find("Food:") != std::string::npos) color = ORANGE;
        else if (line.find("Linemate:") != std::string::npos) color = SKYBLUE;
        else if (line.find("Deraumere:") != std::string::npos) color = GOLD;
        else if (line.find("Sibur:") != std::string::npos) color = PURPLE;
        else if (line.find("Mendiane:") != std::string::npos) color = RED;
        else if (line.find("Phiras:") != std::string::npos) color = GREEN;
        else if (line.find("Thystame:") != std::string::npos) color = PINK;
        
        DrawText(line.c_str(), x, y, titleSize, color);
        y += lineSpacing;
    }
}

void Renderer::InfoBoxBoard() {
    int titleSize = 20;
    int lineSpacing = 20;
    int padding = 10;

    std::string title = "Info Box : ";
    if (_selectedTile) {
        int tx = static_cast<int>(_selectedTile->x);
        int ty = static_cast<int>(_selectedTile->y);
        title += std::to_string(tx) + "," + std::to_string(ty);
    }

    int maxWidth = MeasureText(title.c_str(), titleSize);
    int contentLines = 1;

    std::vector<std::pair<std::string, Color>> resourceLines;
    std::string clickText = "Click on a box";
 
    if (_selectedTile) {
        int tx = static_cast<int>(_selectedTile->x);
        int ty = static_cast<int>(_selectedTile->y);
        const auto& res = _map.getTileResources(tx, ty);
        
        if (res.quantities[FOOD] > 0) {
            std::string txt = TextFormat("Food: %d", res.quantities[FOOD]);
            resourceLines.push_back({txt, ORANGE});
            maxWidth = std::max(maxWidth, MeasureText(txt.c_str(), titleSize));
        }
        if (res.quantities[LINEMATE] > 0) {
            std::string txt = TextFormat("Linemate: %d", res.quantities[LINEMATE]);
            resourceLines.push_back({txt, SKYBLUE});
            maxWidth = std::max(maxWidth, MeasureText(txt.c_str(), titleSize));
        }
        if (res.quantities[DERAUMERE] > 0) {
            std::string txt = TextFormat("Deraumere: %d", res.quantities[DERAUMERE]);
            resourceLines.push_back({txt, GOLD});
            maxWidth = std::max(maxWidth, MeasureText(txt.c_str(), titleSize));
        }
        if (res.quantities[SIBUR] > 0) {
            std::string txt = TextFormat("Sibur: %d", res.quantities[SIBUR]);
            resourceLines.push_back({txt, PURPLE});
            maxWidth = std::max(maxWidth, MeasureText(txt.c_str(), titleSize));
        }
        if (res.quantities[MENDIANE] > 0) {
            std::string txt = TextFormat("Mendiane: %d", res.quantities[MENDIANE]);
            resourceLines.push_back({txt, RED});
            maxWidth = std::max(maxWidth, MeasureText(txt.c_str(), titleSize));
        }
        if (res.quantities[PHIRAS] > 0) {
            std::string txt = TextFormat("Phiras: %d", res.quantities[PHIRAS]);
            resourceLines.push_back({txt, GREEN});
            maxWidth = std::max(maxWidth, MeasureText(txt.c_str(), titleSize));
        }
        if (res.quantities[THYSTAME] > 0) {
            std::string txt = TextFormat("Thystame: %d", res.quantities[THYSTAME]);
            resourceLines.push_back({txt, PINK});
            maxWidth = std::max(maxWidth, MeasureText(txt.c_str(), titleSize));
        }
        
        if (resourceLines.empty()) {
            std::string emptyTxt = "Empty tile";
            resourceLines.push_back({emptyTxt, DARKGRAY});
            maxWidth = std::max(maxWidth, MeasureText(emptyTxt.c_str(), titleSize));
        }
        
        contentLines += resourceLines.size();
    } else {
        maxWidth = std::max(maxWidth, MeasureText(clickText.c_str(), titleSize));
        contentLines++;
    }

    int boxWidth = maxWidth + 2 * padding;
    int boxHeight = contentLines * lineSpacing + 2 * padding;

    const std::vector<std::string>& teamNames = Player::getTeamNames();
    int teamsBoxHeight = (1 + teamNames.size()) * lineSpacing + 2 * padding;
    
    int playersBoxY = 10 + teamsBoxHeight + 10;
    int playersBoxHeight = 0;
    
    if (_selectedPlayerId) {
        const Player* selectedPlayer = nullptr;
        for (const Player& p : _map.getPlayers()) {
            if (p.getId() == *_selectedPlayerId) {
                selectedPlayer = &p;
                break;
            }
        }
        
        if (selectedPlayer) {
            int lines = 5;
            const int* inventory = selectedPlayer->getInventory();
            if (inventory) {
                for (int i = 0; i < 7; i++) {
                    if (inventory[i] > 0) lines++;
                }
            }
            playersBoxHeight = (1 + lines) * lineSpacing + 2 * padding;
        } else {
            playersBoxHeight = (1 + 1) * lineSpacing + 2 * padding;
        }
    } else {
        playersBoxHeight = (1 + 1) * lineSpacing + 2 * padding;
    }

    int boxX = _screenWidth - boxWidth - 10;
    int boxY = playersBoxY + playersBoxHeight + 10;

    DrawRectangle(boxX, boxY, boxWidth, boxHeight, Fade(SKYBLUE, 0.5f));
    DrawRectangleLines(boxX, boxY, boxWidth, boxHeight, BLUE);

    int x = boxX + padding;
    int y = boxY + padding;

    DrawText(title.c_str(), x, y, titleSize, BLACK);
    y += lineSpacing;
    
    if (_selectedTile) {
        for (const auto& [text, color] : resourceLines) {
            DrawText(text.c_str(), x, y, titleSize, color);
            y += lineSpacing;
        }
    } else {
        DrawText(clickText.c_str(), x, y, titleSize, DARKGRAY);
    }
}

void Renderer::gameLoop(Client &client) {
    _assets.loadFonts();
    _assets.loadAudio();
    if (!_mapInitialized) {
        showLoadingScreen("Loading...");
    }

    _assets.loadAllResources();
    _assets.applyShaders();
    initLights();

    while (!WindowShouldClose()) {
        bool wasConnected = client.isConnected();
        client.update();
        bool isConnected = client.isConnected();
        
        if (wasConnected && !isConnected) {
            notifyServerDisconnect();
            disconnected = true;
        }

        if (_disconnectTimer > 0) {
            _disconnectTimer -= GetFrameTime();
        }

        _cameraController.update();
        for (auto& l : _lights)
            l.updateShader(_assets.shaders.getPBR());

        UpdateMusicStream(_assets.mainMusic);

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            handleMouseClick();
        }

        BeginDrawing();
            ClearBackground(BLACK);

            BeginMode3D(_cameraController.getCamera());
               // drawFloor();
                drawRoomAndy();
                DrawGrid();
                drawItems();
                DrawEggs();
                DrawPlayers();
                DrawSelectionMarkers();
            EndMode3D();

            InfoItemsBoard();
            InfoTeamsBoard();
            InfoPlayersBoard();
            InfoBoxBoard();
            
            if (_disconnectTimer > 0) {
                handleServerDisconnect();
            }
        EndDrawing();
    }
}

void Renderer::handleServerDisconnect() {
    static float slideOffset = 0.0f;
    float targetOffset = (_disconnectTimer > 0) ? 0.0f : 50.0f;
    slideOffset = Lerp(slideOffset, targetOffset, 0.2f);

    const char* text = "Server disconnected";
    const float width = MeasureText(text, 20) + 40;
    const float height = 30.0f;
    const float posX = (_screenWidth - width) / 2.0f;
    const float posY = _screenHeight - height - slideOffset;

    DrawRectangleRounded(
        Rectangle{posX, posY, width, height},
        0.3f,
        10,
        Fade(MAROON, 0.7f)
    );

    DrawRectangleRoundedLines(
        Rectangle{posX, posY, width, height},
        0.3f,
        10, RED);

    DrawText(
        text,
        static_cast<int>(posX + (width - MeasureText(text, 20)) / 2),
        static_cast<int>(posY + 5),
        20,
        WHITE
    );
}

void Renderer::notifyServerDisconnect() {
    _disconnectTimer = 5.0f;
}

void Renderer::DrawGrid() {
    float cellSize = 1.0f;
    int width = _map.getWidth();
    int height = _map.getHeight();

    // pour placer en fonction du bureau
    float deskTopY = getDesktopY();
    float onDeskOffset = -0.5f; // Décalage pour que les lignes soient au-dessus du bureau

    for (int x = 0; x <= width; x++) {
        Vector3 start = { x * cellSize, deskTopY + onDeskOffset, 0.0f };
        Vector3 end = { x * cellSize, deskTopY + onDeskOffset, height * cellSize };
        DrawLine3D(start, end, GRAY);
    }

    for (int z = 0; z <= height; z++) {
        Vector3 start = { 0.0f, deskTopY + onDeskOffset, z * cellSize };
        Vector3 end = { width * cellSize, deskTopY + onDeskOffset, z * cellSize };
        DrawLine3D(start, end, GRAY);
    }
}

void Renderer::renderWindow(Client &client) {
    SetTraceLogLevel(LOG_NONE);
    InitWindow(_screenWidth, _screenHeight, "WOODY GUI");
    SetTargetFPS(60);

    gameLoop(client);

    _assets.unloadAllResources();

    CloseWindow();
    client.disconnect();
}

void Renderer::handleMouseClick() {
    Ray ray = GetMouseRay(GetMousePosition(), _cameraController.getCamera());

    // Player
    for (const Player& p : _map.getPlayers()) {
        float cellSize = 1.0f;
        
        float px = p.getPosition().x * cellSize + cellSize/2;
        float pz = p.getPosition().z * cellSize + cellSize/2;
        
        float width = 0.4f;
        float height = 1.0f;
        float depth = 0.4f;
        
        BoundingBox playerBox = {
            {
                px - width/2,0.0f, pz - depth/2},
            {
                px + width/2,
                height,
                pz + depth/2
            }
        };
        
        RayCollision collision = GetRayCollisionBox(ray, playerBox);
        if (collision.hit) {
            _selectedPlayerId = p.getId();
            return;
        }
    }

    // Box
    Vector3 hit;
    if (GetRayGroundIntersection(ray, hit)) {
        int tx = int(floor(hit.x));
        int ty = int(floor(hit.z));
        if (tx >= 0 && tx < _map.getWidth() && ty >= 0 && ty < _map.getHeight()) {
            _selectedTile = Vector2{ float(tx), float(ty) };
            return;
        }
    }
}

void Renderer::DrawSelectionMarkers() {
    if (_selectedPlayerId) {
        const Player* p = nullptr;
        for (const Player& player : _map.getPlayers()) {
            if (player.getId() == *_selectedPlayerId) {
                p = &player;
                break;
            }
        }
        if (p) {
            float cellSize = 1.0f;
            float px = p->getPosition().x * cellSize + cellSize/2;
            float pz = p->getPosition().z * cellSize + cellSize/2;
            
            float width = 0.4f;
            float height = 1.0f;
            float depth = 0.4f;
            
            BoundingBox playerBox = {
                { px - width/2, 0.0f, pz - depth/2 },
                { px + width/2, height, pz + depth/2 }
            };
            
            DrawBoundingBox(playerBox, Fade(GREEN, 0.3f));
        }
    }
    
    if (_selectedTile) {
        float tx = _selectedTile->x;
        float tz = _selectedTile->y;
        Vector3 center = { tx + 0.5f, 0.1f, tz + 0.5f };
        
        DrawCubeWires(center, 1.0f, 0.1f, 1.0f, Fade(RED, 0.5f));
    }
}

bool Renderer::GetRayGroundIntersection(Ray ray, Vector3 &outPoint) {
    if (ray.direction.y == 0) return false;
    float t = -ray.position.y / ray.direction.y;
    if (t < 0) return false;
    outPoint = Vector3Add(ray.position, Vector3Scale(ray.direction, t));
    return true;
}

bool Renderer::firstCall = true;
bool Renderer::disconnected = false;
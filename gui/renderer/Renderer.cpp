/*
** EPITECH PROJECT, 2025
** Zappy
** File description:
** Client
*/

#include "Renderer.hpp"

Renderer::Renderer(int width, int height, const Map &map) : _screenWidth(width), _screenHeight(height), _map(map) {
}

void Renderer::loadModels() {
    _floorModel = LoadModel("../resources/models/plane.glb");
    _playerModel = LoadModel("../resources/models/pixar_lamp/scene.gltf");
}

void Renderer::unloadModels() {
    UnloadModel(_floorModel);
    UnloadModel(_playerModel);
}

void Renderer::loadShaders() {
    _shaders.loadPBR();
}

void Renderer::unloadShaders() {
    _shaders.unloadAll();
}

void Renderer::loadTextures() {
    //Ici on charge les textures
    Texture2D floorAlbedo = LoadTexture("../resources/textures/bitume/bitume_diff.jpg");
    Texture2D floorNormal = LoadTexture("../resources/textures/bitume/bitume_disp.png");
    Texture2D floorMRA = LoadTexture("../resources/textures/bitume/bitume_nor.exr");
    Texture2D floorEmissive = LoadTexture("../resources/textures/bitume/bitume_rough.exr");

    // Et on les associe au modèle concerné (ici le sol)
    for (int i = 0; i < _floorModel.materialCount; ++i) {
        _floorModel.materials[i].maps[MATERIAL_MAP_ALBEDO].texture = floorAlbedo;
        _floorModel.materials[i].maps[MATERIAL_MAP_NORMAL].texture = floorNormal;
        _floorModel.materials[i].maps[MATERIAL_MAP_METALNESS].texture = floorMRA;
        _floorModel.materials[i].maps[MATERIAL_MAP_EMISSION].texture = floorEmissive;
    }


    //texture joueur (lampe)
    Texture2D lampAlbedo = LoadTexture("../resources/models/pixar_lamp/PixarLamp_baseColor.jpeg");
    Texture2D lampNormal = LoadTexture("../resources/models/pixar_lamp/PixarLamp_normal.jpeg");
    Texture2D lampMRA = LoadTexture("../resources/models/pixar_lamp/PixarLamp_metallicRoughness.jpeg");
    Texture2D lampEmissive = LoadTexture("../resources/models/pixar_lamp/PixarLamp_emissive.jpeg");

    //associé au modèle joueur (si on a plusieurs textures on fait une boucle)
    for (int i = 0; i < _playerModel.materialCount; ++i) {
        _playerModel.materials[i].maps[MATERIAL_MAP_ALBEDO].texture = lampAlbedo;
        _playerModel.materials[i].maps[MATERIAL_MAP_NORMAL].texture = lampNormal;
        _playerModel.materials[i].maps[MATERIAL_MAP_METALNESS].texture = lampMRA;
        _playerModel.materials[i].maps[MATERIAL_MAP_EMISSION].texture = lampEmissive;
    }
}

void Renderer::unloadTextures() {
    // Libère les textures du sol
    for (int i = 0; i < _floorModel.materialCount; ++i) {
        UnloadTexture(_floorModel.materials[i].maps[MATERIAL_MAP_ALBEDO].texture);
        UnloadTexture(_floorModel.materials[i].maps[MATERIAL_MAP_NORMAL].texture);
        UnloadTexture(_floorModel.materials[i].maps[MATERIAL_MAP_METALNESS].texture);
        UnloadTexture(_floorModel.materials[i].maps[MATERIAL_MAP_EMISSION].texture);
    }
    // Libère les textures du joueur
    for (int i = 0; i < _playerModel.materialCount; ++i) {
        UnloadTexture(_playerModel.materials[i].maps[MATERIAL_MAP_ALBEDO].texture);
        UnloadTexture(_playerModel.materials[i].maps[MATERIAL_MAP_NORMAL].texture);
        UnloadTexture(_playerModel.materials[i].maps[MATERIAL_MAP_METALNESS].texture);
        UnloadTexture(_playerModel.materials[i].maps[MATERIAL_MAP_EMISSION].texture);
    }
}

void Renderer::applyShaders() {
    // Ici on applique les shaders aux modèles
    Shader& pbr = _shaders.getPBR();
    for (int i = 0; i < _floorModel.materialCount; ++i)
        _floorModel.materials[i].shader = pbr;

    // PLusieurs textures donc on applique les shaders pour chacune d'elles
    for (int i = 0; i < _playerModel.materialCount; ++i)
        _playerModel.materials[i].shader = pbr;
}

void Renderer::initLights() {
    Shader& pbr = _shaders.getPBR();
    _lights.clear();
    _lights.push_back(Light(LIGHT_POINT, { -1.0f, 1.0f, -2.0f }, { 0, 0, 0 }, YELLOW, 4.0f, pbr, 0));
    _lights.push_back(Light(LIGHT_POINT, { 2.0f, 1.0f, 1.0f }, { 0, 0, 0 }, GREEN, 3.3f, pbr, 1));
    _lights.push_back(Light(LIGHT_POINT, { -2.0f, 1.0f, 1.0f }, { 0, 0, 0 }, RED, 8.3f, pbr, 2));
    _lights.push_back(Light(LIGHT_POINT, { 1.0f, 1.0f, -2.0f }, { 0, 0, 0 }, BLUE, 2.0f, pbr, 3));
}

//DOIT dessiner nos différentes pierres
void Renderer::drawItems() {
    int width = _map.getWidth();
    int height = _map.getHeight();
    float cellSize = 1.0f;

    const int gridSize = 3;
    const float gridStep = cellSize / gridSize;
    const float offset = gridStep / 2.0f;

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
                    
                    float posY = 0.2f + (q * 0.01f);
                    
                    Vector3 pos = {posX, posY, posZ};
                    DrawSphere(pos, 0.08f, getColorForResource(static_cast<ResourceType>(i)));
                }
            }
        }
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

void Renderer::drawFloor() {
    float cellSize = 1.0f;
    int width = _map.getWidth();
    int height = _map.getHeight();

    Shader& pbr = _shaders.getPBR();
    int tilingLoc = GetShaderLocation(pbr, "tiling");
    Vector2 tiling = {1.0f, 1.0f}; // 1 répétition par case
    SetShaderValue(pbr, tilingLoc, &tiling, SHADER_UNIFORM_VEC2);

    for (int x = 0; x < width; ++x) {
        for (int y = 0; y < height; ++y) {
            Vector3 pos = { x * cellSize + cellSize/2, 0.0f, y * cellSize + cellSize/2 };
            DrawModel(_floorModel, pos, cellSize, WHITE);
        }
    }
    _mapInitialized = true;
}

void Renderer::showLoadingScreen(const std::string &message) {
    BeginDrawing();
        ClearBackground(BLACK);
        int textSize = 40;
        int textWidth = MeasureText(message.c_str(), textSize);
        int posX = (_screenWidth - textWidth) / 2;
        int posY = _screenHeight / 2;
        DrawText(message.c_str(), posX, posY, textSize, WHITE);
    EndDrawing();
}

void Renderer::gameLoop(Client &client) {
    if (!_mapInitialized) {
        showLoadingScreen("Loading...");
    }

    //on init tout (a mettre dans une fonction init et meme dnas les classes associées)
    loadModels();
    loadTextures();
    loadShaders();
    applyShaders();
    initLights();

    while (!WindowShouldClose()) {
        client.update();
        float wheel = GetMouseWheelMove();

        if (wheel != 0.0f)
            _cameraController.zoom(-wheel);
        
        _cameraController.update();
        for (auto& l : _lights)
            l.updateShader(_shaders.getPBR());

        BeginDrawing();
            ClearBackground(BLACK);

            BeginMode3D(_cameraController.getCamera());
            // Charge les modèles
            // Dessine le sol (a voir plus tard pour adapter le dessin du sol a la bonne position)
            drawFloor();
            DrawGrid();

            // Dessine les ressources sur chaque case
            drawItems();

            // Dessine les joueurs (besoin d'enregistrer le nombre de joueurs)
            // Exemple de joueur sous forme de lampe, essayer de faire en sorte qu'en fonction de la team on change les couleurs
            //faire une boucle pour chaque joueur

            //Ici on dessine le jouer avec une rotation a l'aide de drawModelEx
            Vector3 lampPos = {0, 0, 0};
            Vector3 lampAxis = {1, 0, 0};
            float lampAngle = -180.0f;

            DrawModelEx(_playerModel, lampPos, lampAxis, lampAngle, {1.0f, 1.0f, 1.0f}, RED);

            // Dessine les lights (optionnel : sphères pour debug)
             for (const Light& l : _lights)
                 DrawSphere(l.getPosition(), 0.2f, YELLOW);
            EndMode3D();
            InfoBoard();
        EndDrawing();
    }
}

void Renderer::InfoBoard() {
    DrawRectangle(10, 10, 200, 215, Fade(SKYBLUE, 0.5f));
    DrawRectangleLines(10, 10, 200, 215, BLUE);

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
    DrawText(("Eggs : " + std::to_string(_map.getEggsCount())).c_str(), x, y, 20, WHITE);

    int timeInt = static_cast<int>(GetTime());
    std::string timeStr = "Time : " + std::to_string(timeInt);
    int timeTextWidth = MeasureText(timeStr.c_str(), 20);
    int timeCenterX = (_screenWidth - timeTextWidth) / 2;
    DrawText(timeStr.c_str(), timeCenterX, 10, 20, WHITE);

    int textWidth = MeasureText("Use ZQSD to move the camera", 20);
    int centerX = (_screenWidth - textWidth) / 2;
    int bottomY = _screenHeight - 30;
    DrawText("Use ZQSD to move the camera", centerX, bottomY, 20, RED);
}

void Renderer::DrawGrid() {
    int width = _map.getWidth();
    int height = _map.getHeight();
    float cellSize = 1.0f;

    for (int x = 0; x <= width; x++) {
        Vector3 start = { x * cellSize, 0.0f, 0.0f };
        Vector3 end = { x * cellSize, 0.0f, height * cellSize };
        DrawLine3D(start, end, GRAY);
    }

    for (int z = 0; z <= height; z++) {
        Vector3 start = { 0.0f, 0.0f, z * cellSize };
        Vector3 end = { width * cellSize, 0.0f, z * cellSize };
        DrawLine3D(start, end, GRAY);
    }
}

void Renderer::renderWindow(Client &client) {
    SetTraceLogLevel(LOG_NONE);
    InitWindow(_screenWidth, _screenHeight, "ZAPPY GUI");
    SetTargetFPS(60);

    gameLoop(client);

    unloadTextures();
    unloadModels();
    unloadShaders();

    CloseWindow();
    client.disconnect();
}

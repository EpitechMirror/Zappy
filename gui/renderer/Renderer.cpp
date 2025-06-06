/*
** EPITECH PROJECT, 2025
** Zappy
** File description:
** Client
*/

#include "Renderer.hpp"

Renderer::Renderer(int width, int height, const Map &map) : _screenWidth(width), _screenHeight(height), _map(map) {
}

// Charge les modèles nécessaires pour le rendu
void Renderer::loadModels() {
    _floorModel = LoadModel("../resources/models/plane.glb");
    _playerModel = LoadModel("../resources/models/robot.glb");
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
    Texture2D floorSand = LoadTexture("../resources/textures/floor/brick_pavement_sand.jpg");

    // Et on les associe au modèle concerné (ici le sol)
    _floorModel.materials[0].maps[MATERIAL_MAP_ALBEDO].texture = floorSand;
}

void Renderer::applyShaders() {
    // Ici on applique les shaders aux modèlesù
    Shader& pbr = _shaders.getPBR();
    _floorModel.materials[0].shader = pbr;
    _playerModel.materials[0].shader = pbr;
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
    //recuperer les infos du parsing et adapter la méthode
    int width = _map.getWidth();
    int height = _map.getHeight();
    float cellSize = 1.0f;
    for (int x = 0; x < width; ++x) {
        for (int y = 0; y < height; ++y) {
            const Resources& res = _map.getTileResources(x, y);
            Vector3 pos = { x * cellSize + 0.5f, 0.2f, y * cellSize + 0.5f };
            if (res.quantities[0] > 0)
                DrawSphere(pos, 0.15f, GREEN); // DESSINE LES SPHERES VERTES, adapter ensuite avec les différentes pierres
        }
    }
}

void Renderer::drawFloor() {
    float cellSize = 1.0f;
    int width = _map.getWidth();
    int height = _map.getHeight();

    for (int x = 0; x < width; ++x) {
        for (int y = 0; y < height; ++y) {
            Vector3 pos = { x * cellSize + cellSize/2, 0.0f, y * cellSize + cellSize/2 };
            // A voir ici pour varier la texture du sol en fonction de la case
            DrawModel(_floorModel, pos, cellSize, WHITE);
        }
    }
}

void Renderer::gameLoop(Client &client) {
    (void)client;
    //on init tout (a mettre dans une fonction init et meme dnas les classes associées)
    loadModels();
    loadTextures();
    loadShaders();
    applyShaders();
    initLights();

    while (!WindowShouldClose()) {
        float wheel = GetMouseWheelMove();

        if (wheel != 0.0f)
            _cameraController.zoom(-wheel);
        
        _cameraController.update();
        //client.update();
        for (auto& l : _lights)
            l.updateShader(_shaders.getPBR());

        BeginDrawing();
            ClearBackground(BLACK);

            BeginMode3D(_cameraController.getCamera());
            // Charge les modèles
            // Dessine le sol (a voir plus tard pour adapter le dessin du sol a la bonne position)
            //DrawModel(_floorModel, {0, 0, 0}, 1.0f, WHITE);
            drawFloor();
            DrawGrid();

            // Dessine les ressources sur chaque case
            drawItems();

            // Dessine les joueurs (besoin d'enregistrer le nombre de joueurs)
            // Exemple de joueur sous forme de robot,  a voir pour mettre la texture
            //faire une boucle pour chaque joueur
            DrawModel(_playerModel, {0, 0, 0}, 1.0f, BLACK);

            // Dessine les lights (optionnel : sphères pour debug)
            for (const Light& l : _lights)
                DrawSphere(l.getPosition(), 0.2f, YELLOW);
            
            

            //DrawSphereBasic(GOLD);

            EndMode3D();
            InfoBoard();
        EndDrawing();
    }
}

void Renderer::InfoBoard() {
    DrawRectangle( 10, 10, 200, 215, Fade(SKYBLUE, 0.5f));
    DrawRectangleLines( 10, 10, 200, 215, BLUE);
    std::string infoText = "Map Size: " + std::to_string(_map.getWidth()) + " x " + std::to_string(_map.getHeight()) +
                           "\n\nFood: " + std::to_string(_map.getFoodCount()) +
                           "\nLinemate: " + std::to_string(_map.getLinemateCount()) +
                           "\nDeraumere: " + std::to_string(_map.getDeraumereCount()) +
                           "\nSibur: " + std::to_string(_map.getSiburCount()) +
                           "\nMendiane: " + std::to_string(_map.getMendianeCount()) +
                           "\nPhiras: " + std::to_string(_map.getPhirasCount()) +
                           "\nThystame: " + std::to_string(_map.getThystameCount());

    DrawText(infoText.c_str(), 20, 20, 20, BLACK);

    // Display how to move the camera
    int textWidth = MeasureText("Use ZQSD to move the camera", 20);
    int x = (_screenWidth - textWidth) / 2;
    int y = _screenHeight - 30;
    DrawText("Use ZQSD to move the camera", x, y, 20, RED);
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

    CloseWindow();
    client.disconnect();
}

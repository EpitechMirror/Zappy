/*
** EPITECH PROJECT, 2025
** Zappy
** File description:
** Client
*/

#include "Renderer.hpp"

Renderer::Renderer(int width, int height, const Map &map) : _screenWidth(width), _screenHeight(height), _map(map) {
}

void Renderer::gameLoop(Client &client) {

    while (!WindowShouldClose()) {
        _cameraController.update();
        //client.update();
        BeginDrawing();
            ClearBackground(RAYWHITE);

            BeginMode3D(_cameraController.getCamera());

            DrawGrid();
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

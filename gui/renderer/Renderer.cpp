/*
** EPITECH PROJECT, 2025
** Zappy
** File description:
** Client
*/

#include "Renderer.hpp"

Renderer::Renderer(int width, int height, const Map &map) : _screenWidth(width), _screenHeight(height), _map(map) {
}

void Renderer::gameLoop() {

    while (!WindowShouldClose()) {
        _cameraController.update();

        BeginDrawing();
            ClearBackground(RAYWHITE);

            BeginMode3D(_cameraController.getCamera());

            // Draw the grid par rapport to the map size
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

            
            EndMode3D();

            DrawRectangle( 10, 10, 320, 93, Fade(SKYBLUE, 0.5f));
            DrawRectangleLines( 10, 10, 320, 93, BLUE);
            std::string infoText = "Map Size: " + std::to_string(_map.getWidth()) + " x " + std::to_string(_map.getHeight());

            DrawText(infoText.c_str(), 20, 20, 10, BLACK);
        EndDrawing();
    }
}

void Renderer::renderWindow() {
    InitWindow(_screenWidth, _screenHeight, "ZAPPY GUI");
    SetTargetFPS(60);

    gameLoop();

    CloseWindow();
}
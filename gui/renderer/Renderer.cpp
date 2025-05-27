/*
** EPITECH PROJECT, 2025
** Zappy
** File description:
** Client
*/

#include "Renderer.hpp"

Renderer::Renderer(int width, int height) : _screenWidth(width), _screenHeight(height) {
}

void Renderer::gameLoop() {

    while (!WindowShouldClose()) {
        _cameraController.update();

        BeginDrawing();
            ClearBackground(RAYWHITE);

            BeginMode3D(_cameraController.getCamera());

            DrawGrid(10, 1.0f);
            
            EndMode3D();

            DrawRectangle( 10, 10, 320, 93, Fade(SKYBLUE, 0.5f));
            DrawRectangleLines( 10, 10, 320, 93, BLUE);

            DrawText("TEXTE POUR METTRE DES INFOS A VOIR", 20, 20, 10, BLACK);
        EndDrawing();
    }
}

void Renderer::renderWindow() {
    InitWindow(_screenWidth, _screenHeight, "ZAPPY GUI");
    SetTargetFPS(60);

    gameLoop();

    CloseWindow();
}
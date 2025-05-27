/*
** EPITECH PROJECT, 2025
** Zappy GUI
** File description:
** main
*/

#include "raylib.h"
#include <iostream>
#include <string>
#include <getopt.h>

constexpr int SCREEN_WIDTH = 1280;
constexpr int SCREEN_HEIGHT = 720;

void printUsage(const char* progName) {
    std::cout << "Usage: " << progName << " -p <port> -h <host>" << std::endl;
}

int main(int argc, char** argv) {
    std::string host = "localhost";
    int port = 0;
    int opt;

    while ((opt = getopt(argc, argv, "p:h:")) != -1) {
        switch (opt) {
            case 'p': port = std::stoi(optarg); break;
            case 'h': host = optarg; break;
            default:
                printUsage(argv[0]);
                return EXIT_FAILURE;
        }
    }
    if (port <= 0) {
        printUsage(argv[0]);
        return EXIT_FAILURE;
    }

    // Initialize Raylib window
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Zappy 3D GUI");
    SetTargetFPS(60);

    // Main loop: run until window close
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        // TODO: render 3D scene here

        DrawText("Zappy GUI - Raylib 3D Base", 20, 20, 20, DARKGRAY);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}

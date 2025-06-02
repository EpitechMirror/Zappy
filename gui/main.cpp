/*
** EPITECH PROJECT, 2025
** Zappy GUI
** File description:
** main
*/

#include "raylib.h"
#include "client/Client.hpp"
#include "renderer/Renderer.hpp"

constexpr int SCREEN_WIDTH = 1280;
constexpr int SCREEN_HEIGHT = 720;

void printUsage(const char* progName) {
    std::cout << "Usage: " << progName << " -p <port> -h <host>" << std::endl;
}

void printData(const Map &map) {
    std::cout << "Map Size: " << map.getWidth() << " x " << map.getHeight() << std::endl;
    std::cout << "Food Count: " << map.getFoodCount() << std::endl;
    std::cout << "Linemate Count: " << map.getLinemateCount() << std::endl;
    std::cout << "Deraumere Count: " << map.getDeraumereCount() << std::endl;
    std::cout << "Sibur Count: " << map.getSiburCount() << std::endl;
    std::cout << "Mendiane Count: " << map.getMendianeCount() << std::endl;
    std::cout << "Phiras Count: " << map.getPhirasCount() << std::endl;
    std::cout << "Thystame Count: " << map.getThystameCount() << std::endl;
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

    try {
        Client client(host, port);

        if (!client.connectToServer()) {
            std::cerr << "Connection to server failed." << std::endl;
            return EXIT_FAILURE;
        }
        std::cout << "===================ZAPPY GUI===================" << std::endl;
        std::cout << "Connected to server " << host << ":" << port << std::endl;
        std::cout << "===============================================" << std::endl;

        if (!client.sendGraphicCommand()) {
            std::cerr << "Failed to send GRAPHIC command." << std::endl;
            return EXIT_FAILURE;
        }

        while (!client.isMapReady()) {
            client.update();
        }
        Renderer renderer(SCREEN_WIDTH, SCREEN_HEIGHT, client.getMap());
        renderer.renderWindow(client);

        printData(client.getMap());

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return 0;
}

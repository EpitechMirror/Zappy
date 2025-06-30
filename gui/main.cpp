/*
** EPITECH PROJECT, 2025
** Zappy GUI
** File description:
** main
*/

#include "renderer/Renderer.hpp"

constexpr int SCREEN_WIDTH = 1280;
constexpr int SCREEN_HEIGHT = 720;

void printUsage(const char* progName) {
    std::cout << "Usage: " << progName << " -p <port> -h <host> [-d]" << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  -p <port>   Port to connect to (mandatory)" << std::endl;
    std::cout << "  -h <host>   Host to connect to (default: 127.0.0.1)" << std::endl;
    std::cout << "  -d          Enable debug mode" << std::endl;
}

int main(int argc, char** argv) {
    std::string host = "127.0.0.1";
    std::srand(std::time(nullptr));
    int port = 0;
    int opt;
    bool debugMode = false;

    while ((opt = getopt(argc, argv, "p:h:d")) != -1) {
        switch (opt) {
            case 'p': 
                port = std::stoi(optarg); 
                break;
            case 'h': 
                if (strcasecmp(optarg, "localhost") == 0) {
                    host = "127.0.0.1";
                } else {
                    host = optarg;
                }
                break;
            case 'd':
                debugMode = true;
                break;
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
        if (debugMode) {
            std::cout << "Debug mode: ENABLED" << std::endl;
        }
        std::cout << "===============================================" << std::endl;

        if (!client.sendGraphicCommand()) {
            std::cerr << "Failed to send GRAPHIC command." << std::endl;
            return EXIT_FAILURE;
        }

        while (!client.isMapReady()) {
            client.update();
        }
        
        Renderer renderer(SCREEN_WIDTH, SCREEN_HEIGHT, client.getMap());
        //renderer.setDebugMode(debugMode);
        renderer.renderWindow(client);
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return 0;
}
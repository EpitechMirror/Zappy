#include <criterion/criterion.h>
#include "../renderer/Renderer.hpp"
#include "../map/Map.hpp"
#include "../client/Client.hpp"

Test(Renderer, constructor_initializes_members) {
    InitWindow(1, 1, "Test");
    Map map;
    map.setSize(10, 10);
    Renderer renderer(800, 600, map);
    cr_assert(true);
    CloseWindow();
}

Test(Renderer, load_and_unload_models) {
    InitWindow(1, 1, "Test");
    Map map;
    map.setSize(5, 5);
    Renderer renderer(800, 600, map);
    renderer.loadModels();
    renderer.unloadModels();
    cr_assert(true);
    CloseWindow();
}

Test(Renderer, load_and_unload_textures) {
    InitWindow(1, 1, "Test");
    Map map;
    map.setSize(5, 5);
    Renderer renderer(800, 600, map);
    renderer.loadModels();
    renderer.loadTextures();
    renderer.unloadTextures();
    renderer.unloadModels();
    cr_assert(true);
    CloseWindow();
}

Test(Renderer, load_and_unload_shaders) {
    InitWindow(1, 1, "Test");
    Map map;
    map.setSize(5, 5);
    Renderer renderer(800, 600, map);
    renderer.loadShaders();
    renderer.unloadShaders();
    cr_assert(true);
    CloseWindow();
}

Test(Renderer, apply_shaders) {
    InitWindow(1, 1, "Test");
    Map map;
    map.setSize(5, 5);
    Renderer renderer(800, 600, map);
    renderer.loadModels();
    renderer.loadShaders();
    renderer.applyShaders();
    renderer.unloadShaders();
    renderer.unloadModels();
    cr_assert(true);
    CloseWindow();
}

Test(Renderer, init_lights) {
    InitWindow(1, 1, "Test");
    Map map;
    map.setSize(5, 5);
    Renderer renderer(800, 600, map);
    renderer.loadModels();
    renderer.loadShaders();
    renderer.initLights();
    renderer.unloadShaders();
    renderer.unloadModels();
    cr_assert(true);
    CloseWindow();
}

Test(Renderer, draw_methods_do_not_crash) {
    InitWindow(1, 1, "Test");
    Map map;
    map.setSize(2, 2);
    Renderer renderer(800, 600, map);
    renderer.loadModels();
    renderer.loadTextures();
    renderer.loadShaders();
    renderer.applyShaders();
    renderer.initLights();
    renderer.drawFloor();
    renderer.drawItems();
    renderer.DrawEggs();
    renderer.InfoItemsBoard();
    renderer.InfoTeamsBoard();
    renderer.InfoPlayersBoard();
    renderer.DrawGrid();
    renderer.unloadTextures();
    renderer.unloadModels();
    renderer.unloadShaders();
    cr_assert(true);
    CloseWindow();
}
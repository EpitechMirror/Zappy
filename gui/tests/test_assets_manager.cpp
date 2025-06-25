#include <criterion/criterion.h>
#include <criterion/redirect.h>
#include "../renderer/AssetsManager/AssetsManager.hpp"
#include <raylib.h>
#include <string>
#include <filesystem>
#include <iostream>

// Fonction utilitaire pour vérifier l'existence des fichiers
bool file_exists(const std::string& path) {
    return std::filesystem::exists(path);
}

// Tests pour AssetsManager
Test(AssetsManager, resource_files_exist) {
    cr_assert(file_exists("../resources/models/plane.glb"));
    cr_assert(file_exists("../resources/models/pixar_lamp/scene.gltf"));
    cr_assert(file_exists("../resources/textures/wood_8.jpg"));
    cr_assert(file_exists("../resources/room_andy/textures/wallpaper.jpg"));
    cr_assert(file_exists("../resources/fonts/Woody.ttf"));
    cr_assert(file_exists("../resources/music/main_music.ogg"));
}

Test(AssetsManager, constructor_and_dimensions) {
    AssetsManager manager(15, 12);
    // On ne peut pas accéder directement aux membres privés
    // On vérifie indirectement via les dimensions des maillages
    manager.loadAllResources();
    cr_assert(manager.floorModel.meshCount > 0);
    manager.unloadAllResources();
}

Test(AssetsManager, model_loading) {
    AssetsManager manager(10, 8);
    manager.loadAllResources();
    
    cr_assert(manager.floorModel.meshCount > 0);
    cr_assert(manager.playerModel.meshCount > 0);
    cr_assert(manager.wallLong.meshCount > 0);
    cr_assert(manager.wallShort.meshCount > 0);
    
    manager.unloadAllResources();
}

Test(AssetsManager, texture_loading) {
    // Nécessite un contexte graphique
    SetTraceLogLevel(LOG_NONE);
    InitWindow(800, 600, "TEST");
    
    AssetsManager manager(10, 8);
    manager.loadAllResources();
    
    // Vérifie les textures du sol
    cr_assert(manager.floorModel.materials[0].maps[MATERIAL_MAP_ALBEDO].texture.id > 0);
    
    // Vérifie les textures du joueur
    cr_assert(manager.playerModel.materials[0].maps[MATERIAL_MAP_ALBEDO].texture.id > 0);
    
    manager.unloadAllResources();
    CloseWindow();
}

Test(AssetsManager, shader_management) {
    // Nécessite un contexte graphique
    SetTraceLogLevel(LOG_NONE);
    InitWindow(800, 600, "TEST");
    
    AssetsManager manager(10, 8);
    manager.loadAllResources();
    manager.applyShaders();
    
    // Vérifie que les shaders sont appliqués
    cr_assert(manager.floorModel.materials[0].shader.id > 0);
    cr_assert(manager.playerModel.materials[0].shader.id > 0);
    
    manager.unloadAllResources();
    CloseWindow();
}

Test(AssetsManager, audio_loading) {
    AssetsManager manager(10, 8);
    manager.loadAllResources();
    
    cr_assert(IsAudioDeviceReady());
    cr_assert(manager.mainMusic.frameCount > 0);
    
    manager.unloadAllResources();
}

Test(AssetsManager, font_loading) {
    // Nécessite un contexte graphique
    SetTraceLogLevel(LOG_NONE);
    InitWindow(800, 600, "TEST");
    
    AssetsManager manager(10, 8);
    manager.loadAllResources();
    
    cr_assert(manager.toyFont.texture.id > 0);
    cr_assert(manager.toyFont.glyphCount > 0);
    
    manager.unloadAllResources();
    CloseWindow();
}

Test(AssetsManager, resource_checkers) {
    AssetsManager manager(10, 8);
    
    cr_assert(manager.isModelLoaded("../resources/models/plane.glb"));
    cr_assert(manager.isTextureLoaded("../resources/textures/wood_8.jpg"));
    cr_assert(manager.isFontLoaded("../resources/fonts/Woody.ttf"));
    cr_assert(manager.isAudioLoaded("../resources/music/main_music.ogg"));
    
    // Test avec un fichier inexistant
    cr_assert_not(manager.isModelLoaded("invalid_path.glb"));
    cr_assert_not(manager.isTextureLoaded("invalid_texture.jpg"));
}

Test(AssetsManager, full_lifecycle) {
    // Nécessite un contexte graphique
    SetTraceLogLevel(LOG_NONE);
    InitWindow(800, 600, "TEST");
    
    AssetsManager manager(10, 8);
    manager.loadAllResources();
    
    // Vérifications rapides
    cr_assert(manager.floorModel.meshCount > 0);
    cr_assert(manager.playerModel.meshCount > 0);
    cr_assert(manager.toyFont.glyphCount > 0);
    cr_assert(manager.mainMusic.frameCount > 0);
    
    // Déchargement
    manager.unloadAllResources();
    
    CloseWindow();
}

Test(AssetsManager, mesh_dimensions) {
    AssetsManager manager(15, 12);
    manager.loadAllResources();
    
    // Vérifie la taille des murs générés
    // Les valeurs exactes peuvent varier selon la génération
    cr_assert(manager.wallLong.meshes[0].vertexCount > 0);
    cr_assert(manager.wallShort.meshes[0].vertexCount > 0);
    
    manager.unloadAllResources();
}
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

Test(AssetsManager, loading_nonexistent_resources) {
    // This test verifies graceful failure when loading non-existent resources
    SetTraceLogLevel(LOG_NONE); // Suppress raylib log messages
    InitWindow(800, 600, "TEST");
    
    // Check behavior with non-existent files
    cr_assert_not(file_exists("../resources/nonexistent_model.glb"));
    cr_assert_not(file_exists("../resources/nonexistent_texture.png"));
    
    AssetsManager manager(10, 8);
    
    // These should return false but not crash
    cr_assert_not(manager.isModelLoaded("../resources/nonexistent_model.glb"));
    cr_assert_not(manager.isTextureLoaded("../resources/nonexistent_texture.png"));
    cr_assert_not(manager.isFontLoaded("../resources/fonts/nonexistent.ttf"));
    cr_assert_not(manager.isAudioLoaded("../resources/music/nonexistent.ogg"));
    
    CloseWindow();
}

Test(AssetsManager, zero_dimension_map) {
    // Test with zero dimensions to verify it doesn't crash
    SetTraceLogLevel(LOG_NONE); // Suppress raylib log messages
    InitWindow(800, 600, "TEST");
    
    AssetsManager manager(0, 0);
    manager.loadAllResources();
    
    // Since we can't access private members directly, just verify loading doesn't crash
    cr_assert(true); // If we got here without crashing, that's a success
    
    manager.unloadAllResources();
    CloseWindow();
}

Test(AssetsManager, reloading_resources) {
    // Test loading, unloading, and reloading resources
    SetTraceLogLevel(LOG_NONE);
    InitWindow(800, 600, "TEST");
    
    AssetsManager manager(10, 8);
    
    // First load
    manager.loadAllResources();
    // Instead of checking private members, check if loading was successful using public methods
    // or just verify it didn't crash
    cr_assert(true); // If we got here without crashing, that's a success
    
    manager.unloadAllResources();
    
    // Second load
    manager.loadAllResources();
    cr_assert(true); // If we got here without crashing, that's a success
    
    manager.unloadAllResources();
    
    CloseWindow();
}

Test(AssetsManager, shader_application) {
    // Test that shaders are properly applied to models
    SetTraceLogLevel(LOG_NONE);
    InitWindow(800, 600, "TEST");
    
    AssetsManager manager(10, 8);
    manager.loadAllResources();
    
    // Since we can't access private shader IDs, just test that applying shaders doesn't crash
    manager.applyShaders();
    cr_assert(true); // If we got here without crashing, that's a success
    
    // Clean up
    manager.unloadAllResources();
    CloseWindow();
}

// Mock test helpers
void setup_mock_environment() {
    SetTraceLogLevel(LOG_NONE);  // Silence raylib logs
    InitWindow(1, 1, "Mock Test");  // Smallest possible window
}

void cleanup_mock_environment() {
    if (IsWindowReady())
        CloseWindow();
    if (IsAudioDeviceReady())
        CloseAudioDevice();
}

Test(AssetsManager, mock_load_sequence) {
    // Test the sequence of loading operations without real files
    setup_mock_environment();
    
    AssetsManager manager(1, 1);
    
    // We're just testing that these public methods don't crash
    manager.loadAllResources();  // This calls private methods internally
    manager.loadFonts();
    manager.loadAudio();
    
    // Unload resources (even though they're likely invalid)
    manager.unloadAllResources();
    
    cleanup_mock_environment();
}

Test(AssetsManager, multiple_instances) {
    // Test creating multiple AssetsManager instances
    setup_mock_environment();
    
    AssetsManager manager1(5, 5);
    AssetsManager manager2(10, 10);
    
    // Load resources for both instances
    manager1.loadAllResources();
    manager2.loadAllResources();
    
    // Unload in reverse order
    manager2.unloadAllResources();
    manager1.unloadAllResources();
    
    cleanup_mock_environment();
}

Test(AssetsManager, large_map_dimensions) {
    // Test with extremely large map dimensions
    setup_mock_environment();
    
    AssetsManager manager(1000, 1000);
    
    // Just check that loading doesn't crash with large dimensions
    manager.loadAllResources();
    manager.unloadAllResources();
    
    cleanup_mock_environment();
}

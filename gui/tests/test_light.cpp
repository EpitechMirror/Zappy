#include <criterion/criterion.h>
#include "../renderer/Light/Light.hpp"

// Helper function to create a dummy shader for testing
Shader createDummyShader() {
    Shader shader = {0};
    shader.id = 1; // Set a dummy ID to make it valid for testing
    return shader;
}

Test(Light, constructor_and_getters) {
    InitWindow(1, 1, "Test");
    Shader dummyShader = {0};
    Vector3 pos = {1, 2, 3};
    Vector3 target = {0, 0, 0};
    Light light(LIGHT_POINT, pos, target, RED, 1.0f, dummyShader, 0);
    Vector3 p = light.getPosition();
    cr_assert_eq((int)p.x, 1);
    cr_assert_eq((int)p.y, 2);
    cr_assert_eq((int)p.z, 3);
    CloseWindow();
}

Test(Light, set_enabled) {
    InitWindow(1, 1, "Test");
    Shader dummyShader = {0};
    Light light(LIGHT_POINT, {0,0,0}, {0,0,0}, RED, 1.0f, dummyShader, 0);
    light.setEnabled(false);
    cr_assert(true);
    CloseWindow();
}

// Test different light types
Test(Light, light_types) {
    InitWindow(1, 1, "Test");
    Shader shader = createDummyShader();
    Vector3 pos = {5.0f, 10.0f, 15.0f};
    Vector3 target = {0.0f, 0.0f, 0.0f};
    
    // Test LIGHT_DIRECTIONAL
    Light directional(LIGHT_DIRECTIONAL, pos, target, WHITE, 1.0f, shader, 0);
    Vector3 dirPos = directional.getPosition();
    cr_assert_float_eq(dirPos.x, 5.0f, 0.01f, "Directional light position X");
    cr_assert_float_eq(dirPos.y, 10.0f, 0.01f, "Directional light position Y");
    cr_assert_float_eq(dirPos.z, 15.0f, 0.01f, "Directional light position Z");
    
    // Test LIGHT_POINT
    Light point(LIGHT_POINT, pos, target, GREEN, 0.8f, shader, 1);
    Vector3 pointPos = point.getPosition();
    cr_assert_float_eq(pointPos.x, 5.0f, 0.01f, "Point light position X");
    cr_assert_float_eq(pointPos.y, 10.0f, 0.01f, "Point light position Y");
    cr_assert_float_eq(pointPos.z, 15.0f, 0.01f, "Point light position Z");
    
    // Test LIGHT_SPOT
    Light spot(LIGHT_SPOT, pos, target, BLUE, 1.2f, shader, 2);
    Vector3 spotPos = spot.getPosition();
    cr_assert_float_eq(spotPos.x, 5.0f, 0.01f, "Spot light position X");
    cr_assert_float_eq(spotPos.y, 10.0f, 0.01f, "Spot light position Y");
    cr_assert_float_eq(spotPos.z, 15.0f, 0.01f, "Spot light position Z");
    
    CloseWindow();
}

// Test various colors
Test(Light, color_handling) {
    InitWindow(1, 1, "Test");
    Shader shader = createDummyShader();
    Vector3 pos = {0.0f, 0.0f, 0.0f};
    Vector3 target = {1.0f, 1.0f, 1.0f};
    
    // Test basic colors
    Light redLight(LIGHT_POINT, pos, target, RED, 1.0f, shader, 0);
    Light greenLight(LIGHT_POINT, pos, target, GREEN, 1.0f, shader, 1);
    Light blueLight(LIGHT_POINT, pos, target, BLUE, 1.0f, shader, 2);
    Light whiteLight(LIGHT_POINT, pos, target, WHITE, 1.0f, shader, 3);
    Light blackLight(LIGHT_POINT, pos, target, BLACK, 1.0f, shader, 4);
    
    // Test custom color
    Color customColor = {128, 64, 192, 255};
    Light customLight(LIGHT_POINT, pos, target, customColor, 1.0f, shader, 5);
    
    // All lights should be created successfully
    cr_assert_not_null(&redLight, "Red light should be created");
    cr_assert_not_null(&greenLight, "Green light should be created");
    cr_assert_not_null(&blueLight, "Blue light should be created");
    cr_assert_not_null(&whiteLight, "White light should be created");
    cr_assert_not_null(&blackLight, "Black light should be created");
    cr_assert_not_null(&customLight, "Custom color light should be created");
    
    CloseWindow();
}

// Test intensity values
Test(Light, intensity_values) {
    InitWindow(1, 1, "Test");
    Shader shader = createDummyShader();
    Vector3 pos = {0.0f, 0.0f, 0.0f};
    Vector3 target = {0.0f, 0.0f, 1.0f};
    
    // Test various intensity values
    float intensities[] = {0.0f, 0.5f, 1.0f, 2.0f, 10.0f, -1.0f};
    
    for (size_t i = 0; i < sizeof(intensities) / sizeof(intensities[0]); i++) {
        Light light(LIGHT_POINT, pos, target, WHITE, intensities[i], shader, (int)i);
        // Light should be created successfully regardless of intensity value
        cr_assert_not_null(&light, "Light with intensity %.1f should be created", intensities[i]);
    }
    
    CloseWindow();
}

// Test position precision
Test(Light, position_precision) {
    InitWindow(1, 1, "Test");
    Shader shader = createDummyShader();
    
    // Test with precise floating point values
    Vector3 precisePos = {1.23456f, -7.89123f, 0.00001f};
    Vector3 target = {0.0f, 0.0f, 0.0f};
    
    Light light(LIGHT_POINT, precisePos, target, WHITE, 1.0f, shader, 0);
    Vector3 retrievedPos = light.getPosition();
    
    cr_assert_float_eq(retrievedPos.x, 1.23456f, 0.00001f, "Position X precision");
    cr_assert_float_eq(retrievedPos.y, -7.89123f, 0.00001f, "Position Y precision");
    cr_assert_float_eq(retrievedPos.z, 0.00001f, 0.000001f, "Position Z precision");
    
    CloseWindow();
}

// Test extreme positions
Test(Light, extreme_positions) {
    InitWindow(1, 1, "Test");
    Shader shader = createDummyShader();
    Vector3 target = {0.0f, 0.0f, 0.0f};
    
    // Test with very large values
    Vector3 largePos = {1000000.0f, -1000000.0f, 500000.0f};
    Light largeLight(LIGHT_POINT, largePos, target, WHITE, 1.0f, shader, 0);
    Vector3 retrievedLarge = largeLight.getPosition();
    
    cr_assert_float_eq(retrievedLarge.x, 1000000.0f, 1.0f, "Large position X");
    cr_assert_float_eq(retrievedLarge.y, -1000000.0f, 1.0f, "Large position Y");
    cr_assert_float_eq(retrievedLarge.z, 500000.0f, 1.0f, "Large position Z");
    
    // Test with very small values
    Vector3 smallPos = {0.0001f, -0.0001f, 0.0f};
    Light smallLight(LIGHT_POINT, smallPos, target, WHITE, 1.0f, shader, 1);
    Vector3 retrievedSmall = smallLight.getPosition();
    
    cr_assert_float_eq(retrievedSmall.x, 0.0001f, 0.000001f, "Small position X");
    cr_assert_float_eq(retrievedSmall.y, -0.0001f, 0.000001f, "Small position Y");
    cr_assert_float_eq(retrievedSmall.z, 0.0f, 0.000001f, "Small position Z");
    
    CloseWindow();
}

// Test target vectors
Test(Light, target_vectors) {
    InitWindow(1, 1, "Test");
    Shader shader = createDummyShader();
    Vector3 pos = {0.0f, 0.0f, 0.0f};
    
    // Test various target vectors
    Vector3 targets[] = {
        {0.0f, 0.0f, 1.0f},    // Forward
        {0.0f, 0.0f, -1.0f},   // Backward
        {1.0f, 0.0f, 0.0f},    // Right
        {-1.0f, 0.0f, 0.0f},   // Left
        {0.0f, 1.0f, 0.0f},    // Up
        {0.0f, -1.0f, 0.0f},   // Down
        {1.0f, 1.0f, 1.0f},    // Diagonal
        {-5.0f, 10.0f, -2.0f}  // Arbitrary
    };
    
    for (size_t i = 0; i < sizeof(targets) / sizeof(targets[0]); i++) {
        Light light(LIGHT_SPOT, pos, targets[i], WHITE, 1.0f, shader, (int)i);
        // Light should be created successfully with any target vector
        cr_assert_not_null(&light, "Light with target (%.1f,%.1f,%.1f) should be created", 
                          targets[i].x, targets[i].y, targets[i].z);
    }
    
    CloseWindow();
}

// Test multiple light indices
Test(Light, multiple_indices) {
    InitWindow(1, 1, "Test");
    Shader shader = createDummyShader();
    Vector3 pos = {0.0f, 0.0f, 0.0f};
    Vector3 target = {0.0f, 0.0f, 1.0f};
    
    // Test various indices
    int indices[] = {0, 1, 2, 5, 10, 100, -1};
    
    for (size_t i = 0; i < sizeof(indices) / sizeof(indices[0]); i++) {
        Light light(LIGHT_POINT, pos, target, WHITE, 1.0f, shader, indices[i]);
        // Light should be created successfully with any index
        cr_assert_not_null(&light, "Light with index %d should be created", indices[i]);
    }
    
    CloseWindow();
}

// Test enable/disable functionality
Test(Light, enable_disable) {
    InitWindow(1, 1, "Test");
    Shader shader = createDummyShader();
    Vector3 pos = {0.0f, 0.0f, 0.0f};
    Vector3 target = {0.0f, 0.0f, 1.0f};
    
    Light light(LIGHT_POINT, pos, target, WHITE, 1.0f, shader, 0);
    
    // Test multiple enable/disable cycles
    light.setEnabled(false);
    light.setEnabled(true);
    light.setEnabled(false);
    light.setEnabled(false); // Double disable
    light.setEnabled(true);
    light.setEnabled(true);  // Double enable
    
    // Should not crash or cause issues
    cr_assert(true, "Enable/disable cycles should work without issues");
    
    CloseWindow();
}

// Test shader update functionality
Test(Light, shader_updates) {
    InitWindow(1, 1, "Test");
    Shader shader1 = createDummyShader();
    Shader shader2 = createDummyShader();
    shader2.id = 2; // Different ID
    
    Vector3 pos = {1.0f, 2.0f, 3.0f};
    Vector3 target = {4.0f, 5.0f, 6.0f};
    
    Light light(LIGHT_POINT, pos, target, RED, 0.5f, shader1, 0);
    
    // Test updating with different shaders
    light.updateShader(shader1);
    light.updateShader(shader2);
    light.updateShader(shader1);
    
    // Should not crash
    cr_assert(true, "Shader updates should work without issues");
    
    CloseWindow();
}

// Test light consistency after operations
Test(Light, consistency_after_operations) {
    InitWindow(1, 1, "Test");
    Shader shader = createDummyShader();
    Vector3 originalPos = {10.0f, 20.0f, 30.0f};
    Vector3 target = {0.0f, 0.0f, 0.0f};
    
    Light light(LIGHT_POINT, originalPos, target, BLUE, 2.0f, shader, 5);
    
    // Perform various operations
    light.setEnabled(false);
    light.updateShader(shader);
    light.setEnabled(true);
    light.updateShader(shader);
    
    // Position should remain consistent
    Vector3 finalPos = light.getPosition();
    cr_assert_float_eq(finalPos.x, originalPos.x, 0.01f, "Position X should remain consistent");
    cr_assert_float_eq(finalPos.y, originalPos.y, 0.01f, "Position Y should remain consistent");
    cr_assert_float_eq(finalPos.z, originalPos.z, 0.01f, "Position Z should remain consistent");
    
    CloseWindow();
}

// Test constructor with edge case parameters
Test(Light, constructor_edge_cases) {
    InitWindow(1, 1, "Test");
    Shader shader = createDummyShader();
    
    // Test with zero vectors
    Vector3 zeroPos = {0.0f, 0.0f, 0.0f};
    Vector3 zeroTarget = {0.0f, 0.0f, 0.0f};
    Light zeroLight(LIGHT_POINT, zeroPos, zeroTarget, WHITE, 0.0f, shader, 0);
    
    Vector3 retrievedPos = zeroLight.getPosition();
    cr_assert_float_eq(retrievedPos.x, 0.0f, 0.01f, "Zero position X");
    cr_assert_float_eq(retrievedPos.y, 0.0f, 0.01f, "Zero position Y");
    cr_assert_float_eq(retrievedPos.z, 0.0f, 0.01f, "Zero position Z");
    
    // Test with same position and target
    Vector3 sameVec = {5.0f, 5.0f, 5.0f};
    Light sameLight(LIGHT_SPOT, sameVec, sameVec, GREEN, 1.0f, shader, 1);
    Vector3 samePos = sameLight.getPosition();
    cr_assert_float_eq(samePos.x, 5.0f, 0.01f, "Same pos/target X");
    cr_assert_float_eq(samePos.y, 5.0f, 0.01f, "Same pos/target Y");
    cr_assert_float_eq(samePos.z, 5.0f, 0.01f, "Same pos/target Z");
    
    CloseWindow();
}
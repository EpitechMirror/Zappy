#include <criterion/criterion.h>
#include <cmath>
#include "../renderer/Camera/Camera.hpp"

Test(CameraController, constructor_with_map_size) {
    const float mapSize = 20.0f;
    CameraController cam(mapSize, mapSize);
    Camera& c = cam.getCamera();
    
    const float expectedTargetX = mapSize * 0.5f;
    const float expectedTargetZ = mapSize * 0.5f;
    const float rad = 45.0f * DEG2RAD;
    const float expectedPosX = expectedTargetX + cosf(rad) * 15.0f;
    const float expectedPosZ = expectedTargetZ + sinf(rad) * 15.0f;
    const float expectedHeight = 10.0f;

    cr_assert_float_eq(c.position.x, expectedPosX, 0.01f, "Position X incorrecte");
    cr_assert_float_eq(c.position.y, expectedHeight, 0.01f, "Position Y incorrecte");
    cr_assert_float_eq(c.position.z, expectedPosZ, 0.01f, "Position Z incorrecte");
    cr_assert_float_eq(c.target.x, expectedTargetX, 0.01f, "Target X incorrecte");
    cr_assert_float_eq(c.target.y, 0.0f, 0.01f, "Target Y incorrecte");
    cr_assert_float_eq(c.target.z, expectedTargetZ, 0.01f, "Target Z incorrecte");
}

Test(CameraController, constructor_zero_size) {
    CameraController cam(0.0f, 0.0f);
    Camera& c = cam.getCamera();
    
    cr_assert_float_eq(c.target.x, 0.0f, 0.01f, "Target X devrait être 0");
    cr_assert_float_eq(c.target.z, 0.0f, 0.01f, "Target Z devrait être 0");
    cr_assert_float_eq(c.position.y, 10.0f, 0.01f, "Hauteur devrait être 10");
}

Test(CameraController, constructor_rectangular_map) {
    const float width = 30.0f;
    const float depth = 10.0f;
    CameraController cam(width, depth);
    Camera& c = cam.getCamera();
    
    cr_assert_float_eq(c.target.x, width * 0.5f, 0.01f, "Target X incorrecte");
    cr_assert_float_eq(c.target.z, depth * 0.5f, 0.01f, "Target Z incorrecte");
}

Test(CameraController, camera_movement_rotation) {
    CameraController cam(20.0f, 20.0f);
    Camera& c = cam.getCamera();
    
    // Position initiale
    const float initialAngle = 45.0f;
    const float initialRad = initialAngle * DEG2RAD;
    const float initialX = c.position.x;
    const float initialZ = c.position.z;
    
    // Simuler une rotation
    cam._angleDeg = 90.0f; // Rotation à 90 degrés
    cam.update(); // Mise à jour sans frame time
    
    const float newRad = 90.0f * DEG2RAD;
    const float expectedX = cam._target.x + cosf(newRad) * cam._distance;
    const float expectedZ = cam._target.z + sinf(newRad) * cam._distance;
    
    cr_assert_float_eq(c.position.x, expectedX, 0.01f, "Position X après rotation incorrecte");
    cr_assert_float_eq(c.position.z, expectedZ, 0.01f, "Position Z après rotation incorrecte");
}

Test(CameraController, camera_movement_zoom) {
    CameraController cam(20.0f, 20.0f);
    Camera& c = cam.getCamera();
    
    // Position initiale
    const float initialDistance = cam._distance;
    const float initialX = c.position.x;
    const float initialZ = c.position.z;
    
    // Simuler un zoom
    cam._distance = 5.0f;
    cam.update();
    
    const float rad = cam._angleDeg * DEG2RAD;
    const float expectedX = cam._target.x + cosf(rad) * 5.0f;
    const float expectedZ = cam._target.z + sinf(rad) * 5.0f;
    
    cr_assert_float_eq(c.position.x, expectedX, 0.01f, "Position X après zoom incorrecte");
    cr_assert_float_eq(c.position.z, expectedZ, 0.01f, "Position Z après zoom incorrecte");
}

Test(CameraController, distance_clamping) {
    CameraController cam(20.0f, 20.0f);
    
    // Tester la borne inférieure (should be 2.0f based on header file)
    cam._distance = 0.0f;
    cam.update();
    cr_assert_float_eq(cam._distance, 2.0f, 0.01f, "Distance devrait être clampée au minimum");
    
    // Tester la borne supérieure (should be 50.0f based on header file)
    cam._distance = 100.0f;
    cam.update();
    cr_assert_float_eq(cam._distance, 50.0f, 0.01f, "Distance devrait être clampée au maximum");
}

Test(CameraController, camera_height) {
    CameraController cam(20.0f, 20.0f);
    Camera& c = cam.getCamera();
    
    // Modifier la hauteur
    cam._height = 15.0f;
    cam.update();
    
    cr_assert_float_eq(c.position.y, 15.0f, 0.01f, "Hauteur de la caméra incorrecte");
}

// Test camera configuration consistency
Test(CameraController, camera_configuration) {
    CameraController cam(10.0f, 15.0f);
    Camera& c = cam.getCamera();
    
    // Test camera configuration
    cr_assert_eq(c.projection, CAMERA_PERSPECTIVE, "Should use perspective projection");
    cr_assert_float_eq(c.fovy, 45.0f, 0.01f, "Field of view should be 45 degrees");
    
    // Test up vector
    cr_assert_float_eq(c.up.x, 0.0f, 0.01f, "Up vector X");
    cr_assert_float_eq(c.up.y, 1.0f, 0.01f, "Up vector Y");
    cr_assert_float_eq(c.up.z, 0.0f, 0.01f, "Up vector Z");
    
    // Test initial values
    cr_assert_float_eq(cam._angleDeg, 45.0f, 0.01f, "Initial angle should be 45 degrees");
    cr_assert_float_eq(cam._distance, 15.0f, 0.01f, "Initial distance should be 15");
    cr_assert_float_eq(cam._height, 10.0f, 0.01f, "Initial height should be 10");
}

// Test angle normalization
Test(CameraController, angle_normalization) {
    CameraController cam(10.0f, 10.0f);
    Camera& c = cam.getCamera();
    
    // Test with various angles
    float testAngles[] = {0.0f, 90.0f, 180.0f, 270.0f, 360.0f, -90.0f, 450.0f, -180.0f};
    
    for (size_t i = 0; i < sizeof(testAngles) / sizeof(testAngles[0]); i++) {
        cam._angleDeg = testAngles[i];
        cam.update();
        
        float rad = testAngles[i] * DEG2RAD;
        float expectedX = cam._target.x + cosf(rad) * cam._distance;
        float expectedZ = cam._target.z + sinf(rad) * cam._distance;
        
        cr_assert_float_eq(c.position.x, expectedX, 0.01f, 
            "Position X incorrect for angle %.1f", testAngles[i]);
        cr_assert_float_eq(c.position.z, expectedZ, 0.01f, 
            "Position Z incorrect for angle %.1f", testAngles[i]);
        cr_assert_float_eq(c.position.y, cam._height, 0.01f, 
            "Height should remain constant");
    }
}

// Test extreme map sizes
Test(CameraController, extreme_map_sizes) {
    // Very large map
    CameraController largeCam(1000.0f, 1000.0f);
    Camera& largeC = largeCam.getCamera();
    
    cr_assert_float_eq(largeC.target.x, 500.0f, 0.01f, "Large map target X");
    cr_assert_float_eq(largeC.target.z, 500.0f, 0.01f, "Large map target Z");
    
    // Very small map
    CameraController smallCam(0.1f, 0.1f);
    Camera& smallC = smallCam.getCamera();
    
    cr_assert_float_eq(smallC.target.x, 0.05f, 0.01f, "Small map target X");
    cr_assert_float_eq(smallC.target.z, 0.05f, 0.01f, "Small map target Z");
    
    // Negative map sizes (edge case)
    CameraController negCam(-10.0f, -5.0f);
    Camera& negC = negCam.getCamera();
    
    cr_assert_float_eq(negC.target.x, -5.0f, 0.01f, "Negative map target X");
    cr_assert_float_eq(negC.target.z, -2.5f, 0.01f, "Negative map target Z");
}

// Test distance boundary conditions more accurately
Test(CameraController, distance_boundaries_accurate) {
    CameraController cam(20.0f, 20.0f);
    
    // Test minimum distance (should be 2.0f based on header)
    cam._distance = 0.5f;
    cam.update();
    cr_assert_float_eq(cam._distance, 2.0f, 0.01f, "Distance should be clamped to minimum 2.0");
    
    // Test maximum distance (should be 50.0f based on header)
    cam._distance = 100.0f;
    cam.update();
    cr_assert_float_eq(cam._distance, 50.0f, 0.01f, "Distance should be clamped to maximum 50.0");
    
    // Test exact boundaries
    cam._distance = 2.0f;
    cam.update();
    cr_assert_float_eq(cam._distance, 2.0f, 0.01f, "Distance at minimum boundary");
    
    cam._distance = 50.0f;
    cam.update();
    cr_assert_float_eq(cam._distance, 50.0f, 0.01f, "Distance at maximum boundary");
    
    // Test just inside boundaries
    cam._distance = 2.1f;
    cam.update();
    cr_assert_float_eq(cam._distance, 2.1f, 0.01f, "Distance just above minimum");
    
    cam._distance = 49.9f;
    cam.update();
    cr_assert_float_eq(cam._distance, 49.9f, 0.01f, "Distance just below maximum");
}

// Test target consistency
Test(CameraController, target_consistency) {
    CameraController cam(15.0f, 25.0f);
    Camera& c = cam.getCamera();
    
    Vector3 originalTarget = cam._target;
    
    // Modify camera properties and update
    cam._angleDeg = 120.0f;
    cam._distance = 8.0f;
    cam._height = 20.0f;
    cam.update();
    
    // Target should remain the same
    cr_assert_float_eq(c.target.x, originalTarget.x, 0.01f, "Target X should not change");
    cr_assert_float_eq(c.target.y, originalTarget.y, 0.01f, "Target Y should not change");
    cr_assert_float_eq(c.target.z, originalTarget.z, 0.01f, "Target Z should not change");
    
    // But position should change
    float rad = 120.0f * DEG2RAD;
    float expectedX = originalTarget.x + cosf(rad) * 8.0f;
    float expectedZ = originalTarget.z + sinf(rad) * 8.0f;
    
    cr_assert_float_eq(c.position.x, expectedX, 0.01f, "Position X should update");
    cr_assert_float_eq(c.position.z, expectedZ, 0.01f, "Position Z should update");
    cr_assert_float_eq(c.position.y, 20.0f, 0.01f, "Position Y should update");
}

// Test height modifications
Test(CameraController, height_modifications) {
    CameraController cam(10.0f, 10.0f);
    Camera& c = cam.getCamera();
    
    float testHeights[] = {0.0f, 5.0f, 10.0f, 20.0f, 100.0f, -5.0f};
    
    for (size_t i = 0; i < sizeof(testHeights) / sizeof(testHeights[0]); i++) {
        cam._height = testHeights[i];
        cam.update();
        
        cr_assert_float_eq(c.position.y, testHeights[i], 0.01f, 
            "Camera height should be %.1f", testHeights[i]);
        
        // X and Z should not be affected by height changes
        float rad = cam._angleDeg * DEG2RAD;
        float expectedX = cam._target.x + cosf(rad) * cam._distance;
        float expectedZ = cam._target.z + sinf(rad) * cam._distance;
        
        cr_assert_float_eq(c.position.x, expectedX, 0.01f, 
            "X position should not change with height");
        cr_assert_float_eq(c.position.z, expectedZ, 0.01f, 
            "Z position should not change with height");
    }
}

// Test mathematical precision and consistency
Test(CameraController, mathematical_precision) {
    CameraController cam(10.0f, 10.0f);
    Camera& c = cam.getCamera();
    
    // Test that position calculation is consistent
    float angles[] = {0.0f, 30.0f, 45.0f, 60.0f, 90.0f, 135.0f, 180.0f, 225.0f, 270.0f, 315.0f};
    float distances[] = {2.0f, 5.0f, 10.0f, 25.0f, 50.0f};
    
    for (size_t i = 0; i < sizeof(angles) / sizeof(angles[0]); i++) {
        for (size_t j = 0; j < sizeof(distances) / sizeof(distances[0]); j++) {
            cam._angleDeg = angles[i];
            cam._distance = distances[j];
            cam.update();
            
            float rad = angles[i] * DEG2RAD;
            float expectedX = cam._target.x + cosf(rad) * distances[j];
            float expectedZ = cam._target.z + sinf(rad) * distances[j];
            
            cr_assert_float_eq(c.position.x, expectedX, 0.001f, 
                "Precision error at angle %.1f, distance %.1f (X)", angles[i], distances[j]);
            cr_assert_float_eq(c.position.z, expectedZ, 0.001f, 
                "Precision error at angle %.1f, distance %.1f (Z)", angles[i], distances[j]);
        }
    }
}

// Test circular motion consistency
Test(CameraController, circular_motion) {
    CameraController cam(10.0f, 10.0f);
    Camera& c = cam.getCamera();
    
    Vector3 center = cam._target;
    float distance = cam._distance;
    
    // Test that camera maintains constant distance from target during rotation
    for (float angle = 0.0f; angle <= 360.0f; angle += 15.0f) {
        cam._angleDeg = angle;
        cam.update();
        
        float actualDistance = sqrtf(
            (c.position.x - center.x) * (c.position.x - center.x) +
            (c.position.z - center.z) * (c.position.z - center.z)
        );
        
        cr_assert_float_eq(actualDistance, distance, 0.01f, 
            "Distance from target should remain constant at angle %.1f", angle);
    }
}

// Test asymmetric map handling
Test(CameraController, asymmetric_maps) {
    // Test various width/depth combinations
    struct { float width, depth; } testCases[] = {
        {5.0f, 20.0f},
        {20.0f, 5.0f},
        {1.0f, 100.0f},
        {100.0f, 1.0f},
        {13.7f, 28.3f}
    };
    
    for (size_t i = 0; i < sizeof(testCases) / sizeof(testCases[0]); i++) {
        CameraController cam(testCases[i].width, testCases[i].depth);
        Camera& c = cam.getCamera();
        
        float expectedTargetX = testCases[i].width * 0.5f;
        float expectedTargetZ = testCases[i].depth * 0.5f;
        
        cr_assert_float_eq(c.target.x, expectedTargetX, 0.01f, 
            "Target X for map %.1fx%.1f", testCases[i].width, testCases[i].depth);
        cr_assert_float_eq(c.target.z, expectedTargetZ, 0.01f, 
            "Target Z for map %.1fx%.1f", testCases[i].width, testCases[i].depth);
        
        // Test that camera is positioned correctly relative to the center
        float rad = cam._angleDeg * DEG2RAD;
        float expectedX = expectedTargetX + cosf(rad) * cam._distance;
        float expectedZ = expectedTargetZ + sinf(rad) * cam._distance;
        
        cr_assert_float_eq(c.position.x, expectedX, 0.01f, 
            "Position X for asymmetric map");
        cr_assert_float_eq(c.position.z, expectedZ, 0.01f, 
            "Position Z for asymmetric map");
    }
}

// Test camera state after multiple updates
Test(CameraController, multiple_updates) {
    CameraController cam(10.0f, 10.0f);
    Camera& c = cam.getCamera();
    
    // Store initial state
    Vector3 initialPos = c.position;
    Vector3 initialTarget = c.target;
    
    // Perform multiple updates without changing any parameters
    for (int i = 0; i < 10; i++) {
        cam.update();
    }
    
    // Position and target should remain unchanged
    cr_assert_float_eq(c.position.x, initialPos.x, 0.001f, "Position X should not drift");
    cr_assert_float_eq(c.position.y, initialPos.y, 0.001f, "Position Y should not drift");
    cr_assert_float_eq(c.position.z, initialPos.z, 0.001f, "Position Z should not drift");
    cr_assert_float_eq(c.target.x, initialTarget.x, 0.001f, "Target X should not drift");
    cr_assert_float_eq(c.target.y, initialTarget.y, 0.001f, "Target Y should not drift");
    cr_assert_float_eq(c.target.z, initialTarget.z, 0.001f, "Target Z should not drift");
}

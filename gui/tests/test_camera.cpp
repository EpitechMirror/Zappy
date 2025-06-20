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
    
    // Tester la borne inférieure
    cam._distance = 0.0f;
    cam.update();
    cr_assert_float_eq(cam._distance, 5.0f, 0.01f, "Distance devrait être clampée au minimum");
    
    // Tester la borne supérieure
    cam._distance = 100.0f;
    cam.update();
    cr_assert_float_eq(cam._distance, 30.0f, 0.01f, "Distance devrait être clampée au maximum");
}

Test(CameraController, camera_height) {
    CameraController cam(20.0f, 20.0f);
    Camera& c = cam.getCamera();
    
    // Modifier la hauteur
    cam._height = 15.0f;
    cam.update();
    
    cr_assert_float_eq(c.position.y, 15.0f, 0.01f, "Hauteur de la caméra incorrecte");
}

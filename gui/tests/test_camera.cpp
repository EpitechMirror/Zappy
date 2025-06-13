#include <criterion/criterion.h>
#include "../renderer/Camera/Camera.hpp"

Test(CameraController, default_values) {
    CameraController cam;
    Camera& c = cam.getCamera();
    cr_assert_eq((int)c.position.y, 10);
    cr_assert_eq((int)c.position.z, 10);
    cr_assert_eq((int)c.target.x, 0);
}

Test(CameraController, zoom) {
    CameraController cam;
    float before = cam.getCamera().position.z;
    cam.zoom(2.0f);
    float after = cam.getCamera().position.z;
    cr_assert(before != after);
}
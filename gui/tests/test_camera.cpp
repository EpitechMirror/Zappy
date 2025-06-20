#include <criterion/criterion.h>
#include "../renderer/Camera/Camera.hpp"

Test(CameraController, constructor_with_map_size) {
    CameraController cam(20.0f, 20.0f);
    Camera& c = cam.getCamera();
    cr_assert_eq((int)c.position.y, 10);
    cr_assert_eq((int)c.position.z, 10);
    cr_assert_eq((int)c.target.x, 10);
}

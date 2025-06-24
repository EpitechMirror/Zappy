#include <criterion/criterion.h>
#include "../renderer/Light/Light.hpp"

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
#include <criterion/criterion.h>
#include "../renderer/Shaders/ShadersManager.hpp"

Test(ShadersManager, load_and_unload_pbr) {
    ShadersManager manager;
    manager.loadPBR();
    manager.unloadAll();
    cr_assert(true);
}
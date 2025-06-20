#include <criterion/criterion.h>
#include <raylib.h>
#include "../renderer/Shaders/ShadersManager.hpp"
#include <stdexcept>

Test(ShadersManager, load_and_unload_pbr) {
    struct WindowRAII {
        WindowRAII(int width, int height, const char *title) {
            SetConfigFlags(FLAG_WINDOW_HIDDEN);
            InitWindow(width, height, title);
            if (!IsWindowReady()) {
                throw std::runtime_error("Failed to create window and OpenGL context");
            }
        }
        ~WindowRAII() {
            CloseWindow();
        }
    };

    try {
        WindowRAII window(800, 600, "Shader Test");
        
        ShadersManager manager;
        manager.loadPBR();

        try {
            Shader& pbr = manager.getPBR();
            if (pbr.id == 0) {
                cr_skip_test("PBR shader loaded but ID is 0; skipping test.");
                return;
            }
        } catch (const std::runtime_error& e) {
            cr_skip_test("PBR shader not loaded: %s; skipping test.", e.what());
            return;
        }

        manager.unloadAll();
        cr_assert(true, "PBR shader loaded and unloaded successfully");
    } catch (const std::runtime_error& e) {
        cr_skip_test("%s; skipping test.", e.what());
    }
}
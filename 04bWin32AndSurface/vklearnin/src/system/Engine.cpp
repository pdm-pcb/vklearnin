#include "vklearnin/vklearnin.hpp"
#include "vklearnin/system/Engine.hpp"

#include "vklearnin/system/Application.hpp"

namespace vkl {

// =============================================================================
void Engine::render_loop() {
    using sixty_fps = std::chrono::duration<float, std::ratio<1, 60>>;
    std::this_thread::sleep_for(sixty_fps(60));
}

// =============================================================================
Engine::Engine(Application &app) :
    _application { app }
{ }

} // namespace vkl
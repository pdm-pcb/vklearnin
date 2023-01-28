#include "vklearnin/vklearnin.hpp"
#include "vklearnin/system/Engine.hpp"

#include "vklearnin/system/Application.hpp"
#include "vklearnin/render/swapchain/Swapchain.hpp"

namespace vkl {

// =============================================================================
void Engine::render_loop() {
    Timekeeper::frame_start();

        Swapchain::next_image();
        Swapchain::reset_fence();
        auto image_index = Swapchain::image_index();

        _cmd_pools[image_index].reset();
        _application.run_renderpasses(_cmd_buffers[image_index], image_index);

        Swapchain::present();

    Timekeeper::frame_end();
}

// =============================================================================
void Engine::init() {
    for(uint32_t frame = 0; frame < _cmd_pools.size(); ++frame) {
        _cmd_pools[frame].create();
        _cmd_buffers[frame].allocate(_cmd_pools[frame].native());
    }
}

// =============================================================================
void Engine::shutdown() {
    for(uint32_t frame = 0; frame < _cmd_pools.size(); ++frame) {
        _cmd_buffers[frame].free();
        _cmd_pools[frame].destroy();
    }
}

// =============================================================================
Engine::Engine(Application &app) :
    _application { app }
{
    _cmd_pools.resize(vkl::RenderConfig::image_count);
    _cmd_buffers.resize(vkl::RenderConfig::image_count);
}

} // namespace vkl
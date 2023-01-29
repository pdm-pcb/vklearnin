#include "vklearnin/vklearnin.hpp"
#include "vklearnin/system/Engine.hpp"

#include "vklearnin/system/Application.hpp"
#include "vklearnin/rendering/swapchain/Swapchain.hpp"

namespace vkl {

Renderer::ViewProjMats view_proj_mats {
    .view = {{
        {{ 1.0f,-0.0f,  0.0f, 0.0f }},
        {{ 0.0f, 1.0f,  0.0f, 0.0f }},
        {{ 0.0f, 0.0f, -1.0f, 0.0f }},
        {{ 0.0f, 0.0f,  2.0f, 1.0f }},
    }},
    .proj = {{
        {{ 0.562793f, 0.0f,  0.00000f, 0.0f }},
        {{ 0.000000f, 1.0f,  0.00000f, 0.0f }},
        {{ 0.000000f, 0.0f,  1.00010f, 1.0f }},
        {{ 0.000000f, 0.0f, -0.10001f, 0.0f }},
    }},
};

// =============================================================================
void Engine::render_loop() {
    Timekeeper::frame_start();

        Swapchain::next_image();
        Swapchain::reset_fence();

        _cmd_pools[Swapchain::image_index()].reset();
        auto &cmd_buffer = _cmd_buffers[Swapchain::image_index()];

        const vk::CommandBufferBeginInfo begin_info { };
        auto result = cmd_buffer.native().begin(begin_info);
        if(result != vk::Result::eSuccess) {
            CONSOLE_ERROR(
                "Failed to begin command buffer recording: '{}'",
                to_string(result)
            );
            return;
        }

            _application.submit_draws(_renderer);
            _renderer.render_pass(cmd_buffer.native());

        result = cmd_buffer.native().end();
        if(result != vk::Result::eSuccess) {
            CONSOLE_ERROR(
                "Failed to end command buffer recording: '{}'",
                to_string(result)
            );
            return;
        }

        Swapchain::submit({ cmd_buffer.native() });
        Swapchain::present();

    Timekeeper::frame_end();
}

// =============================================================================
void Engine::init() {
    for(uint32_t frame = 0; frame < _cmd_pools.size(); ++frame) {
        _cmd_pools[frame].create();
        _cmd_buffers[frame].allocate(_cmd_pools[frame].native());
    }

    _renderer.init();
    _renderer.update_view_proj(view_proj_mats);
}

// =============================================================================
void Engine::shutdown() {
    _renderer.shutdown();

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
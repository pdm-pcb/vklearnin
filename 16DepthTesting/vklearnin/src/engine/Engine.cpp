#include "vklearnin/vklearnin.hpp"
#include "vklearnin/engine/Engine.hpp"

#include "vklearnin/system/Application.hpp"
#include "vklearnin/system/TargetWindow.hpp"
#include "vklearnin/engine/Swapchain.hpp"
#include "vklearnin/engine/Pipeline.hpp"
#include "vklearnin/rendering/devices/LogicalDevice.hpp"

using HRC = std::chrono::high_resolution_clock;
using us_period = std::chrono::microseconds::period;
using duration_us = std::chrono::duration<float, us_period>;

namespace vkl {

// =============================================================================
void Engine::render_loop() {
    // Advance the swapchain image index, which will block waiting for an image
    // which has been drawn to screen and released
    auto result = _swapchain->next_image(_frame_index);

    // If one of these two hit, it's because the swapchain images are no longer
    // appropriately sized
    if(result == vk::Result::eErrorOutOfDateKHR ||
       result == vk::Result::eSuboptimalKHR)
    {
        CONSOLE_ERROR("Could not get next image on frame {}", _frame_index);
        _image_invalid();
        return;
    }

    _frame_index = _swapchain->image_index();

    // Un-signal the fence controlling this framebuffer; the GPU will signal
    // when it's done again after we submit this buffer's work
    _swapchain->reset_fence();

    static HRC::time_point frame_end = HRC::now();
    auto time_delta = 1e-6f * duration_us(HRC::now() - frame_end).count();

        const auto &command_buffer = _application.run_pipelines(
            time_delta,
            _frame_index
        );
        _swapchain->submit(command_buffer, LogicalDevice::cmd_queue());
        result = _swapchain->present();

    frame_end = HRC::now();

    // A present operation can return these two, too. Same approach as above -
    // adjust the required stuff and try again
    if(result == vk::Result::eErrorOutOfDateKHR ||
       result == vk::Result::eSuboptimalKHR)
    {
        CONSOLE_ERROR("Could not present frame {}", _frame_index);
        _image_invalid();
        return;
    }    
    
    _next_frame();
}

// =============================================================================
void Engine::init() {
    VKAllocator::init();

    _swapchain = new Swapchain;
    _swapchain->create();

    _pipelines = _application.create_pipelines(*_swapchain);
}

// =============================================================================
void Engine::shutdown() {
    _swapchain->destroy();
    VKAllocator::shutdown();
}

// =============================================================================
void Engine::_image_invalid() {
    // wait for current commands to run their course
    auto result = LogicalDevice::native().waitIdle();
    if(result != vk::Result::eSuccess) {
        CONSOLE_CRITICAL("Failed to wait for idle on image resize.");
    }

    CONSOLE_WARN("Destroy framebuffers");
    for(auto *pipeline : _pipelines) {
        pipeline->destroy_framebuffers();
    }

    CONSOLE_WARN("Destroy swapchain");
    _swapchain->destroy();

    CONSOLE_WARN("Reinitialize window surface");
    TargetWindow::destroy_surface();
    TargetWindow::create_surface();

    CONSOLE_WARN("Recreate swapchain");
    _swapchain->create();

    CONSOLE_WARN("Recreate framebuffers");
    for(auto *pipeline : _pipelines) {
        pipeline->create_framebuffers();
    }

    _application.swapchain_image_invalid();

    _frame_index = 0u;
}

// =============================================================================
void Engine::_next_frame() {
    // Flipping between zero and one without the use of the mod operator.
    // Courtesy paxdiablo: https://stackoverflow.com/a/4084058/1464937
    _frame_index = 1 - _frame_index;
}

// =============================================================================
Engine::Engine(Application &app) :
    _swapchain     { nullptr },
    _frame_index   { 0u },
    _application   { app }
{ }

Engine::~Engine() {
    delete _swapchain;
}

} // namespace vkl
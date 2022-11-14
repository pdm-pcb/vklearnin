#include "vklearnin/vklearnin.hpp"
#include "vklearnin/system/Application.hpp"

#include "vklearnin/rendering/GraphicsInstance.hpp"
#include "vklearnin/system/Win32TargetWindow.hpp"
#include "vklearnin/rendering/PhysicalDevice.hpp"
#include "vklearnin/rendering/LogicalDevice.hpp"
#include "vklearnin/rendering/DeviceQueue.hpp"
#include "vklearnin/rendering/Swapchain.hpp"
#include "vklearnin/rendering/Pipeline.hpp"
#include "vklearnin/rendering/Renderer.hpp"

#include "vklearnin/mesh/XZPlane.hpp"

namespace vkl {

#if defined(__linux__)
    using TargetWindow = XCBTargetWindow;
#elif defined(_WIN32)
    using TargetWindow = Win32TargetWindow;
#endif

// =============================================================================
void Application::init() {
    _graphics_instance = new GraphicsInstance;
    _graphics_instance->init();

    TargetWindow::init();
    TargetWindow::init_surface(_graphics_instance->native());

    _physical_device = new PhysicalDevice;
    _physical_device->init(_graphics_instance->native());
    _physical_device->select_device(TargetWindow::surface());
    PhysicalDevice::set_current(_physical_device);

    _logical_device = new LogicalDevice;
    _logical_device->init();

    _swapchain = new Swapchain(*_logical_device,
                               TargetWindow::surface());
    _swapchain->create();

    _pipeline = new Pipeline(_logical_device->native(), *_swapchain);
    _pipeline->vertex_from_binary(
        "../../vklearnin/assets/shaders/02vertex_buffer.vert-debug.spv"
    );
    _pipeline->fragment_from_binary(
        "../../vklearnin/assets/shaders/01fixed_color.frag-debug.spv"
    );
    _pipeline->init_layout();
    _pipeline->init_render_passes();
    _pipeline->create();

    _renderer = new Renderer(*_graphics_instance, *_logical_device,
                             *_swapchain, *_pipeline);
    _renderer->init();
}

// =============================================================================
void Application::run() {
    XZPlane xzplane(*_logical_device);
    xzplane.create_buffers();

    float runtime = 0.0f;

    while(_running) {
        auto begin = std::chrono::high_resolution_clock::now();
        _renderer->draw(
            xzplane.vertex_buffer(),
            static_cast<uint32_t>(xzplane.vertices().size())
        );
        _renderer->present();
        _running = TargetWindow::message_loop();

        auto duration = std::chrono::high_resolution_clock::now() - begin;
        auto frame_time = std::chrono::duration_cast<std::chrono::microseconds>(duration).count() / 1000.0f;
        runtime += frame_time / 1000.0f;
        
        float rgb_val = (1.0f + sin(-runtime)) * 0.5f;

        xzplane.set_corner_colors({{
            { rgb_val, 0.0f,    0.0f,    1.0f },
            { 0.0f,    rgb_val, 0.0f,    1.0f },
            { 0.0f,    0.0f,    rgb_val, 1.0f },
            { rgb_val, rgb_val, 0.0f,    1.0f }
        }});
    }

    _renderer->shutdown();
    _swapchain->destroy();

    xzplane.destroy_buffers();
}

// =============================================================================
Application::Application() :
    _running           { true    },
    _graphics_instance { nullptr },
    _physical_device   { nullptr },
    _logical_device    { nullptr },
    _swapchain         { nullptr }
{
    ConsoleLog::init();
}

Application::~Application() {
    delete _renderer;
    delete _pipeline;
    delete _swapchain;
    delete _logical_device;
    delete _physical_device;
    TargetWindow::shutdown(_graphics_instance->native());
    delete _graphics_instance;
}

} // namespace vkl
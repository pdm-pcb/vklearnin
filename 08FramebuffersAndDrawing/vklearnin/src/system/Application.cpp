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

    _logical_device = new LogicalDevice(*_physical_device);
    _logical_device->init();

    _swapchain = new Swapchain(*_physical_device, *_logical_device,
                               TargetWindow::surface());
    _swapchain->create();

    _pipeline = new Pipeline(_logical_device->native(), *_swapchain);
    _pipeline->vertex_from_binary(
        "../../vklearnin/assets/shaders/01fixed_color.vert-debug.spv"
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
    while(_running) {
        _renderer->draw();
        _running = TargetWindow::message_loop();
    }

    _renderer->shutdown();
    _swapchain->destroy();
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
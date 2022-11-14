#include "vklearnin/vklearnin.hpp"
#include "vklearnin/system/Application.hpp"

#include "vklearnin/rendering/GraphicsInstance.hpp"
#include "vklearnin/system/Win32TargetWindow.hpp"
#include "vklearnin/rendering/PhysicalDevice.hpp"
#include "vklearnin/rendering/LogicalDevice.hpp"
#include "vklearnin/rendering/DeviceQueue.hpp"
#include "vklearnin/rendering/Swapchain.hpp"

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
}

// =============================================================================
void Application::run() {
    while(_running) {
        _running = TargetWindow::message_loop();
    }

    _swapchain->destroy();
}

// =============================================================================
Application::Application() :
    _running           { true    },
    _graphics_instance { nullptr },
    _physical_device   { nullptr },
    _logical_device    { nullptr }
{
    ConsoleLog::init();
}

Application::~Application() {
    delete _swapchain;
    delete _logical_device;
    delete _physical_device;
    TargetWindow::shutdown(_graphics_instance->native());
    delete _graphics_instance;
}

} // namespace vkl
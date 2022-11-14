#include "vklearnin/vklearnin.hpp"
#include "vklearnin/system/Application.hpp"

#include "vklearnin/rendering/GraphicsInstance.hpp"
#include "vklearnin/system/Win32TargetWindow.hpp"
#include "vklearnin/rendering/PhysicalDevice.hpp"

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
}

// =============================================================================
void Application::run() {
        while(_running) {
        _running = TargetWindow::message_loop();
    }
}

// =============================================================================
Application::Application() :
    _running           { true    },
    _graphics_instance { nullptr },
    _physical_device   { nullptr }
{
    ConsoleLog::init();
}

Application::~Application() {
    delete _physical_device;
    TargetWindow::shutdown(_graphics_instance->native());
    delete _graphics_instance;
}

} // namespace vkl
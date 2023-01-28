#include "vklearnin/vklearnin.hpp"
#include "vklearnin/system/Application.hpp"

#include "vklearnin/system/Engine.hpp"
#include "vklearnin/system/window/TargetWindow.hpp"
#include "vklearnin/render/swapchain/Swapchain.hpp"
#include "vklearnin/system/devices/LogicalDevice.hpp"

namespace vkl {

// =============================================================================
void Application::run() {
    _engine->init();
    init();

    while(TargetWindow::message_loop()) {
        _engine->render_loop();
        CONSOLE_TRACE("A frame took: {:.06f}", Timekeeper::frametime());
    }

    auto result = LogicalDevice::native().waitIdle();
    if(result != vk::Result::eSuccess) {
        CONSOLE_CRITICAL(
            "Failed to wait for device idle: '{}'",
            to_string(result)
        );
    }

    shutdown();
    _engine->shutdown();
}

// =============================================================================
Application::Application() :
    _engine { new Engine(*this) }
{
    ConsoleLog::init();
    GraphicsAPI::init();
    TargetWindow::spawn_window();
    TargetWindow::create_surface();
    GraphicsAPI::create_device();
    Swapchain::create();
}

Application::~Application() {
    Swapchain::destroy();
    GraphicsAPI::destroy_device();
    TargetWindow::destroy_surface();
    GraphicsAPI::shutdown();

    delete _engine;
}

} // namespace vkl
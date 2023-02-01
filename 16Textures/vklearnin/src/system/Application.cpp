#include "vklearnin/vklearnin.hpp"
#include "vklearnin/system/Application.hpp"

#include "vklearnin/system/GraphicsAPI.hpp"
#include "vklearnin/system/Engine.hpp"
#include "vklearnin/system/window/TargetWindow.hpp"
#include "vklearnin/rendering/swapchain/Swapchain.hpp"
#include "vklearnin/system/devices/LogicalDevice.hpp"

namespace vkl {

// =============================================================================
void Application::run() {
    _engine->init();
    init();

    float cummulative_frametime = 0.0f;
    uint32_t cummulative_frame_count = 0u;

    while(TargetWindow::message_loop()) {
        _engine->render_loop();

        cummulative_frametime   += Timekeeper::frametime();
        cummulative_frame_count += 1;
        if(cummulative_frametime >= 0.5f) {
            CONSOLE_TRACE(
                "{:.02f} avg fps",
                cummulative_frame_count / cummulative_frametime
            );
            cummulative_frametime = 0.0f;
            cummulative_frame_count = 0u;
        }

        Timekeeper::update();
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
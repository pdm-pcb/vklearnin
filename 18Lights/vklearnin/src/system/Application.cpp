#include "vklearnin/vklearnin.hpp"
#include "vklearnin/system/Application.hpp"

#include "vklearnin/system/GraphicsAPI.hpp"
#include "vklearnin/system/window/TargetWindow.hpp"
#include "vklearnin/rendering/swapchain/Swapchain.hpp"
#include "vklearnin/system/devices/PhysicalDevice.hpp"
#include "vklearnin/system/devices/LogicalDevice.hpp"

namespace vkl {

// =============================================================================
void Application::run() {
    this->init();
    Renderer::create_pipelines();

    float frame_time = 0.0f;
    uint32_t frame_count = 0u;

    while(_running) {
        TargetWindow::message_loop();

        Timekeeper::frame_start();
            this->update();
            this->submit_draws();
            Renderer::record_commands();
            Renderer::submit_and_present();
        Timekeeper::frame_end();

        frame_time += Timekeeper::frame_time();
        frame_count += 1;

        if(frame_time >= 0.5f) {
            CONSOLE_TRACE("{:.02f} avg fps", frame_count / frame_time);
            frame_time = 0.0f;
            frame_count = 0u;
        }

        Timekeeper::update();
    }

    LogicalDevice::native().waitIdle();

    this->shutdown();
}

// =============================================================================
void Application::on_window_close([[maybe_unused]] const WindowCloseEvent &) {
    CONSOLE_TRACE("Application received WindowClosed");
    _running = false;
}

// =============================================================================
Application::Application() :
    _running { true }
{
    EventBroker::init();
    EventBroker::subscribe<WindowCloseEvent>(
        this,
        &Application::on_window_close
    );

    ConsoleLog::init();
    GraphicsAPI::init();

    TargetWindow::init();
    TargetWindow::spawn_window();
    TargetWindow::create_surface();

    PhysicalDevice::query_devices(
        {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME,
            VK_KHR_DRIVER_PROPERTIES_EXTENSION_NAME
        },
        {
            PhysicalDevice::Features::SAMPLER_ANISOTROPY,
            PhysicalDevice::Features::FILL_MODE_NONSOLID,
        }
    );
    PhysicalDevice::select_device();

    LogicalDevice::create({
        #ifdef VKL_DEBUG
            "VK_LAYER_KHRONOS_validation",
        #endif // VKL_DEBUG
    });

    Swapchain::create();
    Renderer::init();
}

Application::~Application() {
    Renderer::shutdown();
    Swapchain::destroy();
    LogicalDevice::destroy();
    TargetWindow::destroy_surface();
    TargetWindow::shutdown();
    GraphicsAPI::shutdown();

    EventBroker::shutdown();
}

} // namespace vkl
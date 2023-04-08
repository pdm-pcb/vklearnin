#include "vklearnin/vklearnin.hpp"
#include "vklearnin/system/Application.hpp"

#include "vklearnin/system/GraphicsAPI.hpp"
#include "vklearnin/system/window/TargetWindow.hpp"
#include "vklearnin/system/devices/PhysicalDevice.hpp"
#include "vklearnin/system/devices/LogicalDevice.hpp"

namespace vkl {

// =============================================================================
void Application::run() {
    this->init();
    Renderer::create_pipelines();

    float tick_time = 0.0f;
    uint32_t tick_count = 0u;

    while(_running) {
        Timekeeper::update();

        TargetWindow::message_loop();

        this->update();
        this->submit_draws();

        Renderer::record_commands();
        Renderer::submit_commands_and_present();

        tick_time += Timekeeper::tick_delta();
        ++tick_count;

        if(tick_time >= 0.5f) {
            CONSOLE_TRACE("{:.02f} avg fps", tick_count / tick_time);
            tick_time = 0.0f;
            tick_count = 0u;
        }
    }

    LogicalDevice::native().waitIdle();

    this->shutdown();
}

// =============================================================================
void
Application::on_window_close([[maybe_unused]] const WindowCloseEvent &event) {
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
            VK_KHR_SWAPCHAIN_EXTENSION_NAME
        },
        {
            PhysicalDevice::Features::FILL_MODE_NONSOLID,
            PhysicalDevice::Features::SAMPLER_ANISOTROPY,
            PhysicalDevice::Features::DYNAMIC_RENDERING,
            PhysicalDevice::Features::NONUNIFORM_DESCRIPTOR_INDEXING,
        }
    );
    PhysicalDevice::select_device();

    LogicalDevice::create({
        #ifdef VKL_DEBUG
            "VK_LAYER_KHRONOS_validation",
        #endif // VKL_DEBUG
    });

    Renderer::init();
}

Application::~Application() {
    Renderer::shutdown();
    LogicalDevice::destroy();
    TargetWindow::destroy_surface();
    TargetWindow::shutdown();
    GraphicsAPI::shutdown();

    EventBroker::shutdown();
}

} // namespace vkl
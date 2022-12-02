#include "vklearnin/vklearnin.hpp"
#include "vklearnin/system/Application.hpp"

#include "vklearnin/rendering/GraphicsInstance.hpp"
#include "vklearnin/system/TargetWindow.hpp"
#include "vklearnin/engine/Engine.hpp"

namespace vkl {

// =============================================================================
void Application::run() {
    _engine->init();
    this->init();
    _running = true;

    while(_running) {
        Timekeeper::frame_start();
            _engine->render_loop();
        Timekeeper::frame_end();

        TargetWindow::message_loop();
        Timekeeper::update();
    }

    // wait for current commands to run their course
    GraphicsInstance::wait_idle();
    CONSOLE_INFO("Shutting everything down");

    this->shutdown();
    _engine->shutdown();
    TargetWindow::shutdown();
    GraphicsInstance::shutdown();
    EventBroker::shutdown();
}

// =============================================================================
void Application::on_window_close([[maybe_unused]] const WindowCloseEvent &) {
    CONSOLE_TRACE("Application received WindowClosed");
    _running = false;
}

// =============================================================================
Application::Application() :
    _running { false },
    _engine  { new Engine(*this) }
{
    ConsoleLog::init();
    EventBroker::init();

    GraphicsInstance::init();
    TargetWindow::spawn_window();
    TargetWindow::create_surface();
    GraphicsInstance::init_devices();

    EventBroker::subscribe<WindowCloseEvent>(
        this,
        &Application::on_window_close
    );
}

Application::~Application() {
    delete _engine;
}

} // namespace vkl
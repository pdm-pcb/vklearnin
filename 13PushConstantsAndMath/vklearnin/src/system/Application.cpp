#include "vklearnin/vklearnin.hpp"
#include "vklearnin/system/Application.hpp"

#include "vklearnin/rendering/GraphicsInstance.hpp"
#include "vklearnin/system/TargetWindow.hpp"
#include "vklearnin/engine/Engine.hpp"

namespace vkl {

// =============================================================================
void Application::run() {
    _running = true;

    while(_running) {
        _engine->render_loop();
        _running = TargetWindow::message_loop();
    }

    // wait for current commands to run their course
    GraphicsInstance::wait_idle();
    CONSOLE_INFO("Shutting everything down");

    _engine->shutdown();
    TargetWindow::destroy_surface();
    GraphicsInstance::shutdown();
}

// =============================================================================
Application::Application() :
    _running { false },
    _engine  { new Engine }
{
    ConsoleLog::init();

    GraphicsInstance::init();
    TargetWindow::spawn_window();
    TargetWindow::create_surface();
    GraphicsInstance::init_devices();

    _engine->init();
}

Application::~Application() {
    delete _engine;
}

} // namespace vkl
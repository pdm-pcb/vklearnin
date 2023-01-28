#include "vklearnin/vklearnin.hpp"
#include "vklearnin/system/Application.hpp"

#include "vklearnin/system/Engine.hpp"
#include "vklearnin/system/window/TargetWindow.hpp"

namespace vkl {

// =============================================================================
void Application::run() {
    init();

    while(TargetWindow::message_loop()) {
        Timekeeper::frame_start();
            _engine->render_loop();
        Timekeeper::frame_end();
        CONSOLE_TRACE("Sixty frames might take: {}", Timekeeper::frametime());
    }

    shutdown();
}

// =============================================================================
Application::Application() :
    _engine { new Engine(*this) }
{
    ConsoleLog::init();
    GraphicsAPI::init();
    TargetWindow::spawn_window();
    TargetWindow::create_surface();
}

Application::~Application() {
    TargetWindow::destroy_surface();
    GraphicsAPI::shutdown();

    delete _engine;
}

} // namespace vkl
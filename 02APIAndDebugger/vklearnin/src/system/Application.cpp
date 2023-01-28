#include "vklearnin/vklearnin.hpp"
#include "vklearnin/system/Application.hpp"

#include "vklearnin/system/Engine.hpp"

namespace vkl {

// =============================================================================
void Application::run() {
    init();

    shutdown();
}

// =============================================================================
Application::Application() :
    _engine { new Engine(*this) }
{
    ConsoleLog::init();
    GraphicsAPI::init();
}

Application::~Application() {
    GraphicsAPI::shutdown();

    delete _engine;
}

} // namespace vkl
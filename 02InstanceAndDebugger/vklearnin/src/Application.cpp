#include "vklearnin/vklearnin.hpp"
#include "vklearnin/Application.hpp"

#include "vklearnin/GraphicsInstance.hpp"

namespace vkl {

// =============================================================================
void Application::init() {
    _graphics_instance = new GraphicsInstance;
    _graphics_instance->init();
}

// =============================================================================
void Application::run() {
}

// =============================================================================
Application::Application() :
    _graphics_instance { nullptr }
{
    ConsoleLog::init();
}

Application::~Application() {
    delete _graphics_instance;
}

} // namespace vkl
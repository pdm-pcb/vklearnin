#include "vklearnin/vklearnin.hpp"
#include "vklearnin/system/Engine.hpp"

#include "vklearnin/system/Application.hpp"

namespace vkl {

// =============================================================================
Engine::Engine(Application &app) :
    _application { app }
{ }

} // namespace vkl
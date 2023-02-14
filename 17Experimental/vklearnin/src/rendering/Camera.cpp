#include "vklearnin/vklearnin.hpp"
#include "vklearnin/rendering/Camera.hpp"

namespace vkl {

// =============================================================================
void Camera::set_orthographic(float const top, float const bottom) {
    _proj_mat = glm::ortho(
        -RenderConfig::window_aspect,
        RenderConfig::window_aspect,
        bottom,
        top
    );
}

// =============================================================================
void Camera::set_perspective(float const near, float const far,
                             float const vfov)
{
    _proj_mat = glm::perspective(
        glm::radians(vfov),
        RenderConfig::window_aspect,
        near,
        far
    );
}

// =============================================================================
void Camera::orient(glm::vec3 const &position, glm::vec3 const &forward) {
    if(glm::length(forward) <= 0.0f) {
        CONSOLE_CRITICAL("Cannot orient camera with zero forward vector.");
        return;
    }

    _view_mat = glm::lookAt(
        position,
        position + forward,
        { 0.0f, 1.0f, 0.0f }
    );
}

// =============================================================================
Camera::Camera() :
    _view_mat { 1.0f },
    _proj_mat { 1.0f }
{ }

} // namespace vkl
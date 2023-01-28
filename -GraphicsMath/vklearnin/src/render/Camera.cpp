#include "vklearnin/vklearnin.hpp"
#include "vklearnin/render/Camera.hpp"

namespace vkl {

// =============================================================================
void Camera::set_perspective(const float near_plane,
                             const float far_plane,
                             const float vertical_fov_degrees)
{
    const float vfov_radians = math::degrees_to_radians(vertical_fov_degrees);
    const float a = std::tanf(0.5f * vfov_radians);
    const float b = far_plane - near_plane;

    _proj_mat = {
        { 1.0f / (RenderConfig::window_aspect * a), 0.0f, 0.0f, 0.0f },
        { 0.0f, 1.0f / a, 0.0f, 0.0f },
        { 0.0f, 0.0f, far_plane / b, 1.0f },
        { 0.0f, 0.0f, -(far_plane * near_plane) / b, 0.0f },
    };

    CONSOLE_TRACE("\n{}", fmt::streamed(_proj_mat));
}

// =============================================================================
void Camera::_set_view_mat() {
    _forward = _target - _position;
    normalize(_forward);

    _right = cross(_forward, _up);
    normalize(_right);

    _view_mat = { _right, _up, _forward, _position };
    
    CONSOLE_TRACE("\n{}", fmt::streamed(_view_mat));
}

// =============================================================================
Camera::Camera(const vec4 &position, const vec4 &target, const vec4 &up) :
    _position { position },
    _target   { target },
    _up       { normalized(up) },
    _right    { },
    _forward  { },
    _view_mat { mat4_ident },
    _proj_mat { mat4_ident }
{
    _set_view_mat();
}

} // namespace vkl
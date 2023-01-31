#include "vklearnin/vklearnin.hpp"
#include "vklearnin/rendering/Camera.hpp"

namespace vkl {

// =============================================================================
void Camera::orient(const Vec3 &position, const Vec3 &target, const Vec3 &up) {
    _position = position;
    _target   = target;
    _up       = up;

    _set_view_mat();
}

// =============================================================================
void Camera::set_perspective(const float near_plane,
                             const float far_plane,
                             const float vertical_fov_degrees)
{
    const float vfov_radians = math::to_radians(vertical_fov_degrees);
    const float a = std::tanf(vfov_radians * 0.5f);
    const float b = near_plane - far_plane;
    const float c = far_plane - near_plane;

    _proj_mat = Mat4 {
        { 1.0f / (RenderConfig::window_aspect * a), 0.0f, 0.0f, 0.0f },
        { 0.0f, 1.0f / a, 0.0f, 0.0f },
        { 0.0f, 0.0f, far_plane / b, -1.0f },
        { 0.0f, 0.0f, -(far_plane * near_plane) / c, 0.0f },
    };

    CONSOLE_TRACE("\n{}", fmt::streamed(_proj_mat));
}

// =============================================================================
void Camera::_set_view_mat() {
    _forward = math::normalized(_target - _position);
    _side    = math::normalized(math::cross(_forward, _up));
    _up      = math::normalized(math::cross(_side, _forward));

    _view_mat = {
        { _side,     0.0f },
        { _up,       0.0f },
        { -_forward, 0.0f },
        {
            -math::dot(_side,    _position),
            -math::dot(_up,      _position),
             math::dot(_forward, _position),
            1.0f
        }
    };

    CONSOLE_TRACE("\n{}", fmt::streamed(_view_mat));
}

// =============================================================================
Camera::Camera(const Vec3 &position, const Vec3 &target, const Vec3 &up) :
    _position { position },
    _target   { target },
    _up       { up },
    _side     { },
    _forward  { },
    _view_mat { Mat4::identity },
    _proj_mat { Mat4::identity }
{
    orient(position, target, up);
}

Camera::Camera() :
    _position { },
    _target   { },
    _up       { },
    _side     { },
    _forward  { },
    _view_mat { Mat4::identity },
    _proj_mat { Mat4::identity }
{ }

} // namespace vkl
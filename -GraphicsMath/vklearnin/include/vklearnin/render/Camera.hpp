#ifndef VKLEARNIN_RENDER_CAMERA_HPP
#define VKLEARNIN_RENDER_CAMERA_HPP

#include "vklearnin/system/pch.hpp"
#include "vklearnin/math/vec4.hpp"
#include "vklearnin/math/mat4.hpp"

namespace vkl {

class Camera final {
public:
    void set_perspective(const float near_plane,
                         const float far_plane,
                         const float vertical_fov_degrees);

    inline const auto & view_matrix() const { return _view_mat; }
    inline const auto & proj_matrix() const { return _proj_mat; }

    Camera(const vec4 &position, const vec4 &target, const vec4 &up);
    Camera() = delete;

private:
    vec4 _position;
    vec4 _target;
    vec4 _up;
    vec4 _right;
    vec4 _forward;

    mat4 _view_mat;
    mat4 _proj_mat;

    void _set_view_mat();
};

} // namespace vkl

#endif // VKLEARNIN_RENDER_CAMERA_HPP
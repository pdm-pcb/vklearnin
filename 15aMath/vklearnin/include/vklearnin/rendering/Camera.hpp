#ifndef VKLEARNIN_RENDERING_CAMERA_HPP
#define VKLEARNIN_RENDERING_CAMERA_HPP

#include "vklearnin/system/pch.hpp"
#include "vklearnin/math/Vec4.hpp"
#include "vklearnin/math/Mat4.hpp"

namespace vkl {

class Camera final {
public:
    void orient(const Vec3 &position, const Vec3 &target, const Vec3 &up);
    void set_perspective(const float near_plane,
                         const float far_plane,
                         const float vertical_fov_degrees);

    inline const auto& view_matrix() const { return _view_mat; }
    inline const auto& proj_matrix() const { return _proj_mat; }

    Camera();
    ~Camera() = default;
    Camera(const Vec3 &position, const Vec3 &target, const Vec3 &up);

    Camera(Camera &&) = delete;
    Camera(const Camera &) = delete;

    Camera& operator=(Camera &&) = delete;
    Camera& operator=(const Camera &) = delete;

private:
    Vec3 _position;
    Vec3 _target;
    Vec3 _up;
    Vec3 _side;
    Vec3 _forward;

    Mat4 _view_mat;
    Mat4 _proj_mat;

    void _set_view_mat();
};

} // namespace vkl

#endif // VKLEARNIN_RENDERING_CAMERA_HPP
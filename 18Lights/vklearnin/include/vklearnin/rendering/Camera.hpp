#ifndef VKLEARNIN_RENDERING_CAMERA_HPP
#define VKLEARNIN_RENDERING_CAMERA_HPP

#include "vklearnin/system/pch.hpp"

namespace vkl {

class Camera final {
public:
    void set_orthographic(float const top = 1.0f, float const bottom = -1.0f);
    void set_perspective(float const near, float const far,
                         float const vertical_fov_degrees);

    void orient(Vec4 const &position, Vec4 const &forward, Vec4 const &side,
                Vec4 const &up = Vec4::unit_y);

    inline auto const& view_matrix() const { return _view_mat; }
    inline auto const& proj_matrix() const { return _proj_mat; }

    Camera();
    ~Camera() = default;

    Camera(Camera &&) = delete;
    Camera(const Camera &) = delete;

    Camera& operator=(Camera &&) = delete;
    Camera& operator=(const Camera &) = delete;

private:
    Mat4 _view_mat;
    Mat4 _proj_mat;
};

} // namespace vkl

#endif // VKLEARNIN_RENDERING_CAMERA_HPP
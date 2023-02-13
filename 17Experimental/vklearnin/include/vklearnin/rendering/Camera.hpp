#ifndef VKLEARNIN_RENDERING_CAMERA_HPP
#define VKLEARNIN_RENDERING_CAMERA_HPP

#include "vklearnin/system/pch.hpp"
#include "vklearnin/math/math.hpp"

namespace vkl {

class Camera final {
public:    
    void set_perspective(float const near, float const far,
                         float const vertical_fov_degrees);

    void orient(Vec4 const &position, Vec4 const &forward);

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
#include "vklearnin/vklearnin.hpp"
#include "vklearnin/rendering/Camera.hpp"

namespace vkl {

// =============================================================================
void Camera::set_orthographic(float const top, float const bottom) {
    auto const left = -RenderConfig::window_aspect;
    auto const right = RenderConfig::window_aspect;

#ifdef VKL_USE_GLM
    _proj_mat = glm::ortho(left, right, bottom, top);
#else
    auto const a = right - left;
    auto const b = top - bottom;

    _proj_mat = Mat4::identity;

    _proj_mat.x.x = 2.0f / a;
    _proj_mat.y.y = 2.0f / b;
    _proj_mat.z.z = -1.0f;
    _proj_mat.w.x = (right + left) / a;
    _proj_mat.w.y = -(top + bottom) / b;
#endif // VKL_USE_GLM
}

// =============================================================================
void Camera::set_perspective(float const near, float const far,
                             float const vfov)
{
    auto const aspect = RenderConfig::window_aspect;

#ifdef VKL_USE_GLM
    _proj_mat = glm::perspective(math::radians(vfov), aspect, near, far);
#else
    auto const a = std::tanf(math::radians(vfov) * 0.5f);

    _proj_mat = Mat4::zero;

    // Right handed, zero-to-one NDC space
    _proj_mat.x.x = 1.0f / (aspect * a);
    _proj_mat.y.y = 1.0f / a;
    _proj_mat.z.z = far / (near - far);
    _proj_mat.z.w = -1.0f;
    _proj_mat.w.z = -(far * near) / (far - near);
#endif // VKL_USE_GLM
}

// =============================================================================
void Camera::orient(Vec4 const &position, Vec4 const &forward, Vec4 const &up) {
    if(math::length2(forward) <= 0.0f) {
        CONSOLE_CRITICAL("Cannot orient camera with zero forward vector.");
        return;
    }

#ifdef VKL_USE_GLM
    _view_mat = glm::lookAt(
        glm::vec3(position),
        glm::vec3(position + forward),
        glm::vec3(up)
    );
#else
    auto const fore    = math::normalize(forward);
    auto const side    = math::normalize(math::cross(fore, up));
    auto const true_up = math::cross(side, fore);

    _view_mat = Mat4::identity;

    _view_mat.x.x = side.x;
    _view_mat.y.x = side.y;
    _view_mat.z.x = side.z;
    _view_mat.x.y = true_up.x;
    _view_mat.y.y = true_up.y;
    _view_mat.z.y = true_up.z;
    _view_mat.x.z = -fore.x;
    _view_mat.y.z = -fore.y;
    _view_mat.z.z = -fore.z;
    _view_mat.w.x = -math::dot(side, position);
    _view_mat.w.y = -math::dot(true_up, position);
    _view_mat.w.z =  math::dot(fore, position);
#endif // VKL_USE_GLM
}

// =============================================================================
Camera::Camera() :
#ifdef VKL_USE_GLM
    _view_mat { Mat4(1.0f) },
    _proj_mat { Mat4(0.0f) }
#else VKL_USE_GLM
    _view_mat { Mat4::identity },
    _proj_mat { Mat4::zero }
#endif // VKL_USE_GLM
{ }

} // namespace vkl
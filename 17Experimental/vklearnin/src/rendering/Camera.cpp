#include "vklearnin/vklearnin.hpp"
#include "vklearnin/rendering/Camera.hpp"

namespace vkl {

// =============================================================================
void Camera::set_perspective(float const near, float const far,
                             float const vfov)
{
    float const aspect = RenderConfig::window_aspect;
    float const a = std::tanf(math::to_radians(vfov) * 0.5f);
    float const b = far - near;

// This is the "incorrect" option; doing a GL style projection and flipping
// m[1][1] to be negative. It results in:

// proj_mat {1.358, 0.00,    0.00,    0.00}
//          {0.00, -2.41421, 0.00,    0.00}
//          {0.00,  0.00,    1.0001, -0.10001}
//          {0.00,  0.00,    1.00,    0.00} 64 float4x4 (column_major)

    _proj_mat = Mat4 {
        { aspect / a,  0.0f,      0.0f,             0.0f },
        { 0.0f,       -1.0f / a,  0.0f,             0.0f },
        { 0.0f,        0.0f,      far / b,          1.0f },
        { 0.0f,        0.0f,     -(far * near) / b, 0.0f },
    };

// Meanwhile, the below "correct" version gives this, but it doesn't work with
// my already established stuff. Probably the view matrix in specific?

// proj_mat {1.358, 0.00,    0.00,    0.00}
//          {0.00,  2.41421, 0.00,    0.00}
//          {0.00,  0.00,    1.0001, -99.99001}
//          {0.00,  0.00,    1.00,    0.00} 64 float4x4 (column_major)

    // _proj_mat = Mat4 {
    //     { aspect / a, 0.0f,     0.0f,      0.0f },
    //     { 0.0f,       1.0f / a, 0.0f,      0.0f },
    //     { 0.0f,       0.0f,     far / b,   1.0f },
    //     { 0.0f,       0.0f,     -near * b, 0.0f },
    // };
}

// =============================================================================
void Camera::orient(Vec4 const &position, Vec4 const &forward) {
    if(forward.length2() <= 0.0f) {
        CONSOLE_CRITICAL(
            "Cannot orient camera with forward vector [{}]",
            fmt::streamed(forward)
        );
        return;
    }

    Vec4 const fore = forward.normalized();
    Vec4 const side = math::cross(fore, Vec4::unit_y).normalized();
    Vec4 const up   = math::cross(fore, side);

    _view_mat = Mat4 {
        { side.x, side.y, side.z, 0.0f },
        { up.x,   up.y,   up.z,   0.0f },
        { fore.x, fore.y, fore.z, 0.0f },
        {
            -math::dot(side, position),
            -math::dot(up,   position),
            -math::dot(fore, position),
            1.0f
        },
    };
}

// =============================================================================
Camera::Camera() :
    _view_mat { Mat4::identity },
    _proj_mat { Mat4::identity }
{ }

} // namespace vkl
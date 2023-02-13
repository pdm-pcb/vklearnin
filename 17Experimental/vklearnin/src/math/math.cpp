#include "vklearnin/vklearnin.hpp"
#include "vklearnin/math/math.hpp"

namespace vkl {

// Insist on keeping the math types simple
static_assert(sizeof(Vec4) ==  sizeof(float) * 4);
static_assert(sizeof(Mat4) ==  sizeof(Vec4) * 4);
static_assert(std::is_aggregate<Vec4>());
static_assert(std::is_aggregate<Mat4>());

//==============================================================================
// Order-dependent statics

Vec4 const Vec4::unit_x { 1.0f, 0.0f, 0.0f, 0.0f };
Vec4 const Vec4::unit_y { 0.0f, 1.0f, 0.0f, 0.0f };
Vec4 const Vec4::unit_z { 0.0f, 0.0f, 1.0f, 0.0f };
Vec4 const Vec4::origin { 0.0f, 0.0f, 0.0f, 1.0f };

Mat4 const Mat4::identity { };

namespace math {

// =============================================================================
// Vector operations

float dot(Vec4 const &a, Vec4 const &b) {
    return a.x * b.x +
           a.y * b.y +
           a.z * b.z;
}

Vec4 cross(Vec4 const &a, Vec4 const &b) {
    return {
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x
    };
}

Vec4 axis_angle(Vec4 const &v, Vec4 const &axis, float angle) {
    auto const k = axis.normalized();
    auto const theta = to_radians(angle);
    auto const cos_theta = std::cosf(theta);
    auto const sin_theta = std::sinf(theta);

    return (
        v * cos_theta +
        cross(k, v) * sin_theta +
        k * dot(k, v) * (1.0f - cos_theta)
    ).normalized();
}

// =============================================================================
// Matrix operations

Mat4 translate(Mat4 const &m, Vec4 const &v) {
    auto result = m;
    result.w = { v.x, v.y, v.z, 1.0f };
    return result;
}

Mat4 rotate(Mat4 const &m, Vec4 const &v) {
    Vec4 const rads {
        to_radians(v.x),
        to_radians(v.y),
        to_radians(v.z),
    };

    Mat4 rot_x = Mat4::identity;
    if(rads.x > float_epsilon) {
        float const c_x = std::cosf(rads.x);
        float const s_x = std::sinf(rads.x);
        rot_x.y = { 0.0f,  c_x, s_x, 0.0f };
        rot_x.z = { 0.0f, -s_x, c_x, 0.0f };
    }

    Mat4 rot_y = Mat4::identity;
    if(rads.y > float_epsilon) {
        float const c_y = std::cosf(rads.y);
        float const s_y = std::sinf(rads.y);
        rot_y.x = { c_y, 0.0f, -s_y, 0.0f };
        rot_y.z = { s_y, 0.0f,  c_y, 0.0f };
    }

    Mat4 rot_z = Mat4::identity;
    if(rads.z > float_epsilon) {
        float const c_z = std::cosf(rads.z);
        float const s_z = std::sinf(rads.z);
        rot_z.x = {  c_z, s_z, 0.0f, 0.0f };
        rot_z.y = { -s_z, c_z, 0.0f, 0.0f };
    }

    return m * rot_x * rot_y * rot_z;
}

Mat4 scale(Mat4 const &m, Vec4 const &v) {
    auto result = m;
    result.x.x = v.x;
    result.x.y = v.y;
    result.x.z = v.z;

    return m;
}

} // namespace math
} // namespace vkl
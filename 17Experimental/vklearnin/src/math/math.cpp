#include "vklearnin/vklearnin.hpp"
#include "vklearnin/math/math.hpp"

namespace vkl {

#ifndef VKL_USE_GLM
// Insist on keeping the math types simple
static_assert(sizeof(Vec4) ==  sizeof(float) * 4);
static_assert(sizeof(Mat4) ==  sizeof(Vec4) * 4);
static_assert(std::is_aggregate<Vec4>());
static_assert(std::is_aggregate<Mat4>());

// Order-dependent static variables
Vec4 const Vec4::unit_x { 1.0f, 0.0f, 0.0f, 0.0f };
Vec4 const Vec4::unit_y { 0.0f, 1.0f, 0.0f, 0.0f };
Vec4 const Vec4::unit_z { 0.0f, 0.0f, 1.0f, 0.0f };
Vec4 const Vec4::origin { 0.0f, 0.0f, 0.0f, 1.0f };

Mat4 const Mat4::identity { };
Mat4 const Mat4::zero {
    { 0.0f, 0.0f, 0.0f, 0.0f },
    { 0.0f, 0.0f, 0.0f, 0.0f },
    { 0.0f, 0.0f, 0.0f, 0.0f },
    { 0.0f, 0.0f, 0.0f, 0.0f },
};

#endif // VKL_USE_GLM

namespace math {

// =============================================================================
// Vector operations
Vec4 normalize(Vec4 const &v) {
    auto length = length2(v);
    if(length <= 0.0f || length == 1.0f) {
        return v;
    }

    auto result = v;
    float length_inv = 1.0f / std::sqrtf(length);

    result.x *= length_inv;
    result.y *= length_inv;
    result.z *= length_inv;

    return result;
}

float dot(Vec4 const &a, Vec4 const &b) {
    return a.x * b.x +
           a.y * b.y +
           a.z * b.z;
}

Vec4 cross(Vec4 const &a, Vec4 const &b) {
    return {
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x,
        0.0f
    };
}

// =============================================================================
// Matrix operations
Mat4 transpose(Mat4 const &m) {
#ifdef VKL_USE_GLM
    return glm::transpose(m);
#else
    auto result = m;

    std::swap(result.x.y, result.y.x);
    std::swap(result.x.z, result.z.x);
    std::swap(result.x.w, result.w.x);

    std::swap(result.y.z, result.z.y);
    std::swap(result.y.w, result.w.y);

    std::swap(result.z.w, result.w.z);

    return result;
#endif // VKL_USE_GLM
}

Mat4 translate(Mat4 const &m, Vec4 const &v) {
#ifdef VKL_USE_GLM
    return glm::translate(m, glm::vec3(v));
#else
    auto result = m;
    result.w = (m.x * v.x) + (m.y * v.y) + (m.z * v.z) + m.w;
    return result;
#endif // VKL_USE_GLM
}

Mat4 rotate(Mat4 const &m, float const angle, Vec4 const &axis) {
    float const theta = radians(angle);
    auto result = m;

#ifdef VKL_USE_GLM
    result = glm::rotate(m, theta, glm::vec3(axis));
#else
    float const cos_theta = std::cosf(theta);
    float const sin_theta = std::sinf(theta);

    auto const rot_axis = normalize(axis);
    auto const temp = (1.0f - cos_theta) * rot_axis;

    auto rot_mat = Mat4::identity;
    rot_mat.x.x = cos_theta + temp.x * rot_axis.x;
    rot_mat.x.y = temp.x * rot_axis.y + sin_theta * rot_axis.z;
    rot_mat.x.z = temp.x * rot_axis.z - sin_theta * rot_axis.y;

    rot_mat.y.x = temp.y * rot_axis.x - sin_theta * rot_axis.z;
    rot_mat.y.y = cos_theta + temp.y * rot_axis.y;
    rot_mat.y.z = temp.y * rot_axis.z + sin_theta * rot_axis.x;

    rot_mat.z.x = temp.z * rot_axis.x + sin_theta * rot_axis.y;
    rot_mat.z.y = temp.z * rot_axis.y - sin_theta * rot_axis.x;
    rot_mat.z.z = cos_theta + temp.z * rot_axis.z;

    result.x = m.x * rot_mat.x.x + m.y * rot_mat.x.y + m.z * rot_mat.x.z;
    result.y = m.x * rot_mat.y.x + m.y * rot_mat.y.y + m.z * rot_mat.y.z;
    result.z = m.x * rot_mat.z.x + m.y * rot_mat.z.y + m.z * rot_mat.z.z;
    result.w = m.w;
#endif // VKL_USE_GLM

    return result;
}

Mat4 scale(Mat4 const &m, Vec4 const &v) {
#ifdef VKL_USE_GLM
    return glm::scale(m, glm::vec3(v));
#else
    Mat4 result;
    result.x = m.x * v.x;
    result.y = m.y * v.y;
    result.z = m.z * v.z;
    result.w = m.w;

    return result;
#endif // VKL_USE_GLM
}

} // namespace math
} // namespace vkl
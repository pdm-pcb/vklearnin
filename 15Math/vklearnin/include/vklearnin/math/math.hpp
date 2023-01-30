#ifndef VKLEARNIN_MATH_MATH_HPP
#define VKLEARNIN_MATH_MATH_HPP

#include "vklearnin/system/pch.hpp"
#include "vklearnin/math/Vec4.hpp"
#include "vklearnin/math/Mat4.hpp"

namespace vkl {

namespace math {

static constexpr float float_epsilon      = 1.0e-6f;
static constexpr float pi                 = std::numbers::pi_v<float>;
static constexpr float pi_over_one_eighty = pi / 180.0f;
static constexpr float one_eighty_over_pi = 180.0f / pi;

inline float degrees_to_radians(const float degrees) {
    return degrees * pi_over_one_eighty;
}

inline float radians_to_degrees(const float radians) {
    return radians * one_eighty_over_pi;
}

} // namespace math

// =============================================================================
// Four-component vectors
static constexpr uint8_t print_width = 10u;
static constexpr uint8_t print_precs = 4u;

inline float calc_length(const Vec4 &a) {
    return std::sqrtf((a.x * a.x) + (a.y * a.y) + (a.z * a.z));
}

float dot(const Vec4 &a, const Vec4 &b);
Vec4 cross(const Vec4 &a, const Vec4 &b);

Vec4 normalized(const Vec4 &a);
inline void normalize(Vec4 &a) { a = normalized(a); }

// =============================================================================
// 4x4 matrices
Mat4 transposed(const Mat4 &a);
inline void transpose(Mat4 &a) { a = transposed(a); }

Mat4 trasnlated(const Mat4 &a, const Vec4 &pos);
inline void trasnlate(Mat4 &a, const Vec4 &pos) { a = trasnlated(a, pos); }

Mat4 rotated(const Mat4 &a, const Vec4 &degrees);
inline void rotate(Mat4 &a, const Vec4 &degrees) {
    a = rotated(a, degrees);
}

} // namespace vkl

#endif // VKLEARNIN_MATH_MATH_HPP
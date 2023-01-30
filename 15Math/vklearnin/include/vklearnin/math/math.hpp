#ifndef VKLEARNIN_MATH_MATH_HPP
#define VKLEARNIN_MATH_MATH_HPP

#include "vklearnin/system/pch.hpp"
#include "vklearnin/math/Vec3.hpp"
#include "vklearnin/math/Vec4.hpp"
#include "vklearnin/math/Mat3.hpp"
#include "vklearnin/math/Mat4.hpp"

namespace vkl::math {

// General constants
static constexpr float float_epsilon      = 1.0e-6f;
static constexpr float pi                 = std::numbers::pi_v<float>;
static constexpr float pi_over_one_eighty = pi / 180.0f;
static constexpr float one_eighty_over_pi = 180.0f / pi;

// For pretty-printing vectors and matrices
static constexpr uint8_t print_width = 10u;
static constexpr uint8_t print_precs = 4u;

// Conversions
inline float to_radians(const float degrees) {
    return degrees * pi_over_one_eighty;
}
inline float to_degrees(const float radians) {
    return radians * one_eighty_over_pi;
}

// =============================================================================
// Three-component vectors

inline float length(const Vec3 &a) {
    return std::sqrtf((a.x * a.x) + (a.y * a.y) + (a.z * a.z));
}

inline float length(const Vec4 &a) {
    return std::sqrtf((a.x * a.x) + (a.y * a.y) + (a.z * a.z));
}

float dot(const Vec3 &a, const Vec3 &b);
Vec3 cross(const Vec3 &a, const Vec3 &b);

Vec3 normalized(const Vec3 &a);
inline void normalize(Vec3 &a) { a = normalized(a); }

// =============================================================================
// 3x3 matrices
Mat3 transposed(const Mat3 &a);
inline void transpose(Mat3 &a) { a = transposed(a); }

// =============================================================================
// 4x4 matrices
Mat4 transposed(const Mat4 &a);
inline void transpose(Mat4 &a) { a = transposed(a); }

Mat4 translated(const Mat4 &a, const Vec3 &pos);
inline void translate(Mat4 &a, const Vec3 &pos) { a = translated(a, pos); }

Mat4 rotated(const Mat4 &a, const Vec3 &degrees);
inline void rotate(Mat4 &a, const Vec3 &degrees) {
    a = rotated(a, degrees);
}

} // namespace vkl::math

#endif // VKLEARNIN_MATH_MATH_HPP
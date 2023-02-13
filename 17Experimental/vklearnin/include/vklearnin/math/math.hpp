#ifndef VKLEARNIN_MATH_MATH_HPP
#define VKLEARNIN_MATH_MATH_HPP

#include "vklearnin/system/pch.hpp"
#include "vklearnin/math/Vec4.hpp"
#include "vklearnin/math/Mat4.hpp"

namespace vkl::math {

// General constants
static float constexpr float_epsilon      = 1.0e-6f;
static float constexpr pi                 = std::numbers::pi_v<float>;
static float constexpr pi_over_one_eighty = pi / 180.0f;
static float constexpr one_eighty_over_pi = 180.0f / pi;

// For pretty-printing vectors and matrices
static uint8_t constexpr print_precs = 4u;
static uint8_t constexpr print_width = 2u * print_precs + 2u;

// Conversions
inline float to_radians(float const degrees) {
    return degrees * pi_over_one_eighty;
}
inline float to_degrees(float const radians) {
    return radians * one_eighty_over_pi;
}

// =============================================================================
// Vector operations

float dot(Vec4 const &a, Vec4 const &b);
Vec4 cross(Vec4 const &a, Vec4 const &b);
Vec4 axis_angle(Vec4 const &v, Vec4 const &axis, float angle);

// =============================================================================
// Matrix operations

Mat4 translate(Mat4 const &m, Vec4 const &v);
Mat4 rotate(Mat4 const &m, Vec4 const &v);
Mat4 scale(Mat4 const &m, Vec4 const &v);

} // namespace vkl::math

#endif // VKLEARNIN_MATH_MATH_HPP
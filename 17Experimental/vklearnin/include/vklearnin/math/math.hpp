#ifndef VKLEARNIN_MATH_MATH_HPP
#define VKLEARNIN_MATH_MATH_HPP

#include "vklearnin/system/pch.hpp"
#include "vklearnin/math/Vec3.hpp"
#include "vklearnin/math/Vec4.hpp"
#include "vklearnin/math/Mat3.hpp"
#include "vklearnin/math/Mat4.hpp"

namespace vkl::math {

// General constants
static float constexpr float_epsilon      = 1.0e-6f;
static float constexpr pi                 = std::numbers::pi_v<float>;
static float constexpr pi_over_one_eighty = pi / 180.0f;
static float constexpr one_eighty_over_pi = 180.0f / pi;

// For pretty-printing vectors and matrices
static uint8_t constexpr print_width = 10u;
static uint8_t constexpr print_precs = 4u;

// Conversions
inline float to_radians(float const degrees) {
    return degrees * pi_over_one_eighty;
}
inline float to_degrees(float const radians) {
    return radians * one_eighty_over_pi;
}

// =============================================================================
// Three-component vectors

inline float length(const Vec3 &a) {
    return std::sqrtf((a.x * a.x) + (a.y * a.y) + (a.z * a.z));
}

float dot(Vec3 const& a, Vec3 const& b);
Vec3 cross(Vec3 const& a, Vec3 const& b);

void normalize(Vec3 &a);
Vec3 normalized(Vec3 const& a);

// =============================================================================
// 4x4 matrices

void translate(Mat4 &a, Vec3 const& pos);
Mat4 translated(Mat4 const& a, Vec3 const& pos);

void rotate(Mat4 &a, Vec3 const& degrees);
Mat4 rotated(Mat4 const& a, Vec3 const& degrees);

void scale(Mat4 &a, Vec3 const& scale);
Mat4 scaled(Mat4 const& a, Vec3 const& scale);

} // namespace vkl::math

#endif // VKLEARNIN_MATH_MATH_HPP
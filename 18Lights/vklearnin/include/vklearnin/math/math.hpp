#ifndef VKLEARNIN_MATH_MATH_HPP
#define VKLEARNIN_MATH_MATH_HPP

#include "vklearnin/math/Vec2.hpp"
#include "vklearnin/math/Vec3.hpp"
#include "vklearnin/math/Vec4.hpp"
#include "vklearnin/math/Mat4.hpp"

#include <cmath>
#include <numbers>
#include <limits>

namespace vkl::math {

// General constants
static float constexpr float_epsilon = std::numeric_limits<float>::epsilon();
static float constexpr pi = std::numbers::pi_v<float>;
static float constexpr pi_over_one_eighty = pi / 180.0f;
static float constexpr one_eighty_over_pi = 180.0f / pi;

// For pretty-printing vectors and matrices
static uint8_t constexpr print_precs = 4u;
static uint8_t constexpr print_width = 2u * print_precs + 2u;

// Conversions
[[nodiscard]] inline float radians(float const deg) {
    return deg * pi_over_one_eighty;
}
[[nodiscard]] inline float degrees(float const rad) {
    return rad * one_eighty_over_pi;
}

// =============================================================================
// Vector operations
[[nodiscard]] inline float length2(Vec3 const &v) {
    return (v.x * v.x) + (v.y * v.y) + (v.z * v.z);
}

[[nodiscard]] inline float length(Vec3 const &v) {
    return std::sqrt(length2(v));
}

[[nodiscard]] Vec3 normalize(Vec3 const &v);
[[nodiscard]] float dot(Vec3 const &a, Vec3 const &b);
[[nodiscard]] Vec3 cross(Vec3 const &a, Vec3 const &b);

// =============================================================================
// Matrix operations
[[nodiscard]] Mat4 transpose(Mat4 const &m);
[[nodiscard]] Mat4 translate(Mat4 const &m, Vec3 const &v);
[[nodiscard]] Mat4 rotate(Mat4 const &m, float const angle, Vec3 const &axis);
[[nodiscard]] Mat4 scale(Mat4 const &m, float const &pct);

// =============================================================================
// Camera math
[[nodiscard]] Mat4 ortho_proj_rh_zo(float const near, float const far,
                                    float const left, float const right,
                                    float const bottom, float const top);

[[nodiscard]] Mat4 persp_proj_rh_zo_inf(float const near,
                                        float const vfov_degrees,
                                        float const aspect_ratio);

[[nodiscard]] Mat4 persp_proj_rh_oz_inf(float const near,
                                        float const vfov_degrees,
                                        float const aspect_ratio);

[[nodiscard]] Mat4 orient_view_matrix(Vec3 const &position, Vec3 const &forward,
                                      Vec3 const &side, Vec3 const &up);

[[nodiscard]] Mat4 look_at(Vec3 const &position, Vec3 const &target,
                           Vec3 const &up);

} // namespace vkl::math

#endif // VKLEARNIN_MATH_MATH_HPP
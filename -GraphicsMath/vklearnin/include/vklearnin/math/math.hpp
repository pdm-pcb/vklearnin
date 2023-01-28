#ifndef VKLEARNIN_MATH_MATH_HPP
#define VKLEARNIN_MATH_MATH_HPP

#include "vklearnin/system/pch.hpp"
#include "vklearnin/math/vec4.hpp"
#include "vklearnin/math/mat4.hpp"

namespace vkl {

namespace math {

static constexpr float float_epsilon      = 1.0e-6f;
static constexpr float pi                 = std::numbers::pi_v<float>;
static constexpr float pi_over_one_eighty = pi / 180.0f;
static constexpr float one_eighty_over_pi = pi / 180.0f;

inline float degrees_to_radians(const float degrees) {
    return degrees * pi_over_one_eighty;
}

inline float radians_to_degrees(const float radians) {
    return radians * one_eighty_over_pi;
}

} // namespace math

// =============================================================================
// Four-component vectors
static const vec4 vec4_unit_x { 1.0f, 0.0f, 0.0f, 0.0f };
static const vec4 vec4_unit_y { 0.0f, 1.0f, 0.0f, 0.0f };
static const vec4 vec4_unit_z { 0.0f, 0.0f, 1.0f, 0.0f };
static const vec4 vec4_origin { 0.0f, 0.0f, 0.0f, 1.0f };

static constexpr uint8_t print_width = 10u;
static constexpr uint8_t print_precs = 4u;

inline float calc_length(const vec4 &a) {
    return std::sqrtf((a.x * a.x) + (a.y * a.y) + (a.z * a.z));
}

float dot(const vec4 &a, const vec4 &b);
vec4 cross(const vec4 &a, const vec4 &b);

vec4 normalized(const vec4 &a);
inline void normalize(vec4 &a) { a = normalized(a); }

// =============================================================================
// 4x4 matrices
static const mat4 mat4_ident {
    vec4_unit_x,
    vec4_unit_y,
    vec4_unit_z,
    vec4_origin
};

mat4 transposed(const mat4 &a);
inline void transpose(mat4 &a) { a = transposed(a); }

mat4 trasnlated(const mat4 &a, const vec4 &pos);
inline void trasnlate(mat4 &a, const vec4 &pos) { a = trasnlated(a, pos); }

mat4 rotated(const mat4 &a, const vec4 &axis, const float angle);
inline void rotate(mat4 &a, const vec4 &axis, const float angle) {
    a = rotated(a, axis, angle);
}

mat4 scaled(const mat4 &a, const float scale_factor);
inline void scale(mat4 &a, const float scale_factor) {
    a = scaled(a, scale_factor);
}

} // namespace vkl

#endif // VKLEARNIN_MATH_MATH_HPP
#ifndef VKLEARNIN_MATH_VEC4_HPP
#define VKLEARNIN_MATH_VEC4_HPP

#include "vklearnin/system/pch.hpp"

namespace vkl {

struct alignas(16) Vec4 {
    static Vec4 const unit_x;
    static Vec4 const unit_y;
    static Vec4 const unit_z;
    static Vec4 const origin;

// =============================================================================
    [[nodiscard]] Vec4 operator+(Vec4 const& other) const;
    [[nodiscard]] Vec4 operator-(Vec4 const& other) const;
    Vec4 & operator+=(Vec4 const& other);
    Vec4 & operator-=(Vec4 const& other);
    Vec4 & operator*=(float scalar);

    [[nodiscard]] Vec4 operator-() const;
    Vec4 & operator-();

// =============================================================================
    [[nodiscard]] bool operator==(Vec4 const& other) const;
    friend std::ostream& operator<<(std::ostream& out, Vec4 const& a);

// =============================================================================
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
    float w = 0.0f;
};

[[nodiscard]] Vec4 operator*(Vec4 const &v, float scalar);
[[nodiscard]] Vec4 operator*(float scalar, Vec4 const &v);

std::ostream& operator<<(std::ostream& out, Vec4 const& v);

} // namespace vkl

#endif // VKLEARNIN_MATH_VEC4_HPP
#ifndef VKLEARNIN_MATH_VEC2_HPP
#define VKLEARNIN_MATH_VEC2_HPP

#include "vklearnin/system/pch.hpp"

namespace vkl {

struct Vec2 {
    static Vec2 const unit_x;
    static Vec2 const unit_y;
    static Vec2 const origin;

// =============================================================================
    [[nodiscard]] Vec2 operator+(Vec2 const& other) const;
    [[nodiscard]] Vec2 operator-(Vec2 const& other) const;
    Vec2 & operator+=(Vec2 const& other);
    Vec2 & operator-=(Vec2 const& other);
    Vec2 & operator*=(float scalar);

    [[nodiscard]] Vec2 operator-() const;
    Vec2 & operator-();

// =============================================================================
    [[nodiscard]] bool operator==(Vec2 const& other) const;
    friend std::ostream& operator<<(std::ostream& out, Vec2 const& v);

// =============================================================================
    float x = 0.0f;
    float y = 0.0f;
};

[[nodiscard]] Vec2 operator*(Vec2 const &v, float scalar);
[[nodiscard]] Vec2 operator*(float scalar, Vec2 const &v);

std::ostream& operator<<(std::ostream& out, Vec2 const& v);

} // namespace vkl

#endif // VKLEARNIN_MATH_VEC2_HPP
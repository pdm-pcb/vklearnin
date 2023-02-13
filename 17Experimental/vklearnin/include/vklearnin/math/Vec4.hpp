#ifndef VKLEARNIN_MATH_VEC4_HPP
#define VKLEARNIN_MATH_VEC4_HPP

#include "vklearnin/system/pch.hpp"

namespace vkl {

struct Vec4 {
    static Vec4 const unit_x;
    static Vec4 const unit_y;
    static Vec4 const unit_z;
    static Vec4 const origin;

// =============================================================================
    inline float length2() const { return (x * x) + (y * y) + (z * z); }
    inline float length()  const { return std::sqrtf(length2()); }

// =============================================================================
    void normalize();
    Vec4 normalized() const;

// =============================================================================
    Vec4 operator+(Vec4 const& other) const;
    Vec4 operator-(Vec4 const& other) const;
    Vec4 & operator+=(Vec4 const& other);
    Vec4 & operator-=(Vec4 const& other);
    Vec4 operator*(float scalar) const;
    Vec4 & operator*=(float scalar);

    Vec4 operator-() const;
    Vec4 & operator-();

// =============================================================================
    bool operator==(Vec4 const& other) const;
    friend std::ostream& operator<<(std::ostream& out, Vec4 const& a);

// =============================================================================
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
    float w = 0.0f;
};

std::ostream& operator<<(std::ostream& out, Vec4 const& a);

} // namespace vkl

#endif // VKLEARNIN_MATH_VEC4_HPP
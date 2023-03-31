#ifndef VKLEARNIN_MATH_VEC4_HPP
#define VKLEARNIN_MATH_VEC4_HPP

#include "vklearnin/system/pch.hpp"

namespace vkl {

struct Vec3;

struct Vec4 {
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
    friend std::ostream& operator<<(std::ostream& out, Vec4 const& v);

// =============================================================================
    float x;
    float y;
    float z;
    float w;

// =============================================================================
    Vec4();
    ~Vec4() = default;

    Vec4(float x, float y, float z, float w);
    Vec4(Vec3 const &v, float w);

    Vec4(Vec4 &&) = default;
    Vec4(Vec4 const &) = default;

    Vec4 & operator=(Vec4 &&) = default;
    Vec4 & operator=(Vec4 const &) = default;
};

[[nodiscard]] Vec4 operator*(Vec4 const &v, float scalar);
[[nodiscard]] Vec4 operator*(float scalar, Vec4 const &v);

std::ostream& operator<<(std::ostream& out, Vec4 const& v);

} // namespace vkl

#endif // VKLEARNIN_MATH_VEC4_HPP
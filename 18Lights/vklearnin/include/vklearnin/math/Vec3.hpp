#ifndef VKLEARNIN_MATH_VEC3_HPP
#define VKLEARNIN_MATH_VEC3_HPP

#include "vklearnin/system/pch.hpp"

namespace vkl {

struct Vec4;

struct Vec3 {
    static Vec3 const unit_x;
    static Vec3 const unit_y;
    static Vec3 const unit_z;
    static Vec3 const origin;

// =============================================================================
    [[nodiscard]] Vec3 operator+(Vec3 const& other) const;
    [[nodiscard]] Vec3 operator-(Vec3 const& other) const;
    Vec3 & operator+=(Vec3 const& other);
    Vec3 & operator-=(Vec3 const& other);
    Vec3 & operator*=(float scalar);

    [[nodiscard]] Vec3 operator-() const;
    Vec3 & operator-();

// =============================================================================
    [[nodiscard]] bool operator==(Vec3 const& other) const;
    friend std::ostream& operator<<(std::ostream& out, Vec3 const& v);

// =============================================================================
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;

// =============================================================================
    Vec3();
    ~Vec3() = default;

    Vec3(float x, float y, float z);
    Vec3(Vec4 const &v);

    Vec3(Vec3 &&) = default;
    Vec3(Vec3 const &) = default;

    Vec3 & operator=(Vec3 &&) = default;
    Vec3 & operator=(Vec3 const &) = default;
};

[[nodiscard]] Vec3 operator*(Vec3 const &v, float scalar);
[[nodiscard]] Vec3 operator*(float scalar, Vec3 const &v);

std::ostream& operator<<(std::ostream& out, Vec3 const& v);

} // namespace vkl

#endif // VKLEARNIN_MATH_VEC3_HPP
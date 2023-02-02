#ifndef VKLEARNIN_MATH_VEC3_HPP
#define VKLEARNIN_MATH_VEC3_HPP

#include "vklearnin/system/pch.hpp"

namespace vkl {

struct Vec4;

struct Vec3 final {
    explicit Vec3(std::array<float, 4> const& vec);
    Vec3(float const x, float const y, float const z);
    Vec3();

    explicit Vec3(Vec4 const& other);
    Vec3& operator=(Vec4 const& other);

    Vec3& operator+=(Vec3 const& other);
    Vec3& operator-=(Vec3 const& other);
    Vec3& operator*=(float const scalar);
    Vec3& operator/=(float const scalar);

    friend std::ostream& operator<<(std::ostream& out, Vec3 const& a);

    float x;
    float y;
    float z;

    static Vec3 const unit_x;
    static Vec3 const unit_y;
    static Vec3 const unit_z;
    static Vec3 const origin;
};

Vec3 operator+(Vec3 const& a, Vec3 const& b);
Vec3 operator-(Vec3 const& a, Vec3 const& b);
Vec3 operator*(Vec3 const& a, float const scalar);
Vec3 operator/(Vec3 const& a, float const scalar);

Vec3 operator-(Vec3 const& a);

bool operator==(Vec3 const& a, Vec3 const& b);

std::ostream& operator<<(std::ostream& out, Vec3 const& a);

} // namespace vkl

#endif // VKLEARNIN_MATH_VEC3_HPP
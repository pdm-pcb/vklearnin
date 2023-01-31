#ifndef VKLEARNIN_MATH_VEC3_HPP
#define VKLEARNIN_MATH_VEC3_HPP

#include "vklearnin/system/pch.hpp"

namespace vkl {

struct Vec4;

struct Vec3 final {
    explicit Vec3(const std::array<float, 4> &vec);
    Vec3(const float x, const float y, const float z);
    Vec3();

    explicit Vec3(const Vec4 &other);
    Vec3& operator=(const Vec4 &other);

    Vec3& operator+=(const Vec3 &other);
    Vec3& operator-=(const Vec3 &other);
    Vec3& operator*=(const float scalar);
    Vec3& operator/=(const float scalar);

    friend std::ostream& operator<<(std::ostream& out, const Vec3& a);

    float x;
    float y;
    float z;

    static const Vec3 unit_x;
    static const Vec3 unit_y;
    static const Vec3 unit_z;
    static const Vec3 origin;
};

Vec3 operator+(const Vec3 &a, const Vec3 &b);
Vec3 operator-(const Vec3 &a, const Vec3 &b);
Vec3 operator*(const Vec3 &a, const float scalar);
Vec3 operator/(const Vec3 &a, const float scalar);

Vec3 operator-(const Vec3 &a);

bool operator==(const Vec3 &a, const Vec3 &b);

std::ostream& operator<<(std::ostream &out, const Vec3 &a);

} // namespace vkl

#endif // VKLEARNIN_MATH_VEC3_HPP
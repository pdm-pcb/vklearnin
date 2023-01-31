#ifndef VKLEARNIN_MATH_VEC4_HPP
#define VKLEARNIN_MATH_VEC4_HPP

#include "vklearnin/system/pch.hpp"

namespace vkl {

struct Vec3;

struct Vec4 final {
    explicit Vec4(const std::array<float, 4> &vec);
    Vec4(const float x, const float y, const float z, const float w);
    Vec4();

    Vec4(const Vec3 &other, const float w);

    Vec4& operator+=(const Vec4 &other);
    Vec4& operator-=(const Vec4 &other);
    Vec4& operator*=(const float scalar);
    Vec4& operator/=(const float scalar);

    friend std::ostream& operator<<(std::ostream& out, const Vec4& a);

    float x;
    float y;
    float z;
    float w;

    static const Vec4 unit_x;
    static const Vec4 unit_y;
    static const Vec4 unit_z;
    static const Vec4 origin;
};

Vec4 operator+(const Vec4 &a, const Vec4 &b);
Vec4 operator-(const Vec4 &a, const Vec4 &b);
Vec4 operator*(const Vec4 &a, const float scalar);
Vec4 operator/(const Vec4 &a, const float scalar);

Vec4 operator-(const Vec4 &a);

bool operator==(const Vec4 &a, const Vec4 &b);

std::ostream& operator<<(std::ostream &out, const Vec4 &a);

} // namespace vkl

#endif // VKLEARNIN_MATH_VEC4_HPP
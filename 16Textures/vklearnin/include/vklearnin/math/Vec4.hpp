#ifndef VKLEARNIN_MATH_VEC4_HPP
#define VKLEARNIN_MATH_VEC4_HPP

#include "vklearnin/system/pch.hpp"

namespace vkl {

struct Vec3;

struct Vec4 final {
    explicit Vec4(const std::array<float, 4> &vec);
    Vec4(float const x, float const y, float const z, float const w);
    Vec4();

    Vec4(const Vec3 &other, float const w);

    Vec4& operator+=(Vec4 const& other);
    Vec4& operator-=(Vec4 const& other);
    Vec4& operator*=(float const scalar);
    Vec4& operator/=(float const scalar);

    friend std::ostream& operator<<(std::ostream& out, Vec4 const& a);

    float x;
    float y;
    float z;
    float w;

    static Vec4 const unit_x;
    static Vec4 const unit_y;
    static Vec4 const unit_z;
    static Vec4 const origin;
};

Vec4 operator+(Vec4 const& a, Vec4 const& b);
Vec4 operator-(Vec4 const& a, Vec4 const& b);
Vec4 operator*(Vec4 const& a, float const scalar);
Vec4 operator/(Vec4 const& a, float const scalar);

Vec4 operator-(Vec4 const& a);

bool operator==(Vec4 const& a, Vec4 const& b);

std::ostream& operator<<(std::ostream& out, Vec4 const& a);

} // namespace vkl

#endif // VKLEARNIN_MATH_VEC4_HPP
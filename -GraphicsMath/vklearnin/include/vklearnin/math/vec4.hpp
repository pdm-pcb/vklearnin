#ifndef VKLEARNIN_MATH_VEC4_HPP
#define VKLEARNIN_MATH_VEC4_HPP

#include "vklearnin/system/pch.hpp"

namespace vkl {

struct vec4 final {
    explicit vec4(const std::array<float, 4> &vec);
    vec4(const float x, const float y, const float z, const float w);
    vec4();

    vec4 operator+(const vec4 &other) const;
    vec4 operator-(const vec4 &other) const;
    vec4 operator*(const float scalar) const;
    vec4 operator/(const float scalar) const;

    vec4 & operator+=(const vec4 &other);
    vec4 & operator-=(const vec4 &other);
    vec4 & operator*=(const float scalar);
    vec4 & operator/=(const float scalar);

    bool operator==(const vec4 &other) const;

    friend std::ostream & operator<<(std::ostream& out, const vec4& a);

    float x;
    float y;
    float z;
    float w;

    float length;
};

std::ostream & operator<<(std::ostream &out, const vec4 &a);

} // namespace vkl

#endif // VKLEARNIN_MATH_VEC4_HPP
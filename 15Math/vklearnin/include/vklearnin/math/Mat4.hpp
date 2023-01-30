#ifndef VKLEARNIN_MATH_MAT4_HPP
#define VKLEARNIN_MATH_MAT4_HPP

#include "vklearnin/system/pch.hpp"
#include "vklearnin/math/Vec4.hpp"

namespace vkl {

struct Mat4 final {
    explicit Mat4(const std::array<Vec4, 4> &vecs);
    Mat4(const Vec4 &x, const Vec4 &y, const Vec4 &z, const Vec4 &w);
    Mat4();

    Mat4 operator*(const Mat4 &other) const;

    Mat4 & operator*=(const Mat4 &other);

    bool operator==(const Mat4 &other) const;

    friend std::ostream & operator<<(std::ostream& out, const Mat4& a);

    std::array<Vec4, 4> rows;

    static const Mat4 identity;
};

std::ostream & operator<<(std::ostream& out, const Mat4& a);

} // namespace vkl

#endif // VKLEARNIN_MATH_MAT4_HPP
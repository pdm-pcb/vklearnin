#ifndef VKLEARNIN_MATH_MAT3_HPP
#define VKLEARNIN_MATH_MAT3_HPP

#include "vklearnin/system/pch.hpp"
#include "vklearnin/math/Vec3.hpp"

namespace vkl {

struct Mat3 final {
    explicit Mat3(const std::array<Vec3, 3> &vecs);
    explicit Mat3(const std::array<Vec4, 3> &vecs);
    Mat3(const Vec3 &x, const Vec3 &y, const Vec3 &z);
    Mat3(const Vec4 &x, const Vec4 &y, const Vec4 &z);
    Mat3();

    Mat3& operator*=(const Mat3 &other);

    std::array<Vec3, 3> rows;

    static const Mat3 identity;
};

Mat3 operator*(const Mat3 &a, const Mat3 &b);
bool operator==(const Mat3 &a, const Mat3 &b);

std::ostream& operator<<(std::ostream& out, const Mat3& a);

} // namespace vkl

#endif // VKLEARNIN_MATH_MAT3_HPP
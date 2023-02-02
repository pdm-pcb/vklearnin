#ifndef VKLEARNIN_MATH_MAT3_HPP
#define VKLEARNIN_MATH_MAT3_HPP

#include "vklearnin/system/pch.hpp"
#include "vklearnin/math/Vec3.hpp"

namespace vkl {

struct Mat3 final {
    explicit Mat3(std::array<Vec3, 3> const& vecs);
    explicit Mat3(std::array<Vec4, 3> const& vecs);
    Mat3(Vec3 const& x, Vec3 const& y, Vec3 const& z);
    Mat3(Vec4 const& x, Vec4 const& y, Vec4 const& z);
    Mat3();

    Mat3& operator*=(Mat3 const& other);

    std::array<Vec3, 3> rows;

    static Mat3 const identity;
};

Mat3 operator*(Mat3 const& a, Mat3 const& b);
bool operator==(Mat3 const& a, Mat3 const& b);

std::ostream& operator<<(std::ostream& out, Mat3 const& a);

} // namespace vkl

#endif // VKLEARNIN_MATH_MAT3_HPP
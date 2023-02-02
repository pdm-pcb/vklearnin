#ifndef VKLEARNIN_MATH_MAT4_HPP
#define VKLEARNIN_MATH_MAT4_HPP

#include "vklearnin/system/pch.hpp"
#include "vklearnin/math/Vec4.hpp"

namespace vkl {

struct Mat3;

struct Mat4 final {
    explicit Mat4(std::array<Vec4, 4> const& vecs);
    Mat4(Vec4 const& x, Vec4 const& y, Vec4 const& z, Vec4 const& w);
    Mat4();

    Mat4& operator=(Mat3 const& other);
    Mat4& operator*=(Mat4 const& other);

    std::array<Vec4, 4> rows;

    static Mat4 const identity;
};

Mat4 operator*(Mat4 const& a, Mat4 const& b);
bool operator==(Mat4 const& a, Mat4 const& b);

std::ostream& operator<<(std::ostream& out, Mat4 const& a);

} // namespace vkl

#endif // VKLEARNIN_MATH_MAT4_HPP
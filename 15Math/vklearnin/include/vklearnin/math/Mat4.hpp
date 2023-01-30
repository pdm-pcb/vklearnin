#ifndef VKLEARNIN_MATH_MAT4_HPP
#define VKLEARNIN_MATH_MAT4_HPP

#include "vklearnin/system/pch.hpp"
#include "vklearnin/math/Vec4.hpp"

namespace vkl {

struct Mat3;

struct Mat4 final {
    explicit Mat4(const std::array<Vec4, 4> &vecs);
    Mat4(const Vec4 &x, const Vec4 &y, const Vec4 &z, const Vec4 &w);
    Mat4();

#ifdef VKL_USE_GLM
    explicit Mat4(const glm::mat4 &other);
    Mat4& operator=(const glm::mat4 &other);
#endif // VKL_USE_GLM

    Mat4& operator=(const Mat3 &other);
    Mat4& operator*=(const Mat4 &other);

    std::array<Vec4, 4> rows;

    static const Mat4 identity;
};

Mat4 operator*(const Mat4 &a, const Mat4 &b);
bool operator==(const Mat4 &a, const Mat4 &b);

std::ostream& operator<<(std::ostream& out, const Mat4& a);

} // namespace vkl

#endif // VKLEARNIN_MATH_MAT4_HPP
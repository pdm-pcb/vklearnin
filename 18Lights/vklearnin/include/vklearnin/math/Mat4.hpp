#ifndef VKLEARNIN_MATH_MAT4_HPP
#define VKLEARNIN_MATH_MAT4_HPP

#include "vklearnin/system/pch.hpp"

namespace vkl {

struct Mat4 {
    static Mat4 const identity;
    static Mat4 const zero;

// =============================================================================
    Mat4 & operator*=(Mat4 const &other);
    [[nodiscard]] Mat4 operator*(Mat4 const &other) const;

    [[nodiscard]] Vec4 operator*(Vec4 const &v) const;

// =============================================================================
    [[nodiscard]] bool operator==(Mat4 const &other) const;
    friend std::ostream& operator<<(std::ostream& out, Mat4 const& m);

// =============================================================================
    Vec4 x = Vec4::unit_x;
    Vec4 y = Vec4::unit_y;
    Vec4 z = Vec4::unit_z;
    Vec4 w = Vec4::origin;
};

std::ostream& operator<<(std::ostream& out, Mat4 const& m);

} // namespace vkl

#endif // VKLEARNIN_MATH_MAT4_HPP
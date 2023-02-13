#ifndef VKLEARNIN_MATH_MAT4_HPP
#define VKLEARNIN_MATH_MAT4_HPP

#include "vklearnin/system/pch.hpp"
#include "vklearnin/math/Vec4.hpp"

namespace vkl {

struct Mat4 {
    static Mat4 const identity;

// =============================================================================
    void transpose();
    Mat4 transposed() const;

// =============================================================================
    Mat4 & operator*=(Mat4 const &other);
    Mat4 operator*(Mat4 const &other) const;
    
    Vec4 operator*(Vec4 const &v) const;

// =============================================================================
    bool operator==(Mat4 const &other) const;
    friend std::ostream& operator<<(std::ostream& out, Mat4 const& a);

// =============================================================================
    Vec4 x = Vec4::unit_x;
    Vec4 y = Vec4::unit_y;
    Vec4 z = Vec4::unit_z;
    Vec4 w = Vec4::origin;
};

std::ostream& operator<<(std::ostream& out, Mat4 const& a);

} // namespace vkl

#endif // VKLEARNIN_MATH_MAT4_HPP
#ifndef VKLEARNIN_MATH_MAT4_HPP
#define VKLEARNIN_MATH_MAT4_HPP

#include "vklearnin/system/pch.hpp"
#include "vklearnin/math/vec4.hpp"

namespace vkl {

struct mat4 final {
    explicit mat4(const std::array<vec4, 4> &vecs);
    mat4(const vec4 &x, const vec4 &y, const vec4 &z, const vec4 &w);
    mat4();

    bool operator==(const mat4 &other) const;

    friend std::ostream & operator<<(std::ostream& out, const mat4& a);

    std::array<vec4, 4> rows;
};

std::ostream & operator<<(std::ostream& out, const mat4& a);

} // namespace vkl

#endif // VKLEARNIN_MATH_MAT4_HPP
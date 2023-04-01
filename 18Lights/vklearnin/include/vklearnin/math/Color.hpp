#ifndef VKLEARNIN_MATH_COLOR_HPP
#define VKLEARNIN_MATH_COLOR_HPP

#include "vklearnin/math/Vec3.hpp"

namespace vkl::color {

Vec3 const red   { 1.0f, 0.0f, 0.0f };
Vec3 const green { 0.0f, 1.0f, 0.0f };
Vec3 const blue  { 0.0f, 0.0f, 1.0f };
Vec3 const white { 1.0f, 1.0f, 1.0f };
Vec3 const black { 0.0f, 0.0f, 0.0f };

Vec3 const sunlight { 0.39f, 0.37f, 0.25f };

} // namespace vkl::colors

#endif // VKLEARNIN_MATH_COLOR_HPP
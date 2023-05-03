#ifndef VKLEARNIN_MATH_COLOR_HPP
#define VKLEARNIN_MATH_COLOR_HPP

#include "vklearnin/math/Vec3.hpp"

namespace vkl::color {

Vec3 const red   { 1.0f, 0.0f, 0.0f };
Vec3 const green { 0.0f, 1.0f, 0.0f };
Vec3 const blue  { 0.0f, 0.0f, 1.0f };
Vec3 const white { 1.0f, 1.0f, 1.0f };
Vec3 const black { 0.0f, 0.0f, 0.0f };

Vec3 const quarter_white { 0.25f, 0.25f, 0.25f };
Vec3 const half_white    { 0.50f, 0.50f, 0.50f };

Vec3 const american_green { 0.15f, 0.65f, 0.25f };
Vec3 const denim_blue     { 0.15f, 0.25f, 0.65f };
Vec3 const terra_cotta    { 0.85f, 0.45f, 0.35f };

Vec3 const sunlight     { 0.39f, 0.37f, 0.25f };
Vec3 const glacier_gray { 0.77f, 0.78f, 0.78f };
Vec3 const simple_gray  { 0.34f, 0.34f, 0.33f };

} // namespace vkl::colors

#endif // VKLEARNIN_MATH_COLOR_HPP
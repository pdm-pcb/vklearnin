#ifndef VKLEARNIN_MATH_COLOR_HPP
#define VKLEARNIN_MATH_COLOR_HPP

#include "vklearnin/math/Vec4.hpp"

namespace vkl::Color {

static Vec4 constexpr red   = { 1.0f, 0.0f, 0.0f, 1.0f };
static Vec4 constexpr green = { 0.0f, 1.0f, 0.0f, 1.0f };
static Vec4 constexpr blue  = { 0.0f, 0.0f, 1.0f, 1.0f };
static Vec4 constexpr white = { 1.0f, 1.0f, 1.0f, 1.0f };
static Vec4 constexpr black = { 0.0f, 0.0f, 0.0f, 1.0f };

} // namespace vkl::Colors

#endif // VKLEARNIN_MATH_COLOR_HPP
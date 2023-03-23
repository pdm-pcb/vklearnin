#ifndef VKLEARNIN_MATERIALS_LITCOLORMATERIAL_HPP
#define VKLEARNIN_MATERIALS_LITCOLORMATERIAL_HPP

#include "vklearnin/system/pch.hpp"

namespace vkl {

struct LitColorMaterial {
    Vec4 base_color;
    float specular_exponent;
};

} // namespace vkl

#endif // VKLEARNIN_MATERIALS_LITCOLORMATERIAL_HPP
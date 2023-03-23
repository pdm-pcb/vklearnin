#ifndef VKLEARNIN_MATERIALS_MATERIAL_HPP
#define VKLEARNIN_MATERIALS_MATERIAL_HPP

#include "vklearnin/system/pch.hpp"
#include "vklearnin/materials/Texture2D.hpp"

namespace vkl {

struct Material final {
    Texture2D diffuse;
    float specular_exponent;
};

} // namespace vkl

#endif // VKLEARNIN_MATERIALS_MATERIAL_HPP
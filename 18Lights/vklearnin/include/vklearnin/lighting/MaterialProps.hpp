#ifndef VKLEARNIN_LIGHTING_MATERIALPROPS_HPP
#define VKLEARNIN_LIGHTING_MATERIALPROPS_HPP

#include "vklearnin/system/pch.hpp"

namespace vkl {

struct MaterialProps {
    Vec4 ambient  { 0.5f, 0.5f, 0.5f, 1.0f };
    Vec4 diffuse  { 0.5f, 0.5f, 0.5f, 1.0f };
    Vec4 specular { 0.5f, 0.5f, 0.5f, 1.0f };
    float shine = 32.0f;
};

} // namespace vkl

#endif // VKLEARNIN_LIGHTING_MATERIALPROPS_HPP
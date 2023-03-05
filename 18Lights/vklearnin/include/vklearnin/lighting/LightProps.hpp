#ifndef VKLEARNIN_LIGHTING_LIGHTPROPS_HPP
#define VKLEARNIN_LIGHTING_LIGHTPROPS_HPP

#include "vklearnin/system/pch.hpp"

namespace vkl {

struct DirectionalLight {
    Vec4 toward = Vec4::unit_y;
    Vec4 color { 1.0f, 1.0f, 1.0f, 1.0f };
};

struct PointLight {
    Vec4 position = Vec4::origin;
    Vec4 color { 1.0f, 1.0f, 1.0f, 1.0f };
};

struct LightProps {
    DirectionalLight dir { };
    PointLight point { };

    float ambient = 0.05f;
};

} // namespace vkl

#endif // VKLEARNIN_LIGHTING_SURFACEPROPS_HPP
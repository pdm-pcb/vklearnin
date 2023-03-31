#ifndef VKLEARNIN_LIGHTING_SCENELIGHTS_HPP
#define VKLEARNIN_LIGHTING_SCENELIGHTS_HPP

#include "vklearnin/system/pch.hpp"

namespace vkl {

struct DirectionalLight {
    Vec4 position = Vec4::unit_y;
    Vec4 color { 1.0f, 1.0f, 1.0f, 1.0f };
};

struct PointLight {
    Vec4 position = Vec4::origin;
    Vec4 color { 1.0f, 1.0f, 1.0f, 1.0f };
};

struct SpotLight {
    Vec4 position = Vec4::origin;
    Vec4 forward = -Vec4::unit_z;
    Vec4 color { 1.0f, 1.0f, 1.0f, 1.0f };

    float inner_beam_angle = std::cos(math::radians(12.5f));
    float outer_beam_angle = std::cos(math::radians(20.0f));
};

struct SceneLights {
    alignas(16) DirectionalLight dir { };
    alignas(16) PointLight point { };
    alignas(16) SpotLight spot { };

    alignas(16) float ambient = 0.05f;
};

} // namespace vkl

#endif // VKLEARNIN_LIGHTING_SCENELIGHTS_HPP
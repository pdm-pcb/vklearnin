#ifndef VKLEARNIN_LIGHTING_SCENELIGHTS_HPP
#define VKLEARNIN_LIGHTING_SCENELIGHTS_HPP

#include "vklearnin/system/pch.hpp"

namespace vkl {

struct DirectionalLight {
    Mat4 vp_mat   = Mat4::identity;
    Vec4 position = Vec4::unit_y;
    Vec4 color { 1.0f, 1.0f, 1.0f, 1.0f };
};

struct PointLight {
    Mat4 vp_mat   = Mat4::identity;
    Vec4 position = Vec4::origin;
    Vec4 color { 1.0f, 1.0f, 1.0f, 1.0f };
};

struct alignas(16) SpotLight {
    Mat4 vp_mat   = Mat4::identity;
    Vec4 position = Vec4::origin;
    Vec4 color { 1.0f, 1.0f, 1.0f, 1.0f };
    Vec4 forward  = -Vec4::unit_z;

    float inner_beam_angle = std::cos(math::radians(12.5f));
    float outer_beam_angle = std::cos(math::radians(20.0f));
};

struct SceneLights {
    std::vector<DirectionalLight> dir;
    std::vector<PointLight> point;
    std::vector<SpotLight> spot;
};

struct LightProps {
    float scene_ambient  = 0.05f;
    uint32_t dir_count   = 0u;
    uint32_t point_count = 0u;
    uint32_t spot_count  = 0u;
};

struct ShadowPassMVP final {
    Mat4 light_vp_matrix = Mat4::identity;
    Mat4 model_matrix    = Mat4::identity;
};

} // namespace vkl

#endif // VKLEARNIN_LIGHTING_SCENELIGHTS_HPP
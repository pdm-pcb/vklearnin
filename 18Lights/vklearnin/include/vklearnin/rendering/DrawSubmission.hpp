#ifndef VKLEARNIN_RENDERING_DRAWSUBMISSION_HPP
#define VKLEARNIN_RENDERING_DRAWSUBMISSION_HPP

#include "vklearnin/system/pch.hpp"
#include "vklearnin/meshes/Mesh.hpp"
#include "vklearnin/materials/Texture2D.hpp"
#include "vklearnin/materials/Material.hpp"

namespace vkl {

struct PushConstant {
    vk::ShaderStageFlags const stage_flags = vk::ShaderStageFlagBits::eAll;
    size_t               const size        = 0;
    void                 const *data       = nullptr;
};

template <typename VertexType>
struct DrawSubmission {
    Mesh<VertexType> const *mesh = nullptr;
    Texture2D        const *texture = nullptr;
    Material         const *material = nullptr;

    std::vector<PushConstant> const push_constants;
};

} // namespace vkl

#endif // VKLEARNIN_RENDERING_DRAWSUBMISSION_HPP
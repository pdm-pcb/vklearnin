#ifndef VKLEARNIN_RENDERING_DRAWSUBMISSION_HPP
#define VKLEARNIN_RENDERING_DRAWSUBMISSION_HPP

#include "vklearnin/system/pch.hpp"
#include "vklearnin/meshes/Mesh.hpp"
#include "vklearnin/materials/Texture2D.hpp"

namespace vkl {

struct PushConstant {
    vk::ShaderStageFlags const stage_flags = vk::ShaderStageFlagBits::eAll;
    size_t               const size        = 0;
    void                 const *data       = nullptr;
};

template <typename VertexType>
struct DrawSubmission {
    vkl::Mesh<VertexType>     const *mesh = nullptr;
    vkl::Texture2D            const *texture = nullptr;
    std::vector<PushConstant> const push_constants;
};

} // namespace vkl

#endif // VKLEARNIN_RENDERING_DRAWSUBMISSION_HPP
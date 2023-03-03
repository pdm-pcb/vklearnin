#ifndef VKLEARNIN_RENDERING_DRAWSUBMISSION_HPP
#define VKLEARNIN_RENDERING_DRAWSUBMISSION_HPP

#include "vklearnin/system/pch.hpp"
#include "vklearnin/meshes/Mesh.hpp"
#include "vklearnin/resources/images/Texture2D.hpp"

namespace vkl {

struct PushConstant {
    vk::ShaderStageFlags stage_flags = vk::ShaderStageFlagBits::eAll;
    size_t               size        = 0;
    void                 *data       = nullptr;
};

template <typename VertexType>
struct DrawSubmission {
    vkl::Mesh<VertexType>     *mesh = nullptr;
    vkl::Texture2D            *material = nullptr;
    std::vector<PushConstant> push_constants;
};

} // namespace vkl

#endif // VKLEARNIN_RENDERING_DRAWSUBMISSION_HPP
#ifndef VKLEARNIN_RENDERING_DRAWSUBMISSION_HPP
#define VKLEARNIN_RENDERING_DRAWSUBMISSION_HPP

#include "vklearnin/system/pch.hpp"
#include "vklearnin/meshes/GeneratedMesh.hpp"
#include "vklearnin/resources/images/Texture2D.hpp"

namespace vkl {

struct PushConstant {
    vk::ShaderStageFlags const stage_flags = vk::ShaderStageFlagBits::eAll;
    size_t               const size        = 0;
    void                 const *data       = nullptr;
};

struct DrawSubmission {
    GeneratedMesh const *mesh         = nullptr;
    Mat4          const *model_matrix = nullptr;
    Texture2D     const *texture      = nullptr;

    std::vector<PushConstant> const push_constants;
};

} // namespace vkl

#endif // VKLEARNIN_RENDERING_DRAWSUBMISSION_HPP
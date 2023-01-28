#ifndef VKLEARNIN_RENDERING_DRAWSUBMISSION_HPP
#define VKLEARNIN_RENDERING_DRAWSUBMISSION_HPP

#include "vklearnin/system/pch.hpp"

namespace vkl {

class Mesh;

struct PushConstant {
    vk::ShaderStageFlags stage_flags;
    uint32_t size;
    void *data;
};

struct DrawSubmission {
    Mesh &mesh;
    std::vector<PushConstant> push_constants;
};

} // namespace vkl

#endif // VKLEARNIN_RENDERING_DRAWSUBMISSION_HPP
#ifndef VKLEARNIN_RENDERING_DRAWSUBMISSION_HPP
#define VKLEARNIN_RENDERING_DRAWSUBMISSION_HPP

#include "vklearnin/system/pch.hpp"

namespace vkl {

struct PushConstant {
    vk::ShaderStageFlags const stage_flags;
    size_t               const size;
    const void*          const data;
};

struct DrawSubmission {
    vk::Buffer const vertex_buffer;
    vk::Buffer const index_buffer;
    size_t     const index_count;
    vk::Image  const material;
    std::vector<PushConstant> push_constants;
};

} // namespace vkl

#endif // VKLEARNIN_RENDERING_DRAWSUBMISSION_HPP
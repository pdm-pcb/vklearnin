#ifndef VKLEARNIN_RENDERING_DRAWSUBMISSION_HPP
#define VKLEARNIN_RENDERING_DRAWSUBMISSION_HPP

#include "vklearnin/system/pch.hpp"
#include "vklearnin/resources/buffers/BufferObject.hpp"
#include "vklearnin/resources/images/Texture2D.hpp"

namespace vkl {

struct PushConstant {
    vk::ShaderStageFlags const stage_flags;
    size_t               const size;
    void                 const *data;
};

struct DrawSubmission {
    vkl::BufferObject const vertex_buffer;
    vkl::BufferObject const index_buffer;
    size_t            const index_count;
    vkl::Texture2D    const material;
    std::vector<PushConstant> push_constants;
};

} // namespace vkl

#endif // VKLEARNIN_RENDERING_DRAWSUBMISSION_HPP
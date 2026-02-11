#include "vklearnin/vklearnin.hpp"
#include "vklearnin/rendering/camera/PerspectiveCam.hpp"

#include "vklearnin/vulkan/resources/vkBuffer.hpp"
#include "vklearnin/vulkan/descriptors/vkDescriptorSet.hpp"

namespace vkl {

// =============================================================================
void PerspectiveCam::create_ubos(std::size_t const swapchain_image_count,
                                 vkDevice const &device)
{
    _ubos.resize(swapchain_image_count);
    for(auto &ubo : _ubos) {
        ubo.create(sizeof(_persp_mats),
                   vk::BufferUsageFlagBits::eUniformBuffer,
                   device);

        ubo.allocate(vk::MemoryPropertyFlagBits::eHostVisible
                     | vk::MemoryPropertyFlagBits::eHostCoherent);
    }
}

// =============================================================================
void PerspectiveCam::create_descriptors(std::size_t const swapchain_image_count,
                                        vkDescriptorPool const &descriptor_pool,
                                        vkDevice const &device)
{
    _descriptor_set_layout
        .add_binding(0u,                                 // binding
                     vk::DescriptorType::eUniformBuffer, // type
                     1u,                                 // descriptor count
                     vk::ShaderStageFlagBits::eVertex)   // stage flags
        .create(device);

    _descriptor_sets.resize(swapchain_image_count);
    for(uint32_t i = 0u; i < _descriptor_sets.size(); ++i) {
        _descriptor_sets[i].allocate(_descriptor_set_layout,
                                     descriptor_pool,
                                     device);

        _descriptor_sets[i]
            .add_update(vk::DescriptorBufferInfo {
                            .buffer = _ubos[i].native(),
                            .offset = 0u,
                            .range = VK_WHOLE_SIZE,
                        },
                        0u,
                        vk::DescriptorType::eUniformBuffer)
            .update();
    }
}

// =============================================================================
void PerspectiveCam::destroy_descriptors() {
    _descriptor_set_layout.destroy();
}

// =============================================================================
void PerspectiveCam::destroy_ubos() {
    for(auto &ubo : _ubos) {
        ubo.destroy();
    }

    _ubos.clear();
}

// =============================================================================
void PerspectiveCam::update_camera_ubos(uint32_t const frame_index) {
    _ubos[frame_index].fill_buffer(&_persp_mats);
}

// =============================================================================
void PerspectiveCam::bind_descriptor_set(uint32_t const frame_index,
                                         vkGraphicsPipeline const &pipeline,
                                         uint32_t const descriptor_set_number,
                                         vkCmdBuffer const &cmd_buffer)
{
    _descriptor_sets[frame_index].bind(
        pipeline,
        descriptor_set_number,
        cmd_buffer
    );
}

} // namespace vkl
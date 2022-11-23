#include "vklearnin/vklearnin.hpp"
#include "vklearnin/shaders/DescriptorSet.hpp"

#include "vklearnin/shaders/DescriptorPool.hpp"
#include "vklearnin/shaders/DescriptorSetLayout.hpp"
#include "vklearnin/rendering/devices/LogicalDevice.hpp"

namespace vkl {

void
DescriptorSet::create(const DescriptorPool &descriptor_pool,
                      const DescriptorSetLayout &layout,
                      const size_t instance_buffer_size)
{
    vk::DescriptorSetAllocateInfo alloc_info {
        .descriptorPool = descriptor_pool.native(),
        .descriptorSetCount = 1u,
        .pSetLayouts = &layout.native()
    };
    
    auto result = LogicalDevice::native().allocateDescriptorSets(
        &alloc_info,
        &_descriptor_set
    );
    if(result != vk::Result::eSuccess) {
        CONSOLE_CRITICAL("Could not allocate descriptor sets");
    }

    _instance_buffer = BufferTools::create_buffer(
        instance_buffer_size,
        vk::BufferUsageFlagBits::eUniformBuffer,
        vk::SharingMode::eExclusive,
        (vk::MemoryPropertyFlagBits::eHostVisible |
        vk::MemoryPropertyFlagBits::eHostCoherent),
        "instance desc"
    );
    
    _texture = ImageTools::load_from_file(
        "../../vklearnin/assets/textures/metal_panel.jpg"
    );

    vk::DescriptorBufferInfo buffer_info {
        .buffer = _instance_buffer.buffer,
        .offset = 0u,
        .range = instance_buffer_size
    };

    vk::DescriptorImageInfo image_info {
        .sampler = _texture.sampler,
        .imageView = _texture.view,
        .imageLayout = _texture.layout
    };

    vk::WriteDescriptorSet set_writes[] {
        {
            .dstSet = _descriptor_set,
            .dstBinding = 0u,
            .dstArrayElement = 0u,
            .descriptorCount = 1u,
            .descriptorType = vk::DescriptorType::eUniformBuffer,
            .pImageInfo = nullptr,
            .pBufferInfo = &buffer_info,
            .pTexelBufferView = nullptr
        },
        {
            .dstSet = _descriptor_set,
            .dstBinding = 1u,
            .dstArrayElement = 0u,
            .descriptorCount = 1u,
            .descriptorType = vk::DescriptorType::eCombinedImageSampler,
            .pImageInfo = &image_info,
            .pBufferInfo = nullptr,
            .pTexelBufferView = nullptr
        },
    };

    LogicalDevice::native().updateDescriptorSets(set_writes, nullptr);
}

void DescriptorSet::destroy() {
    BufferTools::destroy_buffer(_instance_buffer);
    ImageTools::destroy_image(_texture);
}

DescriptorSet::DescriptorSet(DescriptorSet &&other) :
    _descriptor_set  { std::move(other._descriptor_set)  },
    _instance_buffer { std::move(other._instance_buffer) }
{ }

} // namespace vkl
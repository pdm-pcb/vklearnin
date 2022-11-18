#include "vklearnin/vklearnin.hpp"
#include "vklearnin/shaders/DescriptorSet.hpp"

#include "vklearnin/shaders/DescriptorPool.hpp"
#include "vklearnin/shaders/DescriptorSetLayout.hpp"
#include "vklearnin/rendering/devices/LogicalDevice.hpp"

namespace vkl {

void
DescriptorSet::create(const DescriptorPool &descriptor_pool,
                      const DescriptorSetLayout &layout,
                      const size_t set_size_bytes)
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

    _buffer = BufferTools::create_buffer(
        set_size_bytes,
        vk::BufferUsageFlagBits::eUniformBuffer,
        vk::SharingMode::eExclusive,
        (vk::MemoryPropertyFlagBits::eHostVisible |
        vk::MemoryPropertyFlagBits::eHostCoherent)
    );

    vk::DescriptorBufferInfo buffer_info {
        .buffer = _buffer.buffer,
        .offset = 0u,
        .range = set_size_bytes
    };

    vk::WriteDescriptorSet set_writes[] {{
        .dstSet = _descriptor_set,
        .dstBinding = 0u,
        .dstArrayElement = 0u,
        .descriptorCount = 1u,
        .descriptorType = vk::DescriptorType::eUniformBuffer,
        .pImageInfo = nullptr,
        .pBufferInfo = &buffer_info,
        .pTexelBufferView = nullptr
    }};

    LogicalDevice::native().updateDescriptorSets(set_writes, nullptr);
}

void DescriptorSet::destroy() {
    BufferTools::destroy_buffer(_buffer);
}

DescriptorSet::DescriptorSet(DescriptorSet &&other) :
    _descriptor_set { std::move(other._descriptor_set) },
    _buffer { std::move(other._buffer) }
{ }

} // namespace vkl
#include "vklearnin/vklearnin.hpp"
#include "vklearnin/rendering/descriptors/DescriptorSet.hpp"

#include "vklearnin/rendering/descriptors/DescriptorPool.hpp"
#include "vklearnin/system/devices/LogicalDevice.hpp"

namespace vkl {

// =============================================================================
DescriptorSet & DescriptorSet::add_ubo(BufferObject const &buffer) {
    auto const *buffer_info =
        &_buffer_info.emplace_back(vk::DescriptorBufferInfo {
            .buffer = buffer.handle,
            .offset = 0u,
            .range = VK_WHOLE_SIZE
        });

    _set_writes.emplace_back(vk::WriteDescriptorSet {
        .dstSet = _set,
        .dstBinding = static_cast<uint32_t>(_set_writes.size()),
        .dstArrayElement = 0u,
        .descriptorCount = 1u,
        .descriptorType = vk::DescriptorType::eUniformBuffer,
        .pImageInfo = nullptr,
        .pBufferInfo = buffer_info,
        .pTexelBufferView = nullptr
    });

    return *this;
}

// =============================================================================
DescriptorSet & DescriptorSet::add_ssbo(BufferObject const &buffer) {
    auto const *buffer_info =
        &_buffer_info.emplace_back(vk::DescriptorBufferInfo {
            .buffer = buffer.handle,
            .offset = 0u,
            .range = VK_WHOLE_SIZE
        });

    _set_writes.emplace_back(vk::WriteDescriptorSet {
        .dstSet = _set,
        .dstBinding = static_cast<uint32_t>(_set_writes.size()),
        .dstArrayElement = 0u,
        .descriptorCount = 1u,
        .descriptorType = vk::DescriptorType::eStorageBuffer,
        .pImageInfo = nullptr,
        .pBufferInfo = buffer_info,
        .pTexelBufferView = nullptr
    });

    return *this;
}

// =============================================================================
DescriptorSet & DescriptorSet::add_combined_sampler(ImageObject const &image) {
    auto const *image_info =
        &_image_info.emplace_back(vk::DescriptorImageInfo {
            .sampler     = image.sampler,
            .imageView   = image.view,
            .imageLayout = image.layout
        });

    _set_writes.emplace_back(vk::WriteDescriptorSet {
        .dstSet = _set,
        .dstBinding = static_cast<uint32_t>(_set_writes.size()),
        .dstArrayElement = 0u,
        .descriptorCount = 1u,
        .descriptorType = vk::DescriptorType::eCombinedImageSampler,
        .pImageInfo = image_info,
        .pBufferInfo = nullptr,
        .pTexelBufferView = nullptr
    });

    return *this;
}

// =============================================================================
DescriptorSet & DescriptorSet::allocate(DescriptorPool const &descriptor_pool,
                                        DescriptorSetLayout const &set_layout)
{
    _layout = set_layout.native();

    const vk::DescriptorSetAllocateInfo alloc_info {
        .descriptorPool = descriptor_pool.native(),
        .descriptorSetCount = 1u,
        .pSetLayouts = &set_layout.native()
    };

    auto const result = LogicalDevice::native().allocateDescriptorSets(
        &alloc_info,
        &_set
    );

    if(result != vk::Result::eSuccess) {
        CONSOLE_CRITICAL("Could not allocate descriptor sets");
    }

    return *this;
}

// =============================================================================
void DescriptorSet::write_set() {
    if(_set_writes.empty()) {
        CONSOLE_ERROR("Trying to update descriptor set with no set writes.");
        return;
    }

    LogicalDevice::native().updateDescriptorSets(_set_writes, nullptr);

    _buffer_info.clear();
    _image_info.clear();
    _set_writes.clear();
}

// =============================================================================
DescriptorSet::DescriptorSet() :
    _buffer_info { },
    _image_info  { },
    _set_writes  { },
    _layout      { },
    _set         { }
{ }

DescriptorSet::DescriptorSet(DescriptorSet &&other) noexcept :
    _set_writes { std::move(other._set_writes) },
    _layout     { std::move(other._layout) },
    _set        { std::move(other._set) }
{
    other._set_writes.clear();
    other._layout = nullptr;
    other._set = nullptr;
}

} // namespace vkl
#include "vklearnin/vklearnin.hpp"
#include "vklearnin/rendering/descriptors/DescriptorSet.hpp"

#include "vklearnin/rendering/descriptors/DescriptorPool.hpp"
#include "vklearnin/rendering/descriptors/DescriptorSetLayout.hpp"
#include "vklearnin/system/devices/LogicalDevice.hpp"

namespace vkl {

// =============================================================================
DescriptorSet & DescriptorSet::add_buffer(BufferObject const &buffer) {
    _buffers.push_back(buffer);
    return *this;
}

// =============================================================================
DescriptorSet & DescriptorSet::add_image(ImageObject const &image) {
    _images.push_back(image);
    return *this;
}

// =============================================================================
void DescriptorSet::write_set() {
    std::vector<vk::DescriptorBufferInfo> buffer_info;
    buffer_info.reserve(_buffers.size());

    for(auto const& buffer : _buffers) {
        buffer_info.push_back({
            .buffer = buffer.handle,
            .offset = 0u,
            .range = VK_WHOLE_SIZE
        });
    }

    _buffers.clear();

    std::vector<vk::DescriptorImageInfo> image_info;
    image_info.reserve(_images.size());

    for(auto const& image : _images) {
        image_info.push_back({
            .sampler     = image.sampler,
            .imageView   = image.view,
            .imageLayout = image.layout
        });
    }

    _images.clear();

    auto const buffer_count = static_cast<uint32_t>(buffer_info.size());
    auto const image_count = static_cast<uint32_t>(image_info.size());

    std::vector<vk::WriteDescriptorSet> set_writes;
    set_writes.reserve(2);

    if(buffer_count > 0) {
        set_writes.push_back({
            .dstSet = _set,
            .dstBinding = 0u,
            .dstArrayElement = 0u,
            .descriptorCount = buffer_count,
            .descriptorType = vk::DescriptorType::eUniformBuffer,
            .pImageInfo = nullptr,
            .pBufferInfo = buffer_info.data(),
            .pTexelBufferView = nullptr
        });
    }

    if(image_count > 0) {
        set_writes.push_back({
            .dstSet = _set,
            .dstBinding = 0u,
            .dstArrayElement = 0u,
            .descriptorCount = image_count,
            .descriptorType = vk::DescriptorType::eCombinedImageSampler,
            .pImageInfo = image_info.data(),
            .pBufferInfo = nullptr,
            .pTexelBufferView = nullptr
        });
    }

    LogicalDevice::native().updateDescriptorSets(set_writes, nullptr);
}

// =============================================================================
void DescriptorSet::create(DescriptorPool const &descriptor_pool,
                           DescriptorSetLayout const &set_layout)
{
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
        return;
    }
}

// =============================================================================
void DescriptorSet::destroy() {
    for(auto &buffer : _buffers) {
        BufferTools::destroy(buffer);
    }
    for(auto &image : _images) {
        ImageTools::destroy(image);
    }
}

// =============================================================================
DescriptorSet::DescriptorSet() :
    _buffers { },
    _images  { },
    _set     { }
{ }

DescriptorSet::DescriptorSet(DescriptorSet &&other) noexcept :
    _buffers  { std::move(other._buffers) },
    _images   { std::move(other._images) },
    _set      { other._set  }
{
    other._buffers.clear();
    other._images.clear();
    other._set = nullptr;
}

} // namespace vkl
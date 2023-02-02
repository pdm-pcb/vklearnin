#include "vklearnin/vklearnin.hpp"
#include "vklearnin/rendering/descriptors/DescriptorSet.hpp"

#include "vklearnin/rendering/descriptors/DescriptorPool.hpp"
#include "vklearnin/rendering/descriptors/DescriptorSetLayout.hpp"
#include "vklearnin/system/devices/LogicalDevice.hpp"

namespace vkl {

// =============================================================================
void DescriptorSet::add_ubo(const BufferObject &ubo) {
    _ubos.push_back(ubo);
}

// =============================================================================
void DescriptorSet::add_texture2D(const ImageObject &texture) {
    _textures.push_back(texture);
}

// =============================================================================
void DescriptorSet::create(const DescriptorPool &descriptor_pool,
                           const DescriptorSetLayout &set_layout)
{
    const vk::DescriptorSetAllocateInfo alloc_info {
        .descriptorPool = descriptor_pool.native(),
        .descriptorSetCount = 1u,
        .pSetLayouts = &set_layout.native()
    };

    auto result = LogicalDevice::native().allocateDescriptorSets(
        &alloc_info,
        &_set
    );
    if(result != vk::Result::eSuccess) {
        CONSOLE_CRITICAL("Could not allocate descriptor sets");
        return;
    }

    std::vector<vk::DescriptorBufferInfo> buffer_info;
    buffer_info.reserve(_ubos.size());

    for(auto const& ubo : _ubos) {
        buffer_info.push_back({
            .buffer = ubo.handle,
            .offset = 0u,
            .range = VK_WHOLE_SIZE
        });
    }

    std::vector<vk::DescriptorImageInfo> image_info;
    image_info.reserve(_textures.size());

    for(auto const& texture : _textures) {
        image_info.push_back({
            .sampler     = texture.sampler,
            .imageView   = texture.view,
            .imageLayout = texture.layout
        });
    }

    std::vector<vk::WriteDescriptorSet> set_writes;

    if(!buffer_info.empty()) {
        set_writes.push_back({
            .dstSet = _set,
            .dstBinding = 0u,
            .dstArrayElement = 0u,
            .descriptorCount = static_cast<uint32_t>(buffer_info.size()),
            .descriptorType = vk::DescriptorType::eUniformBuffer,
            .pImageInfo = nullptr,
            .pBufferInfo = buffer_info.data(),
            .pTexelBufferView = nullptr
        });
    }

    if(!image_info.empty()) {
        set_writes.push_back({
            .dstSet = _set,
            .dstBinding = 1u,
            .dstArrayElement = 0u,
            .descriptorCount = static_cast<uint32_t>(image_info.size()),
            .descriptorType = vk::DescriptorType::eCombinedImageSampler,
            .pImageInfo = image_info.data(),
            .pBufferInfo = nullptr,
            .pTexelBufferView = nullptr
        });
    }

    LogicalDevice::native().updateDescriptorSets(set_writes, nullptr);
}

// =============================================================================
void DescriptorSet::destroy() {
    for(auto &buffer : _ubos) {
        BufferTools::destroy(buffer);
    }
}

// =============================================================================
DescriptorSet::DescriptorSet() :
    _ubos     { },
    _textures { },
    _set      { }
{ }

DescriptorSet::DescriptorSet(DescriptorSet &&other) noexcept :
    _ubos     { std::move(other._ubos) },
    _textures { std::move(other._textures) },
    _set      { other._set  }
{
    other._ubos.clear();
    other._textures.clear();
    other._set = nullptr;
}

} // namespace vkl
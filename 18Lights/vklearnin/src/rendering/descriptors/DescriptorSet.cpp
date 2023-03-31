#include "vklearnin/vklearnin.hpp"
#include "vklearnin/rendering/descriptors/DescriptorSet.hpp"

#include "vklearnin/rendering/descriptors/DescriptorPool.hpp"
#include "vklearnin/system/devices/LogicalDevice.hpp"

namespace vkl {

// =============================================================================
DescriptorSet & DescriptorSet::add_ubo(BufferObject const &buffer) {
    _ubos.push_back(buffer);
    return *this;
}

// =============================================================================
DescriptorSet & DescriptorSet::add_combined_sampler(ImageObject const &image) {
    _combined_samplers.push_back(image);
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
    auto const ubo_info = _get_ubo_info();
    auto const combined_sampler_info = _get_combined_sampler_info();
    auto const ssbo_info = _get_ssbo_info();

    auto const ubo_count = static_cast<uint32_t>(ubo_info.size());
    auto const combined_count =
        static_cast<uint32_t>(combined_sampler_info.size());
    auto const ssbo_count = static_cast<uint32_t>(ssbo_info.size());

    std::vector<vk::WriteDescriptorSet> set_writes;
    set_writes.reserve(ubo_count + combined_count + ssbo_count);

    if(ubo_count > 0) {
        set_writes.push_back({
            .dstSet = _set,
            .dstBinding = 0u,
            .dstArrayElement = 0u,
            .descriptorCount = ubo_count,
            .descriptorType = vk::DescriptorType::eUniformBuffer,
            .pImageInfo = nullptr,
            .pBufferInfo = ubo_info.data(),
            .pTexelBufferView = nullptr
        });
    }

    if(combined_count > 0) {
        set_writes.push_back({
            .dstSet = _set,
            .dstBinding = 0u,
            .dstArrayElement = 0u,
            .descriptorCount = combined_count,
            .descriptorType = vk::DescriptorType::eCombinedImageSampler,
            .pImageInfo = combined_sampler_info.data(),
            .pBufferInfo = nullptr,
            .pTexelBufferView = nullptr
        });
    }

    if(ssbo_count > 0) {
        set_writes.push_back({
            .dstSet = _set,
            .dstBinding = 0u,
            .dstArrayElement = 0u,
            .descriptorCount = ssbo_count,
            .descriptorType = vk::DescriptorType::eStorageBuffer,
            .pImageInfo = nullptr,
            .pBufferInfo = ssbo_info.data(),
            .pTexelBufferView = nullptr
        });
    }

    if(set_writes.empty()) {
        CONSOLE_ERROR("Trying to update descriptor set with no set writes.");
        return;
    }

    LogicalDevice::native().updateDescriptorSets(set_writes, nullptr);
}

// =============================================================================
DescriptorSet::BufferInfoList DescriptorSet::_get_ubo_info() {
    std::vector<vk::DescriptorBufferInfo> info;
    info.reserve(_ubos.size());

    for(auto const& buffer : _ubos) {
        info.push_back({
            .buffer = buffer.handle,
            .offset = 0u,
            .range = VK_WHOLE_SIZE
        });
    }

    _ubos.clear();

    return info;
}

// =============================================================================
DescriptorSet::ImageInfoList DescriptorSet::_get_combined_sampler_info() {
    std::vector<vk::DescriptorImageInfo> info;
    info.reserve(_combined_samplers.size());

    for(auto const& image : _combined_samplers) {
        info.push_back({
            .sampler     = image.sampler,
            .imageView   = image.view,
            .imageLayout = image.layout
        });
    }

    _combined_samplers.clear();

    return info;
}

// =============================================================================
DescriptorSet::BufferInfoList DescriptorSet::_get_ssbo_info() {
    std::vector<vk::DescriptorBufferInfo> info;
    info.reserve(_ssbos.size());

    for(auto const& buffer : _ssbos) {
        info.push_back({
            .buffer = buffer.handle,
            .offset = 0u,
            .range = VK_WHOLE_SIZE
        });
    }

    _ssbos.clear();

    return info;
}

// =============================================================================
DescriptorSet::DescriptorSet() :
    _ubos              { },
    _ssbos             { },
    _combined_samplers { },
    _set               { }
{ }

DescriptorSet::DescriptorSet(DescriptorSet &&other) noexcept :
    _ubos              { std::move(other._ubos) },
    _ssbos             { std::move(other._ssbos) },
    _combined_samplers { std::move(other._combined_samplers) },
    _set               { other._set }
{
    other._ubos.clear();
    other._ssbos.clear();
    other._combined_samplers.clear();
    other._set = nullptr;
}

} // namespace vkl
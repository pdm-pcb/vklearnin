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

    for(const auto &ubo : _ubos) {
        buffer_info.push_back({
            .buffer = ubo.handle,
            .offset = 0u,
            .range = VK_WHOLE_SIZE
        });
    }

    std::vector<vk::WriteDescriptorSet> set_writes;

    if(buffer_info.size() > 0) {
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
    _ubos { },
    _set  { }
{ }

DescriptorSet::DescriptorSet(DescriptorSet &&other) noexcept :
    _ubos { std::move(other._ubos) },
    _set  { other._set  }
{
    other._ubos.clear();
    other._set = nullptr;
}

} // namespace vkl
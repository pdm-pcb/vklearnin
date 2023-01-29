#include "vklearnin/vklearnin.hpp"
#include "vklearnin/rendering/descriptors/DescriptorSet.hpp"

#include "vklearnin/rendering/descriptors/DescriptorPool.hpp"
#include "vklearnin/rendering/descriptors/DescriptorSetLayout.hpp"
#include "vklearnin/resources/buffers/BufferTools.hpp"
#include "vklearnin/system/devices/LogicalDevice.hpp"

namespace vkl {

// =============================================================================
DescriptorSet::UBOIter DescriptorSet::add_ubo(const size_t size) {
    Buffer ubo {
        .size = size
    };

    BufferTools::create(
        vk::BufferUsageFlagBits::eUniformBuffer,
        (vk::MemoryPropertyFlagBits::eHostVisible |
         vk::MemoryPropertyFlagBits::eHostCoherent),
         ubo
    );

    CONSOLE_TRACE(
        "Created UBO {:#x}",
        reinterpret_cast<uint64_t>(VkBuffer(ubo.handle))
    );

    _ubos.push_back(ubo);

    return std::prev(_ubos.end());
}

// =============================================================================
void DescriptorSet::update_ubo(const UBOIter &buffer, const void *data) {
    auto &ubo = *buffer;

    auto result = LogicalDevice::native().mapMemory(ubo.memory, 0u, ubo.size);
    if(result.result != vk::Result::eSuccess) {
        CONSOLE_CRITICAL(
            "Unable to map UBO {:#x}: '{}'",
            reinterpret_cast<uint64_t>(VkBuffer(ubo.handle)),
            to_string(result.result)
        );
        return;
    }

    memcpy(result.value, data, ubo.size);
    LogicalDevice::native().unmapMemory(ubo.memory);
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
    }

    std::vector<vk::WriteDescriptorSet>   set_writes;
    std::vector<vk::DescriptorBufferInfo> buffer_info;

    if(!_ubos.empty()) {
        buffer_info.reserve(_ubos.size());
        for(const auto &buffer : _ubos) {
            buffer_info.push_back({
                .buffer = buffer.handle,
                .offset = 0u,
                .range = VK_WHOLE_SIZE
            });
        }

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
    _ubos   { },
    _set    { }
{ }

DescriptorSet::DescriptorSet(DescriptorSet &&other) :
    _ubos { other._ubos },
    _set  { other._set  }
{
    other._ubos.clear();
    other._set = nullptr;
}

} // namespace vkl
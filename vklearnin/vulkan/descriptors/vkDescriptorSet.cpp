#include "vklearnin/vklearnin.hpp"
#include "vklearnin/vulkan/descriptors/vkDescriptorSet.hpp"

#include "vklearnin/vulkan/descriptors/vkDescriptorSetLayout.hpp"
#include "vklearnin/vulkan/descriptors/vkDescriptorPool.hpp"
#include "vklearnin/vulkan/devices/vkDevice.hpp"
#include "vklearnin/vulkan/pipelines/vkGraphicsPipeline.hpp"
#include "vklearnin/vulkan/pipelines/vkComputePipeline.hpp"
#include "vklearnin/vulkan/devices/vkCmdBuffer.hpp"

namespace vkl {

// =============================================================================
vkDescriptorSet::vkDescriptorSet(vkDescriptorSet &&other) :
    _handle  { other._handle },
    _device  { other._device },
    _layout  { other._layout },
    _updates { other._updates }
{
    other._handle = nullptr;
    other._device = nullptr;
    other._layout = nullptr;
    other._updates.clear();
}

// =============================================================================
bool vkDescriptorSet::allocate(vkDescriptorSetLayout const &layout,
                               vkDescriptorPool const &pool,
                               vkDevice const &device)
{
    if(_handle) {
        Log::error("Descriptor set {} already allocated.", _handle);
        return false;
    }

    if(!device.native()) {
        Log::error("Cannot allocate descriptor sets with invalid "
                          "device.");
        return false;
    }

    _device = device.native();
    _layout = layout.native();

    vk::DescriptorSetAllocateInfo const alloc_info {
        .descriptorPool = pool.native(),
        .descriptorSetCount = 1u,
        .pSetLayouts = &_layout,
    };

    _handle = _device.allocateDescriptorSets(alloc_info)[0];
    Log::trace("Allocated descriptor set {}", _handle);

    return true;
}

// =============================================================================
vkDescriptorSet &
vkDescriptorSet::add_update(vk::DescriptorBufferInfo const &update,
                            uint32_t const binding,
                            vk::DescriptorType const descriptor_type)
{
    _updates.emplace_back(vk::WriteDescriptorSet {
        .dstSet = _handle,
        .dstBinding = binding,
        .dstArrayElement = 0u,
        .descriptorCount = 1u,
        .descriptorType = descriptor_type,
        .pImageInfo = nullptr,
        .pBufferInfo = &update,
        .pTexelBufferView = nullptr,
    });

    return *this;
}

// =============================================================================
vkDescriptorSet &
vkDescriptorSet::add_update(vk::DescriptorImageInfo const &update,
                            uint32_t const binding,
                            vk::DescriptorType const descriptor_type)
{
    _updates.emplace_back(vk::WriteDescriptorSet {
        .dstSet = _handle,
        .dstBinding = binding,
        .dstArrayElement = 0u,
        .descriptorCount = 1u,
        .descriptorType = descriptor_type,
        .pImageInfo = &update,
        .pBufferInfo = nullptr,
        .pTexelBufferView = nullptr,
    });

    return *this;
}

// =============================================================================
void vkDescriptorSet::update() {
    _device.updateDescriptorSets(
        static_cast<uint32_t>(_updates.size()), // write count
        _updates.data(),                        // writes
        0u,                                     // copy count
        nullptr                                 // copies
    );

    _updates.clear();
}

// =============================================================================
void vkDescriptorSet::bind(vkGraphicsPipeline const &pipeline,
                           uint32_t const set_number,
                           vkCmdBuffer const &cmd_buffer) const
{
    if(!_updates.empty()) {
        Log::warn("Binding graphics descriptor set {} with {} unwritten updates.",
                  _handle,
                  _updates.size());
    }

    cmd_buffer.native().bindDescriptorSets(
        vk::PipelineBindPoint::eGraphics, // bind point
        pipeline.layout(),                // pipeline layout
        set_number,                       // first descriptor set
        1u,                               // descriptor set count
        &_handle,                         // descriptor sets
        0u,                               // dynamic offset count
        nullptr                           // dynamic offsets
    );
}

// =============================================================================
void vkDescriptorSet::bind(vkComputePipeline const &pipeline,
                           uint32_t const set_number,
                           vkCmdBuffer const &cmd_buffer) const
{
    if(!_updates.empty()) {
        Log::warn("Binding compute descriptor set {} with {} unwritten updates.",
                  _handle,
                  _updates.size());
    }

    cmd_buffer.native().bindDescriptorSets(
        vk::PipelineBindPoint::eCompute, // bind point
        pipeline.layout(),               // pipeline layout
        set_number,                      // first descriptor set
        1u,                              // descriptor set count
        &_handle,                        // descriptor sets
        0u,                              // dynamic offset count
        nullptr                          // dynamic offsets
    );
}

} // namespace vkl
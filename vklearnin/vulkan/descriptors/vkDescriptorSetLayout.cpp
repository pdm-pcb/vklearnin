#include "vklearnin/vklearnin.hpp"
#include "vklearnin/vulkan/descriptors/vkDescriptorSetLayout.hpp"

#include "vklearnin/vulkan/devices/vkDevice.hpp"

namespace vkl {

// =============================================================================
vkDescriptorSetLayout & vkDescriptorSetLayout::add_binding(
    uint32_t const binding,
    vk::DescriptorType const type,
    uint32_t const descriptor_count,
    vk::ShaderStageFlagBits const stage_flags
)
{
    _bindings.emplace_back(vk::DescriptorSetLayoutBinding {
        .binding = binding,
        .descriptorType = type,
        .descriptorCount = descriptor_count,
        .stageFlags = stage_flags,
        .pImmutableSamplers = nullptr,
    });

    return *this;
}

// =============================================================================
bool vkDescriptorSetLayout::create(vkDevice const &device) {
    if(_handle) {
        Log::error("Descriptor set layout {} already exists", _handle);
        return false;
    }

    if(!device.native()) {
        Log::error("Cannot create descriptor set layout with invalid device.");
        return false;
    }

    _device = device.native();

    vk::DescriptorSetLayoutCreateInfo const create_info {
        .flags = { },
        .bindingCount = static_cast<uint32_t>(_bindings.size()),
        .pBindings = _bindings.data(),
    };

    _handle = _device.createDescriptorSetLayout(create_info);
    Log::trace("Created descriptor set layout {}", _handle);

    return true;
}

// =============================================================================
bool vkDescriptorSetLayout::destroy() {
    if(!_handle) {
        Log::error("Must create descriptor set layout before calling destroy.");
        return false;
    }

    Log::trace("Destroying descriptor set layout {}", _handle);
    _device.destroy(_handle);

    _handle = nullptr;
    _device = nullptr;
    _bindings.clear();

    return true;
}

} // namespace vkl
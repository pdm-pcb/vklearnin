#include "vklearnin/vklearnin.hpp"
#include "vklearnin/rendering/descriptors/DescriptorSetLayout.hpp"

#include "vklearnin/system/devices/LogicalDevice.hpp"

namespace vkl {

// =============================================================================
DescriptorSetLayout &
DescriptorSetLayout::add_binding(vk::DescriptorType const type,
                                 vk::ShaderStageFlags const stages,
                                 uint32_t const descriptor_count)
{
    if(_layout) {
        CONSOLE_CRITICAL("Cannot add binding to existing descriptor set "
                         "layout.");
    }

    auto const binding = static_cast<uint32_t>(_bindings.size());
    _bindings.push_back({
        .binding         = binding,
        .descriptorType  = type,
        .descriptorCount = descriptor_count,
        .stageFlags      = stages,
    });

    CONSOLE_TRACE(
        "Adding Descriptor type: {} binding: {} ",
        to_string(type),
        _bindings.back().binding
    );

    return *this;
}

// =============================================================================
void DescriptorSetLayout::create() {
    if(_layout) {
        CONSOLE_CRITICAL("Destroy this descriptor set layout before trying to "
                         "recreate it.");
        return;
    }
    const vk::DescriptorSetLayoutCreateInfo descriptor_info {
        .bindingCount = static_cast<uint32_t>(_bindings.size()),
        .pBindings    = _bindings.data(),
    };

    auto desc_set_result = LogicalDevice::native().createDescriptorSetLayout(
        &descriptor_info,
        nullptr,
        &_layout
    );

    if(desc_set_result != vk::Result::eSuccess) {
        CONSOLE_CRITICAL("Unable to create descriptor set layout");
    }

    CONSOLE_TRACE("Created descriptor set layout {:#x}",
                  reinterpret_cast<uint64_t>(VkDescriptorSetLayout(_layout)));
}

// =============================================================================
void DescriptorSetLayout::destroy() {
    CONSOLE_TRACE("Destroying descriptor set layout {:#x}",
                  reinterpret_cast<uint64_t>(VkDescriptorSetLayout(_layout)));
    LogicalDevice::native().destroyDescriptorSetLayout(_layout);
}

// =============================================================================
DescriptorSetLayout::DescriptorSetLayout() :
    _bindings     { },
    _layout       { }
{ }

} // namespace vkl
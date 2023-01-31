#include "vklearnin/vklearnin.hpp"
#include "vklearnin/rendering/descriptors/DescriptorSetLayout.hpp"

#include "vklearnin/system/devices/LogicalDevice.hpp"

namespace vkl {

// =============================================================================
void DescriptorSetLayout::add_binding(
    const vk::DescriptorSetLayoutBinding &binding)
{
    _bindings.push_back(binding);

    CONSOLE_TRACE(
        "Adding Descriptor Type: {} Binding: {} ",
        to_string(binding.descriptorType),
        binding.binding
    );
}

// =============================================================================
void DescriptorSetLayout::create() {
    const vk::DescriptorSetLayoutCreateInfo descriptor_info {
        .bindingCount = static_cast<uint32_t>(_bindings.size()),
        .pBindings = _bindings.data(),
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
    _bindings { },
    _layout   { }
{ }

} // namespace vkl
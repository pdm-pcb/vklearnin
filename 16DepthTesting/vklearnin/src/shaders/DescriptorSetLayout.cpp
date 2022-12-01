#include "vklearnin/vklearnin.hpp"
#include "vklearnin/shaders/DescriptorSetLayout.hpp"

#include "vklearnin/rendering/devices/LogicalDevice.hpp"

namespace vkl {

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

void DescriptorSetLayout::create() {
    if(_bindings.size() == 0) {
        CONSOLE_CRITICAL("Cannot create an empty layout.");
    }

    vk::DescriptorSetLayoutCreateInfo descriptor_info {
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
}

void DescriptorSetLayout::destroy() {
    LogicalDevice::native().destroyDescriptorSetLayout(_layout);
}

DescriptorSetLayout::DescriptorSetLayout() :
    _layout { nullptr }
{ }

DescriptorSetLayout::DescriptorSetLayout(DescriptorSetLayout &&other) :
    _bindings { std::move(other._bindings) },
    _layout   { std::move(other._layout) }
{ }

} // namespace vkl
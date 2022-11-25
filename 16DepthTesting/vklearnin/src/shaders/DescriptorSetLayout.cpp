#include "vklearnin/vklearnin.hpp"
#include "vklearnin/shaders/DescriptorSetLayout.hpp"

#include "vklearnin/rendering/devices/LogicalDevice.hpp"

namespace vkl {

void DescriptorSetLayout::create(const BindingList &bindings,
                                 const BindingFlags &flags)
{
    assert(bindings.size() == flags.size());

    vk::DescriptorSetLayoutBindingFlagsCreateInfo binding_flags {
        .bindingCount = static_cast<uint32_t>(bindings.size()),
        .pBindingFlags = flags.data(),
    };

    vk::DescriptorSetLayoutCreateInfo descriptor_info {
        // .pNext = &binding_flags,
        .bindingCount = static_cast<uint32_t>(bindings.size()),
        .pBindings = bindings.data(),
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
    _layout { std::move(other._layout) }
{ }

} // namespace vkl
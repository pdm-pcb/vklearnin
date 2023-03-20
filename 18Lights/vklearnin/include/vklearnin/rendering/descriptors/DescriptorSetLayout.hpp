#ifndef VKLEARNIN_RENDERING_DESCRIPTORS_DESCRIPTORSETLAYOUT_HPP
#define VKLEARNIN_RENDERING_DESCRIPTORS_DESCRIPTORSETLAYOUT_HPP

#include "vklearnin/system/pch.hpp"

namespace vkl {

class DescriptorSetLayout {
public:
    auto & add_binding(vk::DescriptorType const type,
                       vk::ShaderStageFlags const stages,
                       uint32_t const descriptor_count = 1u);

    void create();
    void destroy();

    inline auto const& native()   const { return _layout;   }
    inline auto const& bindings() const { return _bindings; }

    DescriptorSetLayout();
    ~DescriptorSetLayout() = default;

    DescriptorSetLayout(DescriptorSetLayout &&) = delete;
    DescriptorSetLayout(const DescriptorSetLayout &) = delete;

    DescriptorSetLayout& operator=(DescriptorSetLayout &&) = delete;
    DescriptorSetLayout& operator=(const DescriptorSetLayout &) = delete;

private:
    std::vector<vk::DescriptorSetLayoutBinding> _bindings;
    vk::DescriptorSetLayout _layout;
};

} // namespace vkl

#endif // VKLEARNIN_RENDERING_DESCRIPTORS_DESCRIPTORSETLAYOUT_HPP
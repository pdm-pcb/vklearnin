#ifndef VKLEARNIN_RENDERING_DESCRIPTORS_DESCRIPTORSETLAYOUT_HPP
#define VKLEARNIN_RENDERING_DESCRIPTORS_DESCRIPTORSETLAYOUT_HPP

#include "vklearnin/system/pch.hpp"

namespace vkl {

class DescriptorSetLayout {
public:
    using DescBindings = std::vector<vk::DescriptorSetLayoutBinding>;
    void add_binding(const vk::DescriptorSetLayoutBinding &binding);

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
    vk::DescriptorSetLayout _layout;
    DescBindings            _bindings;
};

} // namespace vkl

#endif // VKLEARNIN_RENDERING_DESCRIPTORS_DESCRIPTORSETLAYOUT_HPP
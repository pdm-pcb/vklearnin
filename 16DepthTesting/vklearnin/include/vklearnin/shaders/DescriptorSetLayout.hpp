#ifndef VKLEARNIN_SHADERS_DESCRIPTORSETLAYOUT_HPP
#define VKLEARNIN_SHADERS_DESCRIPTORSETLAYOUT_HPP

#include "vklearnin/system/pch.hpp"

namespace vkl {

class DescriptorSetLayout {
public:
    void add_binding(const vk::DescriptorSetLayoutBinding &binding);

    void create();
    void destroy();

    inline const auto & bindings() const { return _bindings; }
    inline const auto & native()   const { return _layout;   }

    DescriptorSetLayout();
    ~DescriptorSetLayout() = default;
    
    DescriptorSetLayout(DescriptorSetLayout &&other);
    DescriptorSetLayout(const DescriptorSetLayout &other) = delete;
    
    DescriptorSetLayout & operator=(DescriptorSetLayout &&other) = delete;
    DescriptorSetLayout & operator=(const DescriptorSetLayout &other) = delete;

private:
    BindingList             _bindings;
    vk::DescriptorSetLayout _layout;
};

} // namespace vkl

#endif // VKLEARNIN_SHADERS_DESCRIPTORSETLAYOUT_HPP
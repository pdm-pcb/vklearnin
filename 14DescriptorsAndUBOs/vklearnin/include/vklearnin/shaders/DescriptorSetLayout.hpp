#ifndef VKLEARNIN_SHADERS_DESCRIPTORSETLAYOUT_HPP
#define VKLEARNIN_SHADERS_DESCRIPTORSETLAYOUT_HPP

#include "vklearnin/system/pch.hpp"

namespace vkl {

class DescriptorSetLayout {
public:
    using Bindings = std::vector<vk::DescriptorSetLayoutBinding>;

    void create(const Bindings &bindings);
    void destroy();

    inline const auto & native() const { return _layout; }

    DescriptorSetLayout();
    ~DescriptorSetLayout() = default;
    
    DescriptorSetLayout(DescriptorSetLayout &&other);
    DescriptorSetLayout(const DescriptorSetLayout &other) = delete;
    
    DescriptorSetLayout & operator=(DescriptorSetLayout &&other) = delete;
    DescriptorSetLayout & operator=(const DescriptorSetLayout &other) = delete;

private:
    vk::DescriptorSetLayout _layout;
};

} // namespace vkl

#endif // VKLEARNIN_SHADERS_DESCRIPTORSETLAYOUT_HPP
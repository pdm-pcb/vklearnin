#ifndef VKLEARNIN_RENDERING_DESCRIPTORS_DESCRIPTORSETLAYOUT_HPP
#define VKLEARNIN_RENDERING_DESCRIPTORS_DESCRIPTORSETLAYOUT_HPP

#include "vklearnin/system/pch.hpp"

namespace vkl {

class DescriptorSetLayout {
public:
    DescriptorSetLayout();
    ~DescriptorSetLayout() = default;

    DescriptorSetLayout(DescriptorSetLayout &&) = delete;
    DescriptorSetLayout(const DescriptorSetLayout &) = delete;

    DescriptorSetLayout& operator=(DescriptorSetLayout &&) = delete;
    DescriptorSetLayout& operator=(const DescriptorSetLayout &) = delete;
};

} // namespace vkl

#endif // VKLEARNIN_RENDERING_DESCRIPTORS_DESCRIPTORSETLAYOUT_HPP
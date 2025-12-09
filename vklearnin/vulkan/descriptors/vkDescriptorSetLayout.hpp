#ifndef VKLEARNIN_VULKAN_VKDESCRIPTORSETLAYOUT_HPP
#define VKLEARNIN_VULKAN_VKDESCRIPTORSETLAYOUT_HPP

#include "vklearnin/pch.hpp"

namespace vkl {

class vkDevice;

class vkDescriptorSetLayout final {
public:
    vkDescriptorSetLayout() = default;
    ~vkDescriptorSetLayout() = default;

    vkDescriptorSetLayout(vkDescriptorSetLayout &&) = delete;
    vkDescriptorSetLayout(vkDescriptorSetLayout const &) = delete;

    vkDescriptorSetLayout & operator=(vkDescriptorSetLayout &&) = delete;
    vkDescriptorSetLayout & operator=(vkDescriptorSetLayout const &) = delete;

    vkDescriptorSetLayout & add_binding(
        uint32_t const binding,
        vk::DescriptorType const type,
        uint32_t const descriptor_count,
        vk::ShaderStageFlagBits const stage_flags
    );

    bool create(vkDevice const &device);
    bool destroy();

    [[nodiscard]] inline auto const & native() const { return _handle; }

private:
    vk::DescriptorSetLayout _handle { nullptr };
    vk::Device _device { nullptr };

    std::vector<vk::DescriptorSetLayoutBinding> _bindings;
};

} // namespace vkl

#endif // VKLEARNIN_VULKAN_VKDESCRIPTORSETLAYOUT_HPP
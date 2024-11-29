#ifndef VKLEARNIN_VULKAN_VKDESCRIPTORPOOL_HPP
#define VKLEARNIN_VULKAN_VKDESCRIPTORPOOL_HPP

#include "vklearnin/pch.hpp"

namespace vkl {

class vkDevice;

class vkDescriptorPool final {
public:
    vkDescriptorPool() = default;
    ~vkDescriptorPool() = default;

    vkDescriptorPool(vkDescriptorPool &&) = delete;
    vkDescriptorPool(vkDescriptorPool const &) = delete;

    vkDescriptorPool & operator=(vkDescriptorPool &&) = delete;
    vkDescriptorPool & operator=(vkDescriptorPool const &) = delete;

    vkDescriptorPool & set_type_count(vk::DescriptorType const type,
                                      uint32_t const count);

    bool create(uint32_t const max_sets, vkDevice const &device);
    bool destroy();

    inline auto const & native() const { return _handle; }

private:
    vk::DescriptorPool _handle { nullptr };
    vk::Device _device { nullptr };

    std::unordered_map<vk::DescriptorType, uint32_t> _type_counts;
};

} // namespace vkl

#endif // VKLEARNIN_VULKAN_VKDESCRIPTORPOOL_HPP
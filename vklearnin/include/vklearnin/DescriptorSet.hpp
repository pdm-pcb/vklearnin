#ifndef VKLEARNIN_DESCRIPTORSET_HPP
#define VKLEARNIN_DESCRIPTORSET_HPP

#include <vulkan/vulkan.h>

#include <vector>

class UniformBufferObject;

class DescriptorSet {
public:
    void init_layout();
    void init_pool();
    void init_sets(UniformBufferObject &ubo);


    inline ::VkDescriptorSetLayout layout() const { return _layout; }
    inline const std::vector<::VkDescriptorSet> & sets() const { return _sets; }

    DescriptorSet(const uint32_t frames_in_flight, const ::VkDevice &device);
    ~DescriptorSet();

private:
    ::VkDescriptorSetLayout        _layout;
    ::VkDescriptorPool             _pool;
    std::vector<::VkDescriptorSet> _sets;

    const uint32_t _max_images;
    const ::VkDevice &_device;
};

#endif // VKLEARNIN_DESCRIPTORSET_HPP

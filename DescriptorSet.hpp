#ifndef VKL_DESCRIPTORSET_HPP
#define VKL_DESCRIPTORSET_HPP

#include <vulkan/vulkan.h>

#include <vector>

class UniformBufferObject;

class DescriptorSet {
public:
    void init_pool();
    void init_sets(UniformBufferObject &ubo);

    const std::vector<::VkDescriptorSet> & sets() const { return _sets; }

    DescriptorSet(const uint32_t frames_in_flight, const ::VkDevice &device);
    ~DescriptorSet();

private:
    ::VkDescriptorPool             _pool;
    std::vector<::VkDescriptorSet> _sets;

    const uint32_t _max_images;
    const ::VkDevice &_device;
};

#endif // VKL_DESCRIPTORSET_HPP

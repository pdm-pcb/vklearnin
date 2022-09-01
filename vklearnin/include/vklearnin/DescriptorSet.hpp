#ifndef VKLEARNIN_DESCRIPTORSET_HPP
#define VKLEARNIN_DESCRIPTORSET_HPP

#include <vulkan/vulkan.hpp>

#include <vector>

class UniformBufferObject;
class Texture2D;

class DescriptorSet {
public:
    void init_layout();
    void init_pool();
    void init_sets(UniformBufferObject &ubo, Texture2D &texture);


    inline vk::DescriptorSetLayout layout() const { return _layout; }
    inline const std::vector<vk::DescriptorSet> & sets() const { return _sets; }

    DescriptorSet(const vk::Device &device);
    ~DescriptorSet();

private:
    vk::DescriptorSetLayout        _layout;
    vk::DescriptorPool             _pool;
    std::vector<vk::DescriptorSet> _sets;

    const vk::Device &_device;
};

#endif // VKLEARNIN_DESCRIPTORSET_HPP

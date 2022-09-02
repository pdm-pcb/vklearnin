#ifndef VKLEARNIN_DESCRIPTORS_PERFRAMEDESCRIPTORS_HPP
#define VKLEARNIN_DESCRIPTORS_PERFRAMEDESCRIPTORS_HPP

#include <vulkan/vulkan.hpp>

#include <vector>

class UniformBufferObject;

class PerFrameDescriptors {
public:
    void init_layout();
    void init_pool();
    void init_sets(UniformBufferObject &ubo);

    inline vk::DescriptorSetLayout layout() const { return _layout; }
    inline const std::vector<vk::DescriptorSet> & sets() const { return _sets; }

    explicit PerFrameDescriptors(const vk::Device &device);
    ~PerFrameDescriptors();

    PerFrameDescriptors() = delete;

    PerFrameDescriptors(PerFrameDescriptors &&other) = delete;
    PerFrameDescriptors(const PerFrameDescriptors &other) = delete;

    PerFrameDescriptors & operator=(PerFrameDescriptors &&other) = delete;
    PerFrameDescriptors & operator=(const PerFrameDescriptors &other) = delete;

private:
    vk::DescriptorSetLayout _layout;
    vk::DescriptorPool      _pool;
    std::vector<vk::DescriptorSet> _sets;

    const vk::Device &_device;
};

#endif // VKLEARNIN_DESCRIPTORS_PERFRAMEDESCRIPTORS_HPP
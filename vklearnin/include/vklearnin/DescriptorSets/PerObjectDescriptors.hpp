#ifndef VKLEARNIN_DESCRIPTORS_PEROBJECTDESCRIPTORS_HPP
#define VKLEARNIN_DESCRIPTORS_PEROBJECTDESCRIPTORS_HPP

#include "vklearnin/pch.hpp"

class UniformBufferObject;

class PerObjectDescriptors {
public:
    void init_layout();
    void init_pool();
    void init_sets(UniformBufferObject &ubo);

    inline vk::DescriptorSetLayout layout() const { return _layout; }
    inline const std::vector<vk::DescriptorSet> &
    sets(const size_t material) const { return _sets[material]; }

    PerObjectDescriptors(const uint32_t object_count, const vk::Device &device);
    ~PerObjectDescriptors();

    PerObjectDescriptors() = delete;

    PerObjectDescriptors(PerObjectDescriptors &&other) = delete;
    PerObjectDescriptors(const PerObjectDescriptors &other) = delete;

    PerObjectDescriptors & operator=(PerObjectDescriptors &&other) = delete;
    PerObjectDescriptors & operator=(const PerObjectDescriptors &other) = delete;

private:
    vk::DescriptorSetLayout                     _layout;
    vk::DescriptorPool                          _pool;
    std::vector<std::vector<vk::DescriptorSet>> _sets;

    const uint32_t _object_count;
    const vk::Device &_device;
};

#endif // VKLEARNIN_DESCRIPTORS_PEROBJECTDESCRIPTORS_HPP
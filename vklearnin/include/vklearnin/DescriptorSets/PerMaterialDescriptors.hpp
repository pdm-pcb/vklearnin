#ifndef VKLEARNIN_DESCRIPTORS_PERMATERIALDESCRIPTORS_HPP
#define VKLEARNIN_DESCRIPTORS_PERMATERIALDESCRIPTORS_HPP

#include "vklearnin/pch.hpp"

class Texture2D;

class PerMaterialDescriptors {
public:
    void init_layout();
    void init_pool();
    void init_sets(const std::vector<Texture2D *> &textures);

    inline vk::DescriptorSetLayout layout() const { return _layout; }
    inline const std::vector<vk::DescriptorSet> &
    sets(const size_t material) const { return _sets[material]; }

    PerMaterialDescriptors(const uint32_t material_count,
                           const vk::Device &device);
    ~PerMaterialDescriptors();

    PerMaterialDescriptors() = delete;

    PerMaterialDescriptors(PerMaterialDescriptors &&other) = delete;
    PerMaterialDescriptors(const PerMaterialDescriptors &other) = delete;

    PerMaterialDescriptors & operator=(PerMaterialDescriptors &&other) = delete;
    PerMaterialDescriptors & operator=(const PerMaterialDescriptors &other) = delete;

private:
    vk::DescriptorSetLayout                     _layout;
    vk::DescriptorPool                          _pool;
    std::vector<std::vector<vk::DescriptorSet>> _sets;

    const uint32_t _material_count;
    const vk::Device &_device;
};

#endif // VKLEARNIN_DESCRIPTORS_PERMATERIALDESCRIPTORS_HPP
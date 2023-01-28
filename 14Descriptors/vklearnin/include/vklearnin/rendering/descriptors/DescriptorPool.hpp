#ifndef VKLEARNIN_RENDERING_DESCRIPTORS_DESCRIPTORPOOL_HPP
#define VKLEARNIN_RENDERING_DESCRIPTORS_DESCRIPTORPOOL_HPP

#include "vklearnin/system/pch.hpp"

namespace vkl {

class DescriptorPool {
public:
    using PoolSizes = std::set<vk::DescriptorPoolSize>;
    void create(const PoolSizes &sizes);
    void destroy();

    inline const auto &native() const { return _pool; }

    DescriptorPool();
    ~DescriptorPool() = default;

    DescriptorPool(DescriptorPool &&) = delete;
    DescriptorPool(const DescriptorPool &) = delete;

    DescriptorPool& operator=(DescriptorPool &&) = delete;
    DescriptorPool& operator=(const DescriptorPool &) = delete;

private:
    vk::DescriptorPool _pool;
};

} // namespace vkl

#endif // VKLEARNIN_RENDERING_DESCRIPTORS_DESCRIPTORPOOL_HPP
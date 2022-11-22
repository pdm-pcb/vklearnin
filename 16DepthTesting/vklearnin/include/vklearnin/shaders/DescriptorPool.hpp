#ifndef VKLEARNIN_SHADERS_DESCRIPTORPOOLS_HPP
#define VKLEARNIN_SHADERS_DESCRIPTORPOOLS_HPP

#include "vklearnin/system/pch.hpp"

namespace vkl {

class DescriptorPool {
public:
    using PoolSizes = std::vector<vk::DescriptorPoolSize>;

    void create(const PoolSizes &sizes);
    void destroy();

    inline const auto &native() const { return _pool; }

    DescriptorPool() = default;
    ~DescriptorPool() = default;

    DescriptorPool(DescriptorPool &&);
    DescriptorPool(const DescriptorPool &) = delete;

    DescriptorPool & operator=(DescriptorPool &&) = delete;
    DescriptorPool & operator=(const DescriptorPool &) = delete;

private:
    vk::DescriptorPool _pool;
};

} // namespace vkl

#endif // VKLEARNIN_SHADERS_DESCRIPTORPOOLS_HPP
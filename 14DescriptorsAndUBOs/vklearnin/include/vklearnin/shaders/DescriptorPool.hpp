#ifndef VKLEARNIN_SHADERS_DESCRIPTORPOOL_HPP
#define VKLEARNIN_SHADERS_DESCRIPTORPOOL_HPP

#include "vklearnin/system/pch.hpp"

namespace vkl {

class DescriptorPool {
public:
    void create(const vk::DescriptorPoolSize &size);
    void destroy();

    inline const auto &native() const { return _pool; }

    DescriptorPool();
    ~DescriptorPool() = default;

    DescriptorPool(DescriptorPool &&);
    DescriptorPool(const DescriptorPool &) = delete;

    DescriptorPool & operator=(DescriptorPool &&) = delete;
    DescriptorPool & operator=(const DescriptorPool &) = delete;

private:
    vk::DescriptorPool _pool;
};

} // namespace vkl

#endif // VKLEARNIN_SHADERS_DESCRIPTORPOOL_HPP
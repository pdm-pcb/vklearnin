#include "vklearnin/vklearnin.hpp"
#include "vklearnin/shaders/DescriptorPool.hpp"

#include "vklearnin/rendering/devices/LogicalDevice.hpp"

namespace vkl {

//==============================================================================
void DescriptorPool::create(const vk::DescriptorPoolSize &size) {
    vk::DescriptorPoolCreateInfo pool_info {
        .maxSets = 10u,
        .poolSizeCount = 1u,
        .pPoolSizes = &size
    };

    auto result = LogicalDevice::native().createDescriptorPool(
        &pool_info,
        nullptr,
        &_pool
    );
    if(result != vk::Result::eSuccess) {
        CONSOLE_CRITICAL("Failed to create descriptor pool");
    }
}

//==============================================================================
void DescriptorPool::destroy() {
    LogicalDevice::native().destroyDescriptorPool(_pool);
}

//==============================================================================
DescriptorPool::DescriptorPool() :
    _pool { nullptr }
{ }

DescriptorPool::DescriptorPool(DescriptorPool &&other) :
    _pool { std::move(other._pool) }
{ }

} // namespace vkl
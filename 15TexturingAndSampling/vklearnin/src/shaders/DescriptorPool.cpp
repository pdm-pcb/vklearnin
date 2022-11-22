#include "vklearnin/vklearnin.hpp"
#include "vklearnin/shaders/DescriptorPool.hpp"

#include "vklearnin/rendering/devices/LogicalDevice.hpp"

namespace vkl {

//==============================================================================
void DescriptorPool::create(const PoolSizes &sizes) {
    vk::DescriptorPoolCreateInfo pool_info {
        .maxSets = 10u,
        .poolSizeCount = static_cast<uint32_t>(sizes.size()),
        .pPoolSizes = sizes.data()
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
DescriptorPool::DescriptorPool(DescriptorPool &&other) :
    _pool { std::move(other._pool) }
{ }

} // namespace vkl
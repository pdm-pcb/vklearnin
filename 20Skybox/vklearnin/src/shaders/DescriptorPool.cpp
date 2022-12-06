#include "vklearnin/vklearnin.hpp"
#include "vklearnin/shaders/DescriptorPool.hpp"

#include "vklearnin/rendering/devices/LogicalDevice.hpp"

namespace vkl {

// =============================================================================
void DescriptorPool::create(const PoolSizes &sizes) {
    std::vector<vk::DescriptorPoolSize> size_list {
        sizes.begin(),
        sizes.end()
    };

    CONSOLE_TRACE("Creating descriptor pool for {} types", size_list.size());
    vk::DescriptorPoolCreateInfo pool_info {
        .maxSets = 10u,
        .poolSizeCount = static_cast<uint32_t>(size_list.size()),
        .pPoolSizes = size_list.data()
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

// =============================================================================
void DescriptorPool::destroy() {
    LogicalDevice::native().destroyDescriptorPool(_pool);
}

// =============================================================================
DescriptorPool::DescriptorPool(DescriptorPool &&other) :
    _pool { std::move(other._pool) }
{ }

} // namespace vkl
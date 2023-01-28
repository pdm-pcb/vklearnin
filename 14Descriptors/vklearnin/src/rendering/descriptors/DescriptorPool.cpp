#include "vklearnin/vklearnin.hpp"
#include "vklearnin/rendering/descriptors/DescriptorPool.hpp"

#include "vklearnin/system/devices/LogicalDevice.hpp"

namespace vkl {

// =============================================================================
void DescriptorPool::create(const PoolSizes &sizes) {
    std::vector<vk::DescriptorPoolSize> size_list {
        sizes.begin(),
        sizes.end()
    };

    CONSOLE_TRACE("Creating descriptor pool for {} types", size_list.size());
    const vk::DescriptorPoolCreateInfo pool_info {
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
        return;
    }

    CONSOLE_TRACE(
        "Created descriptor pool {:#x}",
        reinterpret_cast<uint64_t>(VkDescriptorPool(_pool))
    );
}

// =============================================================================
void DescriptorPool::destroy() {
    CONSOLE_TRACE(
        "Destroying descriptor pool {:#x}",
        reinterpret_cast<uint64_t>(VkDescriptorPool(_pool))
    );
    LogicalDevice::native().destroyDescriptorPool(_pool);
}


// =============================================================================
DescriptorPool::DescriptorPool() :
    _pool { }
{ }

} // namespace vkl
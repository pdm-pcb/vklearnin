#include "vklearnin/vklearnin.hpp"
#include "vklearnin/rendering/descriptors/DescriptorPool.hpp"

#include "vklearnin/system/devices/LogicalDevice.hpp"

namespace vkl {

// =============================================================================
void DescriptorPool::create(uint32_t const max_sets, const PoolSizes &sizes) {
    std::vector<vk::DescriptorPoolSize> size_list {
        sizes.begin(),
        sizes.end()
    };

    _pool = LogicalDevice::native().createDescriptorPool(
        vk::DescriptorPoolCreateInfo { }
            .setMaxSets(max_sets)
            .setPoolSizes(size_list)
    );

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
    _pool = nullptr;
}


// =============================================================================
DescriptorPool::DescriptorPool() :
    _pool { }
{ }

} // namespace vkl
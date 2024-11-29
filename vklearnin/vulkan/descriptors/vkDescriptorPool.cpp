#include "vklearnin/vklearnin.hpp"
#include "vklearnin/vulkan/descriptors/vkDescriptorPool.hpp"

#include "vklearnin/vulkan/devices/vkDevice.hpp"

namespace vkl {

// =============================================================================
vkDescriptorPool &
vkDescriptorPool::set_type_count(vk::DescriptorType const type,
                                 uint32_t const count)
{
    _type_counts[type] = count;
    return *this;
}

// =============================================================================
bool vkDescriptorPool::create(uint32_t const max_sets, vkDevice const &device) {
    if(_handle) {
        Log::critical("Descriptor pool {} already exists", _handle);
        return false;
    }

    if(!device.native()) {
        Log::error("Cannot create descriptor pool with invalid device.");
        return false;
    }

    _device = device.native();

    std::vector<vk::DescriptorPoolSize> pool_sizes;
    pool_sizes.reserve(_type_counts.size());

    for(auto const [type, count] : _type_counts) {
        pool_sizes.emplace_back(vk::DescriptorPoolSize {
            .type = type,
            .descriptorCount = count
        });
    }

    vk::DescriptorPoolCreateInfo create_info {
        .flags = { },
        .maxSets = max_sets,
        .poolSizeCount = static_cast<uint32_t>(pool_sizes.size()),
        .pPoolSizes = pool_sizes.data(),
    };

    auto const [ result, value ] = _device.createDescriptorPool(create_info);

    if(result != vk::Result::eSuccess) {
        Log::error(
            "Unable to create descriptor pool: '{}'",
            vk::to_string(result)
        );
        return false;
    }

    _handle = value;
    Log::trace("Created descriptor pool {}.", _handle);
    return true;
}

// =============================================================================
bool vkDescriptorPool::destroy() {
    if(!_handle) {
        Log::error("Must create descriptor pool before calling "
                          "destroy.");
        return false;
    }

    Log::trace("Destroying descriptor pool {}", _handle);
    _device.destroy(_handle);

    _handle = nullptr;
    _device = nullptr;
    _type_counts.clear();

    return true;
}

} // namespace vkl
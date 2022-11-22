#include "vklearnin/vklearnin.hpp"
#include "vklearnin/engine/FrameData.hpp"

#include "vklearnin/rendering/devices/LogicalDevice.hpp"
#include "vklearnin/shaders/InstanceData.hpp"

namespace vkl {

// =============================================================================
void FrameData::update_instance_data(const InstanceData &data) {
    const auto &buffer = _descriptor_set.instance_buffer();

    void *mapped = VKAllocator::map_buffer(buffer.allocation);
        memcpy(mapped, &data, buffer.allocation.size);
    VKAllocator::unmap_buffer(buffer.allocation);
}

// =============================================================================
void FrameData::create(const Bindings &bindings) {
    _cmd_pool.create();
    _cmd_buffer.create(_cmd_pool);

    PoolSizes pool_sizes;
    pool_sizes.reserve(bindings.size());

    for(const auto &binding : bindings) {
        pool_sizes.push_back({
            .type = binding.descriptorType,
            .descriptorCount = 10u
        });
    }

    _descriptor_pool.create(pool_sizes);
    _descriptor_set_layout.create(bindings);
    _descriptor_set.create(_descriptor_pool,
                           _descriptor_set_layout,
                           sizeof(InstanceData));
}

// =============================================================================
void FrameData::destroy() {
    _cmd_buffer.destroy();
    _cmd_pool.destroy();

    _descriptor_set_layout.destroy();
    _descriptor_pool.destroy();
    _descriptor_set.destroy();
}

// =============================================================================
FrameData::FrameData(FrameData &&other) :
    _cmd_pool   { std::move(other._cmd_pool) },
    _cmd_buffer { std::move(other._cmd_buffer) },
    _descriptor_pool       { std::move(other._descriptor_pool) },
    _descriptor_set_layout { std::move(other._descriptor_set_layout) },
    _descriptor_set        { std::move(other._descriptor_set) }
{ }

} // namespace vkl
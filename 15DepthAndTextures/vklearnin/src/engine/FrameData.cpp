#include "vklearnin/vklearnin.hpp"
#include "vklearnin/engine/FrameData.hpp"

#include "vklearnin/rendering/devices/LogicalDevice.hpp"
#include "vklearnin/shaders/InstanceData.hpp"

namespace vkl {

// =============================================================================
void FrameData::update_instance_data(const InstanceData &data) {
    auto result = LogicalDevice::native().mapMemory(
        _descriptor_set.buffer().memory,
        0u,
        _descriptor_set.buffer().size
    );
    if(result.result != vk::Result::eSuccess) {
        CONSOLE_CRITICAL("Unable to map device memory");
    }

    void *map = result.value;
        memcpy(map, &data, _descriptor_set.buffer().size);
    LogicalDevice::native().unmapMemory(_descriptor_set.buffer().memory);
}

// =============================================================================
void FrameData::create() {
    _cmd_pool.create();
    _cmd_buffer.create(_cmd_pool);

    _descriptor_pool.create({
        .type = vk::DescriptorType::eUniformBuffer,
        .descriptorCount = 10u
    });

    _descriptor_set_layout.create({
    {
        .binding = 0u,
        .descriptorType = vk::DescriptorType::eUniformBuffer,
        .descriptorCount = 1u,
        .stageFlags = vk::ShaderStageFlagBits::eVertex,
        .pImmutableSamplers = nullptr
    }});

    _descriptor_set.create(_descriptor_pool, _descriptor_set_layout,
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
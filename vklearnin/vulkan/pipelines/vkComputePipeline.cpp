#include "vklearnin/vklearnin.hpp"
#include "vklearnin/vulkan/pipelines/vkComputePipeline.hpp"

#include "vklearnin/vulkan/devices/vkDevice.hpp"
#include "vklearnin/vulkan/devices/vkCmdBuffer.hpp"

namespace vkl {

// =============================================================================
vkComputePipeline & vkComputePipeline::add_shader(vkShaderModule const &module)
{
    if(!module.native()) {
        Log::error("Cannot add invalid shader to graphics pipeline.");
        return *this;
    }

    if(_shader_stage.module) {
        Log::warn(
            "Replacing compute shader module {} with {}",
            _shader_stage.module,
            module.native()
        );
    }

    _shader_stage = vk::PipelineShaderStageCreateInfo {
        .pNext = nullptr,
        .flags = { },
        .stage = module.stage(),
        .module = module.native(),
        .pName = module.entry_point().data(),
        .pSpecializationInfo = nullptr,
    };

    return *this;
}

// =============================================================================
vkComputePipeline &
vkComputePipeline::add_push_constant(vk::ShaderStageFlags const stage_flags,
                                     vk::DeviceSize const size)
{
    _push_constants.emplace_back(vk::PushConstantRange {
        .stageFlags = stage_flags,
        .offset = static_cast<uint32_t>(_push_constant_offset),
        .size = static_cast<uint32_t>(size)
    });

    _push_constant_offset += size;

    return *this;
}

// =============================================================================
vkComputePipeline &
vkComputePipeline::add_descriptor_set_layout(
    vk::DescriptorSetLayout const &layout)
{
    _descriptor_set_layouts.push_back(layout);
    return *this;
}

// =============================================================================
bool vkComputePipeline::create(vkDevice const &device) {
    if(_handle) {
        Log::critical("Pipeline {} already exists", _handle);
        return false;
    }

    if(!device.native()) {
        Log::error("Cannot create pipeline with invalid device.");
        return false;
    }

    _device = device.native();

    _init_layout();

    _create_info = vk::ComputePipelineCreateInfo {
        .pNext = nullptr,

        // If we're in a debug build, don't optimize the shaders
        #ifdef VKL_DEBUG
            .flags = vk::PipelineCreateFlagBits::eDisableOptimization,
        #endif // VKL_DEBUG

        .stage              = _shader_stage,
        .layout             = _layout,
        .basePipelineHandle = nullptr,
        .basePipelineIndex  = 0u,
    };

    auto const [ result, value ] = _device.createComputePipeline(
        vk::PipelineCache { },
        _create_info
    );

    if(result != vk::Result::eSuccess) {
        Log::critical(
            "Unable to create Vulkan pipeline: '{}'",
             vk::to_string(result)
        );
        return false;
    }

    _handle = value;
    Log::trace("Created Vulkan pipeline {}", _handle);
    return true;
}

// =============================================================================
bool vkComputePipeline::destroy() {
    if(!_handle) {
        Log::error("Must create pipeline before calling destroy.");
        return false;
    }

    _shader_stage = vk::PipelineShaderStageCreateInfo { };

    _create_info = vk::ComputePipelineCreateInfo { };

    Log::trace("Destroying pipeline {}, layout {}", _handle, _layout);
    _device.destroy(_handle);
    _device.destroy(_layout);

    _handle = nullptr;
    _layout = nullptr;
    _device = nullptr;

    return true;
}

// =============================================================================
bool vkComputePipeline::bind(vkCmdBuffer const &cmd_buffer) const {
    if(!_handle) {
        Log::error("Must create pipline before binding.");
        return false;
    }

    if(!cmd_buffer.native()) {
        Log::error("Cannot bind pipeline with invalid command buffer.");
        return false;
    }

    cmd_buffer.native().bindPipeline(
        vk::PipelineBindPoint::eCompute,
        _handle
    );

    return true;
}

// =============================================================================
bool vkComputePipeline::send_push_constants(vk::ShaderStageFlags stage_flags,
                                     uint32_t offset,
                                     uint32_t size_bytes,
                                     void const * data,
                                     vkCmdBuffer const &cmd_buffer) const
{
    if(!_handle) {
        Log::error("Must create pipline before sending push constants.");
        return false;
    }

    if(!cmd_buffer.native()) {
        Log::error("Cannot send push constants with invalid command buffer.");
        return false;
    }

    cmd_buffer.native().pushConstants(
        _layout,
        stage_flags,
        offset,
        size_bytes,
        data
    );

    return true;
}

// =============================================================================
void vkComputePipeline::_init_layout() {
    vk::PipelineLayoutCreateInfo const layout_info {
        .flags = { },
        .setLayoutCount = static_cast<uint32_t>(_descriptor_set_layouts.size()),
        .pSetLayouts = _descriptor_set_layouts.data(),
        .pushConstantRangeCount = static_cast<uint32_t>(_push_constants.size()),
        .pPushConstantRanges = _push_constants.data(),
    };

    _layout = _device.createPipelineLayout(layout_info);
    Log::trace("Created compute pipeline layout {}", _layout);
}

} // namespace vkl

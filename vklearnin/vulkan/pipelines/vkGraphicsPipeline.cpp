#include "vklearnin/vklearnin.hpp"
#include "vklearnin/vulkan/pipelines/vkGraphicsPipeline.hpp"

#include "vklearnin/vulkan/devices/vkDevice.hpp"
#include "vklearnin/vulkan/devices/vkCmdBuffer.hpp"
#include "vklearnin/vulkan/pipelines/vkRenderPass.hpp"

namespace vkl {

// =============================================================================
vkGraphicsPipeline &
vkGraphicsPipeline::describe_vertex_input(VertexBindings const bindings,
                                          VertexAttribs const attributes)
{
    _vert_input_info = vk::PipelineVertexInputStateCreateInfo {
        .pNext = nullptr,
        .flags = { },
        .vertexBindingDescriptionCount =
            static_cast<uint32_t>(bindings.size()),
        .pVertexBindingDescriptions = bindings.data(),
        .vertexAttributeDescriptionCount =
            static_cast<uint32_t>(attributes.size()),
        .pVertexAttributeDescriptions = attributes.data(),
    };

    return *this;
}

// =============================================================================
vkGraphicsPipeline &
vkGraphicsPipeline::add_shader(vkShaderModule const &module)
{
    if(!module.native()) {
        Log::error("Cannot add invalid shader to graphics pipeline.");
        return *this;
    }

    _shader_stages.emplace_back( vk::PipelineShaderStageCreateInfo {
        .pNext = nullptr,
        .flags = { },
        .stage = module.stage(),
        .module = module.native(),
        .pName = module.entry_point().data(),
        .pSpecializationInfo = nullptr,
    });

    if(module.stage() == vk::ShaderStageFlagBits::eVertex) {
        _vert_input_info = module.vert_input_info();
    }

    return *this;
}

// =============================================================================
vkGraphicsPipeline &
vkGraphicsPipeline::add_push_constant(vk::ShaderStageFlags const stage_flags,
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
vkGraphicsPipeline &
vkGraphicsPipeline::add_descriptor_set_layout(vk::DescriptorSetLayout const &layout) {
    _descriptor_set_layouts.push_back(layout);
    return *this;
}

// =============================================================================
vkGraphicsPipeline & vkGraphicsPipeline::add_render_pass(vkRenderPass const &render_pass) {
    if(_render_pass) {
        Log::warn(
            "Replacing render pass {} with {}",
            _render_pass,
            render_pass.native()
        );
    }
    _render_pass = render_pass.native();
    return *this;
}

// =============================================================================
bool vkGraphicsPipeline::create(Config const &config, vkDevice const &device) {
    if(_handle) {
        Log::critical("Pipeline {} already exists", _handle);
        return false;
    }

    if(!device.native()) {
        Log::error("Cannot create pipeline with invalid device.");
        return false;
    }

    _device = device.native();

    _init_input_assembly(config);
    _init_viewport(config);
    _init_raster(config);
    _init_multisample(config);
    _init_depth_stencil(config);
    _init_blend_states();
    _init_dynamic_states();
    _init_layout();

    _create_info = vk::GraphicsPipelineCreateInfo {
        .pNext = config.rendering_create_info,

        // If we're in a debug build, don't optimize the shaders
        #ifdef VKL_DEBUG
            .flags = vk::PipelineCreateFlagBits::eDisableOptimization,
        #endif // VKL_DEBUG

        .stageCount = static_cast<uint32_t>(_shader_stages.size()),
        .pStages    = _shader_stages.data(),

        .pVertexInputState   = &_vert_input_info,
        .pInputAssemblyState = &_assembly_info,
        .pTessellationState  = nullptr,
        .pViewportState      = &_viewport_info,
        .pRasterizationState = &_raster_info,
        .pMultisampleState   = &_multisample_info,
        .pDepthStencilState  = &_depth_stencil_info,
        .pColorBlendState    = &_blend_info,
        .pDynamicState       = &_dynamic_state_info,
        .layout              = _layout,
        .renderPass          = _render_pass,
        .subpass             = 0u,

        // A new pipeline may be derrived from an existing one, only updating
        // what needs to be updated. The .basePipeline* values designate an
        // existing pipeline to pull from.
        .basePipelineHandle  = nullptr,
        .basePipelineIndex   = 0,
    };

    auto const [ result, value ] = _device.createGraphicsPipeline(
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
bool vkGraphicsPipeline::destroy() {
    if(!_handle) {
        Log::error("Must create pipeline before calling destroy.");
        return false;
    }

    _shader_stages.clear();
    _blend_states.clear();
    _dynamic_states.clear();

    _viewport           = vk::Viewport { };
    _scissor            = vk::Rect2D { };
    _vert_input_info    = vk::PipelineVertexInputStateCreateInfo { };
    _assembly_info      = vk::PipelineInputAssemblyStateCreateInfo { };
    _viewport_info      = vk::PipelineViewportStateCreateInfo { };
    _raster_info        = vk::PipelineRasterizationStateCreateInfo { };
    _multisample_info   = vk::PipelineMultisampleStateCreateInfo { };
    _depth_stencil_info = vk::PipelineDepthStencilStateCreateInfo { };
    _blend_info         = vk::PipelineColorBlendStateCreateInfo { };
    _dynamic_state_info = vk::PipelineDynamicStateCreateInfo { };
    _render_pass        = nullptr;
    _create_info        = vk::GraphicsPipelineCreateInfo { };

    Log::trace("Destroying pipeline {}, layout {}", _handle, _layout);
    _device.destroy(_handle);
    _device.destroy(_layout);

    _handle = nullptr;
    _layout = nullptr;
    _device = nullptr;

    return true;
}

// =============================================================================
bool vkGraphicsPipeline::bind(vkCmdBuffer const &cmd_buffer) const {
    if(!_handle) {
        Log::error("Must create pipline before binding.");
        return false;
    }

    if(!cmd_buffer.native()) {
        Log::error("Cannot bind pipeline with invalid command buffer.");
        return false;
    }

    cmd_buffer.native().bindPipeline(
        vk::PipelineBindPoint::eGraphics,
        _handle
    );

    cmd_buffer.native().setViewport(0u, _viewport);
    cmd_buffer.native().setScissor(0u, _scissor);

    return true;
}

// =============================================================================
bool vkGraphicsPipeline::send_push_constants(vk::ShaderStageFlags stage_flags,
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
void vkGraphicsPipeline::update_dimensions(vk::Extent2D const &extent,
                                   vk::Offset2D const &offset)
{
    _viewport = vk::Viewport {
        .x         = static_cast<float>(offset.x),
        .y         = static_cast<float>(extent.height),
        .width     = static_cast<float>(extent.width),
        .height    = -static_cast<float>(extent.height),
        .minDepth  = 0.0f,
        .maxDepth  = 1.0f,
    };

    _scissor = vk::Rect2D {
        .offset = offset,
        .extent = extent,
    };

    Log::trace(
        "Pipeline viewport updated: {:.02f} x {:.02f} ({:.02f}, {:.02f}) ",
        _viewport.width,
        _viewport.height,
        _viewport.x,
        _viewport.y
    );
}

// =============================================================================
void vkGraphicsPipeline::_init_input_assembly(Config const &config) {
    // The primitive assembly stage requires knowing how to interpret the
    // vertices you've asked it to draw. Again, we're not feeding anything
    // into the vertex shader, but we do want the assembly stage to see the
    // fixed vertex data as a triangle.
    _assembly_info = {
        .pNext = nullptr,
        .flags = { },
        // More correctly, the vertices we provided constitute a triangle list
        // with a length of one.
        .topology = config.topology,

        // Restarting the assembly of primitives is not applicable here.
        .primitiveRestartEnable = vk::False
    };
}

// =============================================================================
void vkGraphicsPipeline::_init_viewport(Config const &config) {
    // We need to tell Vulkan how many viewports and scissors we're providing,
    // but since we're going to mark these as dynamic states, the actual
    // pointers should be null.
    _viewport_info = vk::PipelineViewportStateCreateInfo {
        .pNext         = nullptr,
        .flags         = { },
        .viewportCount = 1u,
        .pViewports    = nullptr,
        .scissorCount  = 1u,
        .pScissors     = nullptr,
    };

    update_dimensions(config.viewport_extent, config.viewport_offset);
}

// =============================================================================
void vkGraphicsPipeline::_init_raster(Config const &config) {
    _raster_info = {
        .pNext = nullptr,
        .flags = { },

        // Depth clamping requires enabling a VkPhysicalDevice feature of the
        // same name, and changes how the depth test of a given fragment might
        // go by clamping its Z value to be within the near and far planes of
        // the view frustum before running the test.
        .depthClampEnable = vk::False,

        // There are some situations in which you want a pipeline to complete
        // only the vertex (or geometry, etc) stage on the geometry. In those
        // situations, it's hugely beneficial to discard the primitives before
        // reaching the rasterization stage.
        .rasterizerDiscardEnable = vk::False,

        .polygonMode = config.polygon_mode,
        .cullMode    = config.cull_mode,
        .frontFace   = config.front_face,

        .depthBiasEnable         = config.enable_depth_bias,
        .depthBiasConstantFactor = config.depth_bias_constant,
        .depthBiasClamp          = 0.0f,
        .depthBiasSlopeFactor    = config.depth_bias_slope,

        // If a line segment is to be reasterized, what width should it be?
        .lineWidth = config.line_width,
    };
}

// =============================================================================
void vkGraphicsPipeline::_init_multisample(Config const &config) {
    _multisample_info = {
        .pNext                 = nullptr,
        .flags                 = { },
        .rasterizationSamples  = config.sample_flags,
        .sampleShadingEnable   = vk::False,
        .minSampleShading      = 0.0f,
        .pSampleMask           = nullptr,
        .alphaToCoverageEnable = vk::False,
        .alphaToOneEnable      = vk::False,
    };
}

// =============================================================================
void vkGraphicsPipeline::_init_depth_stencil(Config const &config) {
    _depth_stencil_info = {
        .pNext                 = nullptr,
        .flags                 = { },
        .depthTestEnable       = config.enable_depth_test,
        .depthWriteEnable      = vk::True,
        .depthCompareOp        = config.depth_compare,
        .depthBoundsTestEnable = vk::False,
        .stencilTestEnable     = vk::False,
        .front                 = { },
        .back                  = { },
        .minDepthBounds        = 0.0f,
        .maxDepthBounds        = 1.0f
    };
}

// =============================================================================
void vkGraphicsPipeline::_init_blend_states() {
    _blend_states.emplace_back(vk::PipelineColorBlendAttachmentState {
        // Even though blending is disabled, the pipeline still runs this stage
        .blendEnable = vk::True,

        .srcColorBlendFactor = vk::BlendFactor::eSrcAlpha,
        .dstColorBlendFactor = vk::BlendFactor::eOneMinusSrcAlpha,
        .colorBlendOp        = vk::BlendOp::eAdd,
        .srcAlphaBlendFactor = vk::BlendFactor::eOneMinusSrcAlpha,
        .dstAlphaBlendFactor = vk::BlendFactor::eZero,
        .alphaBlendOp        = vk::BlendOp::eAdd,

        // ...and the blend stage needs to know the color channels to which
        // it's allowed to write
        .colorWriteMask = vk::ColorComponentFlagBits::eR |
                          vk::ColorComponentFlagBits::eG |
                          vk::ColorComponentFlagBits::eB |
                          vk::ColorComponentFlagBits::eA,
    });

    _blend_info = {
        .pNext = nullptr,
        .flags = { },
        .logicOpEnable = vk::False,
        .logicOp = vk::LogicOp::eCopy,

        // The blend stage also needs to know what images it's blending. Since
        // our render pass writes to its color attachemnt, we need to tell
        // the blend stage to look there.
        .attachmentCount = static_cast<uint32_t>(_blend_states.size()),
        .pAttachments    = _blend_states.data(),

        .blendConstants = { },
    };
}

// =============================================================================
void vkGraphicsPipeline::_init_dynamic_states() {
    // Setting the viewport and scissor states to dynamic allows us to change
    // the size of the target surface without recreating the entire pipeline.
    _dynamic_states = {
        vk::DynamicState::eViewport,
        vk::DynamicState::eScissor,
    };

    _dynamic_state_info = {
        .dynamicStateCount = static_cast<uint32_t>(_dynamic_states.size()),
        .pDynamicStates    = _dynamic_states.data(),
    };
}

// =============================================================================
void vkGraphicsPipeline::_init_layout() {
    vk::PipelineLayoutCreateInfo const layout_info {
        .setLayoutCount = static_cast<uint32_t>(_descriptor_set_layouts.size()),
        .pSetLayouts    = _descriptor_set_layouts.data(),
        .pushConstantRangeCount = static_cast<uint32_t>(_push_constants.size()),
        .pPushConstantRanges    = _push_constants.data(),
    };

    _layout = _device.createPipelineLayout(layout_info);
    Log::trace("Created graphics pipeline layout {}", _layout);
}

} // namespace vkl

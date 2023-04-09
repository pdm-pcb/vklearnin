#include "vklearnin/vklearnin.hpp"
#include "vklearnin/rendering/pipeline/Pipeline.hpp"

#include "vklearnin/rendering/swapchain/Swapchain.hpp"
#include "vklearnin/system/devices/LogicalDevice.hpp"
#include "vklearnin/rendering/descriptors/DescriptorSetLayout.hpp"
#include "vklearnin/system/devices/CmdBuffer.hpp"

namespace vkl {

// =============================================================================
void Pipeline::bind(CmdBuffer const &cmd_buffer) {
    cmd_buffer.native().bindPipeline(
        vk::PipelineBindPoint::eGraphics,
        _pipeline
    );
    cmd_buffer.native().setViewport(0u, _viewport);
    cmd_buffer.native().setScissor(0u, _scissor);
}

// =============================================================================
void Pipeline::bind_descriptor_set(CmdBuffer const &cmd_buffer,
                                   DescriptorSet const &set)
{
    auto const set_key = reinterpret_cast<uint64_t>(
        VkDescriptorSetLayout(set.layout())
    );

    cmd_buffer.native().bindDescriptorSets(
        vk::PipelineBindPoint::eGraphics,
        _layout,
        _desc_set_bindings[set_key],
        1u, &(set.native()),
        0u, nullptr
    );
}

// =============================================================================
Pipeline & Pipeline::vert_from_spirv(std::string_view filepath,
                                     std::string_view entry_point)
{
    if(_pipeline) {
        CONSOLE_CRITICAL("Adding a vertex stage to a pipeline that's already "
                         "been created.");
    }

    _vert.create(filepath);
    _shader_stages.emplace_back(vk::PipelineShaderStageCreateInfo {
        .stage = vk::ShaderStageFlagBits::eVertex,
        .module = _vert.native(),
        .pName = entry_point.data(),
    });

    return *this;
}

// =============================================================================
Pipeline & Pipeline::frag_from_spirv(std::string_view filepath,
                                     std::string_view entry_point)
{
    if(_pipeline) {
        CONSOLE_CRITICAL("Adding a fragment stage to a pipeline that's already "
                         "been created.");
    }

    _frag.create(filepath);
    _shader_stages.emplace_back(vk::PipelineShaderStageCreateInfo {
        .stage = vk::ShaderStageFlagBits::eFragment,
        .module = _frag.native(),
        .pName = entry_point.data(),
    });

    return *this;
}

// =============================================================================
Pipeline & Pipeline::describe_vertex_input(Vertex::Bindings const &bindings,
                                           Vertex::Attribs const &attributes)
{
    auto const binding_count = static_cast<uint32_t>(bindings.size());
    auto const attrib_count  = static_cast<uint32_t>(attributes.size());

    _vert_input_info = {
        .vertexBindingDescriptionCount = binding_count,
        .pVertexBindingDescriptions    = bindings.data(),

        .vertexAttributeDescriptionCount = attrib_count,
        .pVertexAttributeDescriptions   = attributes.data(),
    };

    return *this;
}

// =============================================================================
Pipeline & Pipeline::add_descriptor_set(DescriptorSetLayout const &set_layout) {
    if(_pipeline) {
        CONSOLE_CRITICAL("Adding a descriptor set to a pipeline that's already "
                         "been created.");
    }

    _desc_set_layouts.push_back(set_layout.native());
    return *this;
}

// =============================================================================
Pipeline & Pipeline::add_push_constant(vk::ShaderStageFlags const stage_flags,
                                       size_t const size)
{
    if(_pipeline) {
        CONSOLE_CRITICAL("Adding a push constant to a pipeline that's already "
                         "been created.");
    }

    _push_constants.push_back({
        .stageFlags = stage_flags,
        .offset = static_cast<uint32_t>(_push_constant_offset),
        .size = static_cast<uint32_t>(size)
    });

    _push_constant_offset += size;

    return *this;
}

// =============================================================================
void Pipeline::create(Config const &config) {
    if(_pipeline) {
        CONSOLE_CRITICAL("Attempting to recreate a pipeline.");
        return;
    }

    _init_assembly();
    _init_viewport(config);
    _init_raster(config);
    _init_multisample(config);
    _init_depth_stencil(config);
    _init_blend();
    _init_dynamic_states();
    _init_layout();

    vk::PipelineRenderingCreateInfo const rendering_info {
        .colorAttachmentCount = static_cast<uint32_t>(
            config.color_formats.size()
        ),
        .pColorAttachmentFormats = config.color_formats.data(),
        .depthAttachmentFormat = config.depth_format,
    };

    vk::GraphicsPipelineCreateInfo const pipeline_info {
        .pNext = &rendering_info,

        // If we're in a debug build, don't optimize out unused shader data
        // and such
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

        // A new pipeline may be derrived from an existing one, only updating
        // what needs to be updated. The .basePipeline* values designate an
        // existing pipeline to pull from.
        .basePipelineHandle  = nullptr,
        .basePipelineIndex   = 0,
    };

    auto pipeline_return =
        LogicalDevice::native().createGraphicsPipeline({ }, pipeline_info);

    if(pipeline_return.result != vk::Result::eSuccess) {
        CONSOLE_CRITICAL(
            "Unable to create pipelines: '{}'",
            to_string(pipeline_return.result)
        );
        return;
    }

    _pipeline = pipeline_return.value;
    CONSOLE_TRACE(
        "Created pipeline {:#x}",
        reinterpret_cast<uint64_t>(VkPipeline(_pipeline))
    );

}

// =============================================================================
void Pipeline::destroy() {
    _vert.destroy();
    _frag.destroy();

    CONSOLE_TRACE(
        "Destroying pipeline {:#x}, layout {:#x}",
        reinterpret_cast<uint64_t>(VkPipeline(_pipeline)),
        reinterpret_cast<uint64_t>(VkPipelineLayout(_layout))
    );

    LogicalDevice::native().destroy(_layout);
    LogicalDevice::native().destroy(_pipeline);
}

// =============================================================================
void Pipeline::update_dimensions(vk::Extent2D const &extent,
                                 vk::Offset2D const &offset)
{
    _viewport = vk::Viewport {
        .x         = static_cast<float>(offset.x),
        .y         = static_cast<float>(offset.y),
        .width     = static_cast<float>(extent.width),
        .height    = static_cast<float>(extent.height),
        .minDepth  = 0.0f,
        .maxDepth  = 1.0f,
    };

    _scissor = vk::Rect2D {
        .offset = { offset.x, offset.y },
        .extent = { extent.width, extent.height },
    };

    CONSOLE_TRACE(
        "Pipeline viewport updated: {:.02f}x{:.02f} ({:.02f}, {:.02f}) ",
        _viewport.width,
        _viewport.height,
        _viewport.x,
        _viewport.y
    );
}

// =============================================================================
void Pipeline::_init_assembly() {
    // The primitive assembly stage requires knowing how to interpret the
    // vertices you've asked it to draw. Again, we're not feeding anything
    // into the vertex shader, but we do want the assembly stage to see the
    // fixed vertex data as a triangle.
    _assembly_info = {
        // More correctly, the vertices we provided constitute a triangle list
        // with a length of one.
        .topology = vk::PrimitiveTopology::eTriangleList,

        // Restarting the assembly of primitives is not applicable here.
        .primitiveRestartEnable = VK_FALSE
    };
}

// =============================================================================
void Pipeline::_init_viewport(Config const &config) {
    // We need to tell Vulkan how many viewports and scissors we're providing,
    // but since we're going to mark these as dynamic states, the actual
    // pointers should be null.
    _viewport_info = vk::PipelineViewportStateCreateInfo {
        .viewportCount = 1u,
        .pViewports    = nullptr,
        .scissorCount  = 1u,
        .pScissors     = nullptr,
    };

    update_dimensions(config.viewport_extent, config.viewport_offset);
}

// =============================================================================
void Pipeline::_init_raster(Config const &config) {
    _raster_info = {
        // Depth clamping requires enabling a VkPhysicalDevice feature of the
        // same name, and changes how the depth test of a given fragment might
        // go by clamping its Z value to be within the near and far planes of
        // the view frustum before running the test.
        .depthClampEnable = VK_FALSE,

        // There are some situations in which you want a pipeline to complete
        // only the vertex (or geometry, etc) stage on the geometry. In those
        // situations, it's hugely beneficial to discard the primitives before
        // reaching the rasterization stage.
        .rasterizerDiscardEnable = VK_FALSE,

        .polygonMode = config.polygon_mode,
        .cullMode    = config.cull_mode,
        .frontFace   = config.front_face,

        .depthBiasEnable         = config.enable_depth_bias,
        .depthBiasConstantFactor = config.depth_bias_constant,
        .depthBiasClamp          = 0.0f,
        .depthBiasSlopeFactor    = config.depth_bias_slope,

        // If a line segment is to be reasterized, what width should it be?
        .lineWidth = 1.0f,
    };
}

// =============================================================================
void Pipeline::_init_multisample(Config const &config) {
    _multisample_info = {
        .rasterizationSamples  = config.sample_flags,
        .sampleShadingEnable   = VK_FALSE,
        .minSampleShading      = 0.0f,
        .pSampleMask           = nullptr,
        .alphaToCoverageEnable = VK_FALSE,
        .alphaToOneEnable      = VK_FALSE,
    };
}

// =============================================================================
void Pipeline::_init_depth_stencil(Config const &config) {
    _depth_stencil_info = {
        .depthTestEnable       = config.enable_depth_test,
        .depthWriteEnable      = VK_TRUE,
        .depthCompareOp        = config.depth_compare,
        .depthBoundsTestEnable = VK_FALSE,
        .stencilTestEnable     = VK_FALSE,
        .front                 = { },
        .back                  = { },
        .minDepthBounds        = 0.0f,
        .maxDepthBounds        = 1.0f
    };
}

// =============================================================================
void Pipeline::_init_blend() {
    // Even though blending is disabled, the pipeline still runs a blend stage
    _blend_states = {{
        .blendEnable = VK_FALSE,

        // ...and the blend stage needs to know the color channels to which
        // it's allowed to write
        .colorWriteMask = vk::ColorComponentFlagBits::eR |
                          vk::ColorComponentFlagBits::eG |
                          vk::ColorComponentFlagBits::eB |
                          vk::ColorComponentFlagBits::eA
    }};

    _blend_info = {
        // The blend stage also needs to know what images it's blending. Since
        // our render pass writes to its color attachemnt, we need to tell
        // the blend stage to look there.
        .attachmentCount = static_cast<uint32_t>(_blend_states.size()),
        .pAttachments    = _blend_states.data()
    };
}

// =============================================================================
void Pipeline::_init_dynamic_states() {
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
void Pipeline::_init_layout() {
    for(auto const &layout : _desc_set_layouts) {
        auto const layout_key = reinterpret_cast<uint64_t>(
            VkDescriptorSetLayout(layout)
        );

        _desc_set_bindings[layout_key] = _next_set_binding;

        ++_next_set_binding;
    }

    const vk::PipelineLayoutCreateInfo layout_info {
        .setLayoutCount = static_cast<uint32_t>(_desc_set_layouts.size()),
        .pSetLayouts    = _desc_set_layouts.data(),
        .pushConstantRangeCount = static_cast<uint32_t>(_push_constants.size()),
        .pPushConstantRanges    = _push_constants.data()
    };

    _layout = LogicalDevice::native().createPipelineLayout(layout_info);

    CONSOLE_TRACE(
        "Created pipeline layout {:#x}",
        reinterpret_cast<uint64_t>(VkPipelineLayout(_layout))
    );
}

// =============================================================================
Pipeline::Pipeline() :
    _viewport             { },
    _scissor              { },
    _vert_input_info      { },
    _assembly_info        { },
    _viewport_info        { },
    _raster_info          { },
    _multisample_info     { },
    _dynamic_state_info   { },
    _desc_set_layouts     { },
    _desc_set_bindings    { },
    _next_set_binding     { 0u },
    _push_constants       { },
    _push_constant_offset { 0 },
    _layout               { },
    _pipeline             { }
{ }

} // namespace vkl
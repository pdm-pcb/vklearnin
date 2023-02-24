#include "vklearnin/vklearnin.hpp"
#include "vklearnin/rendering/pipeline/Pipeline.hpp"

#include "vklearnin/rendering/swapchain/Swapchain.hpp"
#include "vklearnin/system/devices/LogicalDevice.hpp"
#include "vklearnin/rendering/renderpass/RenderPass.hpp"

namespace vkl {

// =============================================================================
void Pipeline::vert_from_spirv(std::string_view filepath) {
    _vert.create(filepath);
    _shader_stages.emplace_back(vk::PipelineShaderStageCreateInfo {
        .stage = vk::ShaderStageFlagBits::eVertex,
        .module = _vert.native(),
        .pName = "main",
    });
}

// =============================================================================
void Pipeline::frag_from_spirv(std::string_view filepath) {
    _frag.create(filepath);
    _shader_stages.emplace_back(vk::PipelineShaderStageCreateInfo {
        .stage = vk::ShaderStageFlagBits::eFragment,
        .module = _frag.native(),
        .pName = "main",
    });
}

// =============================================================================
void Pipeline::describe_vertex_input(const VertexBindings &bindings,
                                     const VertexAttribs &attributes)
{
    auto binding_count = static_cast<uint32_t>(bindings.size());
    auto attrib_count  = static_cast<uint32_t>(attributes.size());

    // The caller of this function will provide the details of the vertex
    // data this pipeline will expect to see in any given vertex buffer.

    _vert_input_info = {
        .vertexBindingDescriptionCount = binding_count,
        .pVertexBindingDescriptions    = bindings.data(),

        .vertexAttributeDescriptionCount = attrib_count,
        .pVertexAttributeDescriptions   = attributes.data(),
    };
}

// =============================================================================
void Pipeline::add_descriptor_set(const vk::DescriptorSetLayout &set_layout) {
    _desc_set_layouts.push_back(set_layout);
}

// =============================================================================
void Pipeline::add_push_constant(vk::ShaderStageFlags stage_flags,
                                 size_t size)
{
    _push_constants.push_back({
        .stageFlags = stage_flags,
        .offset = static_cast<uint32_t>(_push_constant_offset),
        .size = static_cast<uint32_t>(size)
    });

    _push_constant_offset += size;
}

// =============================================================================
void Pipeline::create(const RenderPass &render_pass) {
    _init_assembly();
    _init_viewport();
    _init_raster();
    _init_multisample();
    _init_depth_stencil();
    _init_blend();
    _init_dynamic_states();
    _init_layout();

    const vk::GraphicsPipelineCreateInfo pipeline_info {
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

        // Which render pass will use this pipeline?
        .renderPass          = render_pass.native(),
        // And within that render pass, which subpass will use this pipeline?
        .subpass             = 0u,

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
            "Unable to create graphics pipelines: '{}'",
            to_string(pipeline_return.result)
        );
        return;
    }

    _pipeline = pipeline_return.value;
    CONSOLE_TRACE(
        "Created graphics pipeline {:#x}",
        reinterpret_cast<uint64_t>(VkPipeline(_pipeline))
    );

}

// =============================================================================
void Pipeline::destroy() {
    _vert.destroy();
    _frag.destroy();

    CONSOLE_TRACE(
        "Destroying graphics pipeline {:#x}, layout {:#x}",
        reinterpret_cast<uint64_t>(VkPipeline(_pipeline)),
        reinterpret_cast<uint64_t>(VkPipelineLayout(_layout))
    );

    LogicalDevice::native().destroy(_layout);
    LogicalDevice::native().destroy(_pipeline);
}

// =============================================================================
void Pipeline::update_dimensions() {
    auto [width, height] = Swapchain::extent();
    auto [x, y]          = Swapchain::offset();

    _viewport = vk::Viewport {
        .x         = static_cast<float>(x),
        .y         = static_cast<float>(height),
        .width     = static_cast<float>(width),
        .height    = -static_cast<float>(height),
        .minDepth  = 0.0f,
        .maxDepth  = 1.0f,
    };

    _scissor = vk::Rect2D {
        .offset = { x, y },
        .extent = { width, height },
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
void Pipeline::_init_viewport() {
    // We need to tell Vulkan how many viewports and scissors we're providing,
    // but since we're going to mark these as dynamic states, the actual
    // pointers should be null.
    _viewport_info = vk::PipelineViewportStateCreateInfo {
        .viewportCount = 1u,
        .pViewports    = nullptr,
        .scissorCount  = 1u,
        .pScissors     = nullptr,
    };

    update_dimensions();
}

// =============================================================================
void Pipeline::_init_raster() {
    _raster_info = {
        // No depth testing is happening yet, so there's nothing to base any
        // clamping on.
        .depthClampEnable = VK_FALSE,

        // There are some situations in which you want a pipeline to complete
        // only the vertex (or geometry, etc) stage on the geometry. In those
        // situations, it's hugely beneficial to discard the primitives before
        // reaching the rasterization stage.
        .rasterizerDiscardEnable = VK_FALSE,

        // The rasterizer can take the points of a polygon and fill them in,
        // only draw their outline, or even just draw the points in space.
        .polygonMode = vk::PolygonMode::eFill,

        // Once the vertex shader has placed a triangle in space, either it's
        // very likely at some oblique angle to the screen. In our case, the
        // fixed vertex data forms a triangle that is perfectly flat on screen,
        // but either way one of the sides of the triangle is not visible. To
        // save on running the fragment shader for every fragment on the side
        // of the triangle facing away from us, we cull the back-facing data.
        .cullMode = vk::CullModeFlagBits::eNone,

        // Which order are the now-processed vertices connected in? Who's on
        // first? This is also called triangle winding, and for us the order
        // is clockwise.
        .frontFace = vk::FrontFace::eClockwise,

        // Once more, there is no depth testing being done, so these values are
        // superfluous.
        .depthBiasEnable         = VK_FALSE,
        .depthBiasConstantFactor = 0.0f,
        .depthBiasClamp          = 0.0f,
        .depthBiasSlopeFactor    = 0.0f,

        // If a line segment is to be reasterized, what width should it be?
        .lineWidth = 1.0f,
    };
}

// =============================================================================
void Pipeline::_init_multisample() {
    switch(RenderConfig::msaa_samples) {
        case 64u: _samples = vk::SampleCountFlagBits::e64; break;
        case 32u: _samples = vk::SampleCountFlagBits::e32; break;
        case 16u: _samples = vk::SampleCountFlagBits::e16; break;
        case 8u:  _samples = vk::SampleCountFlagBits::e8;  break;
        case 4u:  _samples = vk::SampleCountFlagBits::e4;  break;
        case 2u:  _samples = vk::SampleCountFlagBits::e2;  break;
        case 1u:  _samples = vk::SampleCountFlagBits::e1;  break;
        default:
            CONSOLE_CRITICAL(
                "Unsupported color buffer sample count {}.",
                RenderConfig::msaa_samples
            );
    }

    _multisample_info = {
        .rasterizationSamples  = _samples,
        .sampleShadingEnable   = VK_FALSE,
        .minSampleShading      = 0.0f,
        .pSampleMask           = nullptr,
        .alphaToCoverageEnable = VK_FALSE,
        .alphaToOneEnable      = VK_FALSE,
    };
}

// =============================================================================
void Pipeline::_init_depth_stencil() {
    _depth_stencil_info = {
        .depthTestEnable = VK_TRUE,
        .depthWriteEnable = VK_TRUE,
        .depthCompareOp = vk::CompareOp::eLess,
        .depthBoundsTestEnable = VK_FALSE,
        .stencilTestEnable = VK_FALSE,
        .front = { },
        .back = { },
        .minDepthBounds = 0.0f,
        .maxDepthBounds = 1.0f
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
    // Much like the input state above, the layout of this pipeline is empty.
    const vk::PipelineLayoutCreateInfo layout_info {
        .setLayoutCount = static_cast<uint32_t>(_desc_set_layouts.size()),
        .pSetLayouts    = _desc_set_layouts.data(),
        .pushConstantRangeCount = static_cast<uint32_t>(_push_constants.size()),
        .pPushConstantRanges    = _push_constants.data()
    };

    // Unlike the input state above, we do have to explicitly create the
    // pipeline layout object.
    auto result = LogicalDevice::native().createPipelineLayout(layout_info);
    if(result.result != vk::Result::eSuccess) {
        CONSOLE_CRITICAL(
            "Could not create pipeline layout: '{}'",
            to_string(result.result)
        );
        return;
    }

    CONSOLE_TRACE(
        "Created pipeline layout {:#x}",
        reinterpret_cast<uint64_t>(VkPipelineLayout(result.value))
    );

    _layout = result.value;
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
    _samples              { },
    _desc_set_layouts     { },
    _push_constants       { },
    _push_constant_offset { 0 },
    _layout               { },
    _pipeline             { }
{ }

} // namespace vkl
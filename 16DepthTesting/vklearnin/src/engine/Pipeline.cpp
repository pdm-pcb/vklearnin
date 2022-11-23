#include "vklearnin/vklearnin.hpp"
#include "vklearnin/engine/Pipeline.hpp"

#include "vklearnin/tools/ShaderTools.hpp"
#include "vklearnin/rendering/devices/LogicalDevice.hpp"
#include "vklearnin/engine/FrameData.hpp"

////////////////////////////////////////////////////////////////////////////////
// TODO: I feel like these really shouldn't be here, but...?
#include "vklearnin/mesh/Vertex.hpp"
////////////////////////////////////////////////////////////////////////////////

namespace vkl {

// =============================================================================
void Pipeline::vertex_from_binary(const char *filepath) {        
    _vert = ShaderTools::module_from_binary(filepath, LogicalDevice::native());
    _shader_stages.emplace_back(vk::PipelineShaderStageCreateInfo {
        .stage = vk::ShaderStageFlagBits::eVertex,
        .module = _vert,
        .pName = "main",
    });
}

// =============================================================================
void Pipeline::fragment_from_binary(const char *filepath) {        
    _frag = ShaderTools::module_from_binary(filepath, LogicalDevice::native());
    _shader_stages.emplace_back(vk::PipelineShaderStageCreateInfo {
        .stage = vk::ShaderStageFlagBits::eFragment,
        .module = _frag,
        .pName = "main",
    });
}

// =============================================================================
void Pipeline::set_push_constants(const PushConstantRanges &ranges) {
    _push_constant_ranges.clear();
    _push_constant_ranges = ranges;
}

// =============================================================================
void Pipeline::set_layout(const DescriptorSetLayouts &descriptor_layouts) {
    vk::PipelineLayoutCreateInfo pipeline_layout_info {
        .setLayoutCount = static_cast<uint32_t>(descriptor_layouts.size()),
        .pSetLayouts = descriptor_layouts.data(),
        .pushConstantRangeCount =
            static_cast<uint32_t>(_push_constant_ranges.size()),
        .pPushConstantRanges = _push_constant_ranges.data()
    };

    auto pipeline_result =
        LogicalDevice::native().createPipelineLayout(pipeline_layout_info);
    if(pipeline_result.result != vk::Result::eSuccess) {
        CONSOLE_CRITICAL("Could not create pipeline layout");
    }
    _layout = pipeline_result.value;

    CONSOLE_TRACE("Setting a pipeline layout with {} descriptor sets",
                  pipeline_layout_info.setLayoutCount);
}

// =============================================================================
void Pipeline::create() {
    _init_vert_input();
    _init_assembly();
    _init_viewport();
    _init_raster();
    _init_blend();
    _init_depth();
    _init_dynamic_states();

    _render_pass.create(_swapchain);

    vk::GraphicsPipelineCreateInfo pipeline_info {
        .stageCount = static_cast<uint32_t>(_shader_stages.size()),
        .pStages    = _shader_stages.data(),
        .pVertexInputState   = &_vert_input_info,
        .pInputAssemblyState = &_assembly_info,
        .pTessellationState  = nullptr,
        .pViewportState      = &_viewport_info,
        .pRasterizationState = &_raster_info,
        .pMultisampleState   = { },
        .pDepthStencilState  = &_depth_stencil_info,
        .pColorBlendState    = &_blend_info,
        .pDynamicState       = &_dynamic_state_info,
        .layout              = _layout,
        .renderPass          = _render_pass.native(),
        .subpass             = 0u,
        .basePipelineHandle  = nullptr,
        .basePipelineIndex   = -1,
    };

    auto pipeline_return =
        LogicalDevice::native().createGraphicsPipeline({ }, pipeline_info);

    if(pipeline_return.result != vk::Result::eSuccess) {
        CONSOLE_CRITICAL("Unable to create graphics pipelines.");
    }

    _pipeline = pipeline_return.value;

    CONSOLE_TRACE(
        "Created pipeline {:#x}",
        reinterpret_cast<uint64_t>(VkPipeline(_pipeline))
    );
}

// =============================================================================
void Pipeline::destroy() {
    CONSOLE_TRACE(
        "Destroying pipeline {:#x}",
        reinterpret_cast<uint64_t>(VkPipeline(_pipeline))
    );

    _render_pass.destroy();

    LogicalDevice::native().destroy(_vert);
    LogicalDevice::native().destroy(_frag);
    LogicalDevice::native().destroy(_layout);
    LogicalDevice::native().destroy(_pipeline);
}

// =============================================================================
void Pipeline::create_framebuffers() {
    _render_pass.create_framebuffers(_swapchain);
}

// =============================================================================
void Pipeline::destroy_framebuffers() {
    _render_pass.destroy_framebuffers();
}

// =============================================================================
void Pipeline::update_dimensions() {
    const auto [width, height] = _swapchain.extent();
    const auto [x, y]          = _swapchain.offset();

    _viewport = vk::Viewport {
        .x = static_cast<float>(x),
        .y = static_cast<float>(height),
        .width  = static_cast<float>(width),
        .height = -static_cast<float>(height),
        .minDepth = 0.0f,
        .maxDepth = 1.0f,
    };

    _scissor = vk::Rect2D {
        .offset = { x, y },
        .extent = { width, height },
    };

    CONSOLE_TRACE(
        "Pipeline viewport updated: {:.0f} by {:.0f} at ({:.0f}, {:.0f}) ",
        _viewport.width,
        _viewport.height,
        _viewport.x,
        _viewport.y
    );
}

// =============================================================================
void Pipeline::_init_vert_input() {
    _vert_input_info = vk::PipelineVertexInputStateCreateInfo {
        .vertexBindingDescriptionCount =
            static_cast<uint32_t>(Vertex::bindings.size()),
        .pVertexBindingDescriptions = Vertex::bindings.data(),
        .vertexAttributeDescriptionCount =
            static_cast<uint32_t>(Vertex::attribs.size()),
        .pVertexAttributeDescriptions = Vertex::attribs.data()
    };
}

// =============================================================================
void Pipeline::_init_assembly() {
    _assembly_info = {
        .topology = vk::PrimitiveTopology::eTriangleList,
        .primitiveRestartEnable = false
    };
}

// =============================================================================
void Pipeline::_init_viewport() {
    _viewport_info = vk::PipelineViewportStateCreateInfo {
        .viewportCount = 1u,
        .pViewports = nullptr,
        .scissorCount = 1u,
        .pScissors = nullptr,
    };

    CONSOLE_TRACE("Viewport count: {}", _viewport_info.viewportCount);
    CONSOLE_TRACE("Scissor count:  {}", _viewport_info.scissorCount);

    // viewports and scissor rectangles are all sized the same as the surface
    // at present
    update_dimensions();
}

// =============================================================================
void Pipeline::_init_raster() {
    _raster_info = {
        .depthClampEnable = false,
        .rasterizerDiscardEnable = false,
        .polygonMode = vk::PolygonMode::eFill,
        .cullMode    = vk::CullModeFlagBits::eNone,
        .frontFace   = vk::FrontFace::eCounterClockwise,
        .depthBiasEnable         = false,
        .depthBiasConstantFactor = 0.0f,
        .depthBiasClamp          = 0.0f,
        .depthBiasSlopeFactor    = 0.0f,
        .lineWidth = 1.0f,
    };
}

// =============================================================================
void Pipeline::_init_depth() {
    _depth_stencil_info = {
        .depthTestEnable = true,
        .depthWriteEnable = true,
        .depthCompareOp = vk::CompareOp::eLess,
        .depthBoundsTestEnable = false,
        .stencilTestEnable = false,
        .front = { },
        .back = { },
        .minDepthBounds = 0.0f,
        .maxDepthBounds = 1.0f
    };
}

// =============================================================================
void Pipeline::_init_blend() {
    _blend_states = {{
        .blendEnable = false,

        .srcColorBlendFactor = vk::BlendFactor::eOne,
        .dstColorBlendFactor = vk::BlendFactor::eZero,
        .colorBlendOp        = vk::BlendOp::eAdd,

        .srcAlphaBlendFactor = vk::BlendFactor::eOne,
        .dstAlphaBlendFactor = vk::BlendFactor::eZero,
        .alphaBlendOp        = vk::BlendOp::eAdd,

        .colorWriteMask = vk::ColorComponentFlagBits::eR |
                          vk::ColorComponentFlagBits::eG |
                          vk::ColorComponentFlagBits::eB |
                          vk::ColorComponentFlagBits::eA
    }};

    _blend_info = {
        .logicOpEnable   = false,
        .logicOp         = vk::LogicOp::eAnd,
        .attachmentCount = static_cast<uint32_t>(_blend_states.size()),
        .pAttachments    = _blend_states.data(),
        .blendConstants  = std::array<float, 4>{ 0.0f, 0.0f, 0.0f, 0.0f }
    };
}

// =============================================================================
void Pipeline::_init_dynamic_states() {
    _dynamic_states = { 
        vk::DynamicState::eViewport,
        vk::DynamicState::eScissor,
    };

    CONSOLE_TRACE("Dynamic state count: {}", _dynamic_states.size());

    _dynamic_state_info = {
        .dynamicStateCount = static_cast<uint32_t>(_dynamic_states.size()),
        .pDynamicStates    = _dynamic_states.data(),
    };
}

// =============================================================================
Pipeline::Pipeline(const Swapchain &swapchain) :
    _vert               { nullptr },
    _frag               { nullptr },
    _viewport           { },
    _scissor            { },
    _vert_input_info    { },
    _assembly_info      { },
    _viewport_info      { },
    _raster_info        { },
    _blend_info         { },
    _dynamic_state_info { },
    _render_pass        { },
    _layout             { nullptr },
    _pipeline           { nullptr },
    _swapchain          { swapchain }
{ }

} // namespace vkl
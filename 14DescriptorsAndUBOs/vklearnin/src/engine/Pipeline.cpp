#include "vklearnin/vklearnin.hpp"
#include "vklearnin/engine/Pipeline.hpp"

#include "vklearnin/tools/ShaderTools.hpp"
#include "vklearnin/rendering/devices/LogicalDevice.hpp"
#include "vklearnin/engine/FrameData.hpp"

////////////////////////////////////////////////////////////////////////////////
// TODO: I feel like these really shouldn't be here, but...?
#include "vklearnin/mesh/Vertex.hpp"
#include "vklearnin/mesh/XZPlane.hpp"
#include "vklearnin/shaders/CameraData.hpp"
#include "vklearnin/shaders/InstanceData.hpp"
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
void Pipeline::create(const FrameData &frame_data) {
    _init_layout(frame_data);
    _init_render_passes();
    _init_dynamic_states();
    _init_viewport();
    _init_vert_input();
    _init_assembly();
    _init_blend();
    _init_raster();

    vk::GraphicsPipelineCreateInfo pipeline_info {
        .stageCount = static_cast<uint32_t>(_shader_stages.size()),
        .pStages    = _shader_stages.data(),
        .pVertexInputState   = &_vert_input_info,
        .pInputAssemblyState = &_assembly_info,
        .pTessellationState  = nullptr,
        .pViewportState      = &_viewport_info,
        .pRasterizationState = &_raster_info,
        .pMultisampleState   = { },
        .pDepthStencilState  = { },
        .pColorBlendState    = &_blend_info,
        .pDynamicState       = &_dynamic_state_info,
        .layout              = _layout,
        .renderPass          = _renderpass,
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

    create_framebuffers();
}

// =============================================================================
void Pipeline::destroy() {
    CONSOLE_TRACE(
        "Destroying pipeline {:#x}",
        reinterpret_cast<uint64_t>(VkPipeline(_pipeline))
    );

    destroy_framebuffers();

    LogicalDevice::native().destroy(_vert);
    LogicalDevice::native().destroy(_frag);
    LogicalDevice::native().destroy(_renderpass);
    LogicalDevice::native().destroy(_layout);
    LogicalDevice::native().destroy(_pipeline);
}

// =============================================================================
void Pipeline::create_framebuffers() {

    for(uint32_t image_index = 0;
        image_index < RenderConfig::swapchain_image_count;
        ++image_index)
    {
        _framebuffers[image_index].create(_swapchain, *this, image_index);
    }
    
    update_dimensions();
}

// =============================================================================
void Pipeline::destroy_framebuffers() {
    for(auto &buffer : _framebuffers) {
        buffer.destroy();
    }
}

// =============================================================================
void Pipeline::_init_layout(const FrameData &frame_data) {
    vk::PushConstantRange push_constant_ranges[] {{
        .stageFlags = vk::ShaderStageFlagBits::eVertex,
        .offset = 0u,
        .size = sizeof(CameraData)
    }};

    vk::PipelineLayoutCreateInfo pipeline_layout_info {
        .setLayoutCount =
            static_cast<uint32_t>(frame_data.dsc_set_layouts().size()),
        .pSetLayouts = frame_data.dsc_set_layouts().data(),
        .pushConstantRangeCount =
            static_cast<uint32_t>(std::size(push_constant_ranges)),
        .pPushConstantRanges = push_constant_ranges
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
void Pipeline::_init_render_passes() {    
    _attachments.clear();

    vk::AttachmentReference color_attach[] {{
        .attachment = 0u,
        .layout     = vk::ImageLayout::eColorAttachmentOptimal,
    }};

    vk::SubpassDescription subpasses[] {{
        .pipelineBindPoint    = vk::PipelineBindPoint::eGraphics,
        .inputAttachmentCount = 0u,
        .pInputAttachments    = nullptr,
        .colorAttachmentCount = static_cast<uint32_t>(std::size(color_attach)),
        .pColorAttachments    = color_attach,
        .pResolveAttachments  = nullptr,
        .pDepthStencilAttachment = nullptr,
        .preserveAttachmentCount = 0u,
        .pPreserveAttachments    = 0u,
    }};

    // render target attachment
    _attachments.emplace_back(vk::AttachmentDescription {
        .format         = _swapchain.surface_format(),
        .samples        = vk::SampleCountFlagBits::e1,
        .loadOp         = vk::AttachmentLoadOp::eClear,
        .storeOp        = vk::AttachmentStoreOp::eStore,
        .stencilLoadOp  = vk::AttachmentLoadOp::eDontCare,
        .stencilStoreOp = vk::AttachmentStoreOp::eDontCare,
        .initialLayout  = vk::ImageLayout::eUndefined,
        .finalLayout    = vk::ImageLayout::ePresentSrcKHR,
    });

    // given the single render pass for this setup, specifying the following
    // dependency ensures the render pass doesn't begin until there's an image
    // available
    vk::SubpassDependency dependencies[] {
    {
        .srcSubpass      = VK_SUBPASS_EXTERNAL,
        .dstSubpass      = 0u,
        .srcStageMask    = vk::PipelineStageFlagBits::eBottomOfPipe,
        .dstStageMask    = vk::PipelineStageFlagBits::eColorAttachmentOutput,
        .srcAccessMask   = vk::AccessFlagBits::eMemoryRead,
        .dstAccessMask   = vk::AccessFlagBits::eColorAttachmentWrite,
        .dependencyFlags = vk::DependencyFlagBits::eByRegion
    },
    {
        .srcSubpass      = 0u,
        .dstSubpass      = VK_SUBPASS_EXTERNAL,
        .srcStageMask    = vk::PipelineStageFlagBits::eColorAttachmentOutput,
        .dstStageMask    = vk::PipelineStageFlagBits::eBottomOfPipe,
        .srcAccessMask   = vk::AccessFlagBits::eColorAttachmentWrite,
        .dstAccessMask   = vk::AccessFlagBits::eMemoryRead,
        .dependencyFlags = vk::DependencyFlagBits::eByRegion
    }};

    vk::RenderPassCreateInfo renderpass_info {
        .attachmentCount = static_cast<uint32_t>(_attachments.size()),
        .pAttachments    = _attachments.data(),
        .subpassCount    = static_cast<uint32_t>(std::size(subpasses)),
        .pSubpasses      = subpasses,
        .dependencyCount = static_cast<uint32_t>(std::size(dependencies)),
        .pDependencies   = dependencies,
    };

    CONSOLE_TRACE(
        "Creating render pass with {} {} and {} {}",
        renderpass_info.attachmentCount,
        renderpass_info.attachmentCount == 1 ? "attachment" : "attachments",
        renderpass_info.subpassCount,
        renderpass_info.subpassCount == 1 ? "subpass" : "subpasses"
    );

    auto result = LogicalDevice::native().createRenderPass(renderpass_info);
    if(result.result != vk::Result::eSuccess) {
        CONSOLE_CRITICAL("Failed to create render pass");
    }
    _renderpass = result.value;
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
void Pipeline::_init_blend() {
    _blend_attachments = {{
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
        .attachmentCount = static_cast<uint32_t>(_blend_attachments.size()),
        .pAttachments    = _blend_attachments.data(),
        .blendConstants  = std::array<float, 4>{ 0.0f, 0.0f, 0.0f, 0.0f }
    };
}

// =============================================================================
void Pipeline::_init_raster() {
    _raster_info = {
        .depthClampEnable = false,
        .rasterizerDiscardEnable = false,
        .polygonMode = vk::PolygonMode::eFill,
        .cullMode    = vk::CullModeFlagBits::eBack,
        .frontFace   = vk::FrontFace::eCounterClockwise,
        .depthBiasEnable         = false,
        .depthBiasConstantFactor = 0.0f,
        .depthBiasClamp          = 0.0f,
        .depthBiasSlopeFactor    = 0.0f,
        .lineWidth = 1.0f,
    };
}

// =============================================================================
Pipeline::Pipeline(const Swapchain &swapchain) :
    _vert               { nullptr },
    _frag               { nullptr },
    _viewport           { },
    _scissor            { },
    _layout             { nullptr },
    _renderpass         { nullptr },
    _dynamic_state_info { },
    _viewport_info      { },
    _vert_input_info    { },
    _assembly_info      { },
    _blend_info         { },
    _raster_info        { },
    _pipeline           { nullptr },
    _swapchain          { swapchain }
{    
    _attachments.reserve(10);
    _framebuffers.resize(RenderConfig::swapchain_image_count);
}

} // namespace vkl
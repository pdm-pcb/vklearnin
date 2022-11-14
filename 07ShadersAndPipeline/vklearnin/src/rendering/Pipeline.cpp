#include "vklearnin/vklearnin.hpp"
#include "vklearnin/rendering/Pipeline.hpp"

#include "vklearnin/tools/ShaderTools.hpp"
#include "vklearnin/tools/ImageTools.hpp"

namespace vkl {

// =============================================================================
void Pipeline::vertex_from_binary(const char *filepath) {        
    _vert = ShaderTools::module_from_binary(filepath, _logical_device);
    _shader_stages.emplace_back(vk::PipelineShaderStageCreateInfo {
        .stage = vk::ShaderStageFlagBits::eVertex,
        .module = _vert,
        .pName = "main",
    });
}

// =============================================================================
void Pipeline::fragment_from_binary(const char *filepath) {        
    _frag = ShaderTools::module_from_binary(filepath, _logical_device);
    _shader_stages.emplace_back(vk::PipelineShaderStageCreateInfo {
        .stage = vk::ShaderStageFlagBits::eFragment,
        .module = _frag,
        .pName = "main",
    });
}

// =============================================================================
void Pipeline::init_layout() {
    vk::PipelineLayoutCreateInfo pipeline_layout_info { };

    auto result =
        _logical_device.createPipelineLayout(pipeline_layout_info);
    if(result.result != vk::Result::eSuccess) {
        CONSOLE_CRITICAL("Could not create pipeline layout");
    }
    _layout = result.value;

    CONSOLE_TRACE("Setting a pipeline layout with {} descriptor sets",
                  pipeline_layout_info.setLayoutCount);
}

// =============================================================================
void Pipeline::init_render_passes() {    
    _attachments.clear();

    vk::AttachmentReference color_refs[] {{
        .attachment = 0u,
        .layout     = vk::ImageLayout::eColorAttachmentOptimal,
    }};

    vk::SubpassDescription subpasses[] {{
        .pipelineBindPoint       = vk::PipelineBindPoint::eGraphics,
        .inputAttachmentCount    = 0u,
        .pInputAttachments       = nullptr,
        .colorAttachmentCount    = static_cast<uint32_t>(std::size(color_refs)),
        .pColorAttachments       = color_refs,
        .pResolveAttachments     = nullptr,
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
    vk::SubpassDependency dependencies[] {{
        .srcSubpass      = VK_SUBPASS_EXTERNAL,
        .dstSubpass      = 0u,
        .srcStageMask    = vk::PipelineStageFlagBits::eColorAttachmentOutput |
                           vk::PipelineStageFlagBits::eEarlyFragmentTests,
        .dstStageMask    = vk::PipelineStageFlagBits::eColorAttachmentOutput |
                           vk::PipelineStageFlagBits::eEarlyFragmentTests,
        .srcAccessMask   = { },
        .dstAccessMask   = vk::AccessFlagBits::eColorAttachmentWrite |
                           vk::AccessFlagBits::eDepthStencilAttachmentWrite,
        .dependencyFlags = { }
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

    auto result = _logical_device.createRenderPass(renderpass_info);
    if(result.result != vk::Result::eSuccess) {
        CONSOLE_CRITICAL("Failed to create render pass");
    }
    _renderpass = result.value;
}

// =============================================================================
void Pipeline::create() {
    // these two will allow resizing of the viewport and/or scissor rectangle
    // without recreating the entire graphcis pipeline
    vk::DynamicState dynamic_states[] = { 
        vk::DynamicState::eViewport,
        vk::DynamicState::eScissor,
    };

    CONSOLE_TRACE("Dynamic state count: {}", std::size(dynamic_states));

    vk::PipelineDynamicStateCreateInfo dynamic_state_info {
        .dynamicStateCount = static_cast<uint32_t>(std::size(dynamic_states)),
        .pDynamicStates    = dynamic_states,
    };

    vk::PipelineViewportStateCreateInfo viewport_info {
        .viewportCount = 1u,
        .pViewports = nullptr,
        .scissorCount = 1u,
        .pScissors = nullptr,
    };

    CONSOLE_TRACE("Viewport count: {}", viewport_info.viewportCount);
    CONSOLE_TRACE("Scissor count:  {}", viewport_info.scissorCount);

    // viewports and scissor rectangles are all sized the same as the surface
    // at present
    update_dimensions();

    vk::PipelineVertexInputStateCreateInfo vertex_info { };

    // assemble a triangle list, like anything at this point
    vk::PipelineInputAssemblyStateCreateInfo assembly_info {
        .topology = vk::PrimitiveTopology::eTriangleList,
        .primitiveRestartEnable = false
    };

    vk::PipelineColorBlendAttachmentState blend_attachments[] = {{
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

    vk::PipelineColorBlendStateCreateInfo blend_info {
        .logicOpEnable   = false,
        .logicOp         = vk::LogicOp::eAnd,
        .attachmentCount = static_cast<uint32_t>(std::size(blend_attachments)),
        .pAttachments    = blend_attachments,
        .blendConstants  = std::array<float, 4>{ 0.0f, 0.0f, 0.0f, 0.0f }
    };

    // standard rasterization details: fill, cull backfaces
    vk::PipelineRasterizationStateCreateInfo rasterizer {
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

    vk::GraphicsPipelineCreateInfo pipeline_info {
        .stageCount = static_cast<uint32_t>(_shader_stages.size()),
        .pStages    = _shader_stages.data(),
        .pVertexInputState   = &vertex_info,
        .pInputAssemblyState = &assembly_info,
        .pTessellationState  = nullptr,
        .pViewportState      = &viewport_info,
        .pRasterizationState = &rasterizer,
        .pMultisampleState   = { },
        .pDepthStencilState  = { },
        .pColorBlendState    = &blend_info,
        .pDynamicState       = &dynamic_state_info,
        .layout              = _layout,
        .renderPass          = _renderpass,
        .subpass             = 0u,
        .basePipelineHandle  = nullptr,
        .basePipelineIndex   = -1,
    };

    auto pipeline_return =
        _logical_device.createGraphicsPipeline({ }, pipeline_info);

    if(pipeline_return.result != vk::Result::eSuccess) {
        CONSOLE_CRITICAL("Unable to create graphics pipelines.");
    }

    _pipeline = pipeline_return.value;
}

// =============================================================================
Pipeline::Pipeline(const vk::Device &logical_device,
                   const Swapchain &swapchain) :
    _vert           { nullptr },
    _frag           { nullptr },
    _viewport       { },
    _scissor        { },
    _renderpass     { nullptr },
    _layout         { nullptr },
    _pipeline       { nullptr },
    _logical_device { logical_device },
    _swapchain      { swapchain }
{    
    _attachments.reserve(10);
}

Pipeline::~Pipeline() {           
    _logical_device.destroy(_vert);
    _logical_device.destroy(_frag);
    _logical_device.destroy(_renderpass);
    _logical_device.destroy(_layout);
    _logical_device.destroy(_pipeline);
}

} // namespace vkl
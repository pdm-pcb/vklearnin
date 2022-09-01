#include "vklearnin/common.hpp"
#include "vklearnin/Pipeline.hpp"

#include "vklearnin/Instance.hpp"
#include "vklearnin/Buffers/DepthBuffer.hpp"

// =============================================================================
void Pipeline::vertex_from_binary(const char *filepath) {
    _vert = Shader::module_from_binary(filepath, _instance.logical_device());
    _shader_stages.emplace_back(vk::PipelineShaderStageCreateInfo {
        .stage = vk::ShaderStageFlagBits::eVertex,
        .module = _vert,
        .pName = "main",
    });
}

// =============================================================================
void Pipeline::fragment_from_binary(const char *filepath) {
    _frag = Shader::module_from_binary(filepath, _instance.logical_device());
    _shader_stages.emplace_back(vk::PipelineShaderStageCreateInfo {
        .stage = vk::ShaderStageFlagBits::eFragment,
        .module = _frag,
        .pName = "main",
    });
}

// =============================================================================
void Pipeline::init_render_passes(const Swapchain &swapchain)
{
    CONSOLE_INFO("");

    _init_depth_buffer(swapchain);

    vk::AttachmentReference color_refs[] {{
        // the zeroth attachment is the fragment shader's outColor layout
        // location
        .attachment = 0u,
        .layout     = vk::ImageLayout::eColorAttachmentOptimal,
    }};

    vk::AttachmentReference depth_ref {
        .attachment = 1,
        .layout = vk::ImageLayout::eDepthStencilAttachmentOptimal,
    };

    vk::SubpassDescription subpasses[] {{
        .pipelineBindPoint    = vk::PipelineBindPoint::eGraphics,
        .inputAttachmentCount = 0u,
        .pInputAttachments    = nullptr,
        .colorAttachmentCount = static_cast<uint32_t>(std::size(color_refs)),
        .pColorAttachments    = color_refs,
        .pResolveAttachments  = nullptr,
        .pDepthStencilAttachment = &depth_ref,
        .preserveAttachmentCount = 0u,
        .pPreserveAttachments    = 0u,
    }};

    vk::AttachmentDescription attachments[] {
        // color attachment description
        {
            .format         = swapchain.color_format(),
            .samples        = vk::SampleCountFlagBits::e1,
            .loadOp         = vk::AttachmentLoadOp::eClear,
            .storeOp        = vk::AttachmentStoreOp::eStore,
            .stencilLoadOp  = vk::AttachmentLoadOp::eDontCare,
            .stencilStoreOp = vk::AttachmentStoreOp::eDontCare,
            .initialLayout  = vk::ImageLayout::eUndefined,
            .finalLayout    = vk::ImageLayout::ePresentSrcKHR,
        },
        // depth buffer attachment description
        {
            .format         = _depth_buffer->format(),
            .samples        = vk::SampleCountFlagBits::e1,
            .loadOp         = vk::AttachmentLoadOp::eClear,
            .storeOp        = vk::AttachmentStoreOp::eDontCare,
            .stencilLoadOp  = vk::AttachmentLoadOp::eDontCare,
            .stencilStoreOp = vk::AttachmentStoreOp::eDontCare,
            .initialLayout  = vk::ImageLayout::eUndefined,
            .finalLayout    = vk::ImageLayout::eDepthStencilAttachmentOptimal,
        }
    };

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
        .attachmentCount = static_cast<uint32_t>(std::size(attachments)),
        .pAttachments    = attachments,
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

    _renderpass = _instance.logical_device().createRenderPass(renderpass_info);
}

// =============================================================================
void Pipeline::init_layout(const vk::DescriptorSetLayout &desc_set_layout) {
    CONSOLE_INFO("");

    vk::PipelineLayoutCreateInfo pipeline_layout_info {
        .setLayoutCount = 1u,
        .pSetLayouts = &desc_set_layout,
        .pushConstantRangeCount = 0u,
        .pPushConstantRanges = nullptr
    };

    _layout =
        _instance.logical_device().createPipelineLayout(pipeline_layout_info);
}

// =============================================================================
void Pipeline::init_pipeline(const Swapchain &swapchain)
{
    CONSOLE_INFO("");

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

    // grab the data from the newly formulated Vertex class
    auto binding_desc   = Vertex::binding_desc();
    auto attribute_desc = Vertex::attribute_desc();

    // and hand them over as part of the pipeline's input state
    vk::PipelineVertexInputStateCreateInfo vertex_info {
        .vertexBindingDescriptionCount =
            static_cast<uint32_t>(binding_desc.size()),
        .pVertexBindingDescriptions = binding_desc.data(),
        .vertexAttributeDescriptionCount =
            static_cast<uint32_t>(attribute_desc.size()),
        .pVertexAttributeDescriptions = attribute_desc.data(),
    };

    // assemble a triangle list, like anything at this point
    vk::PipelineInputAssemblyStateCreateInfo assembly_info {
        .topology = vk::PrimitiveTopology::eTriangleList,
        .primitiveRestartEnable = false
    };

    // viewports and scissor rectangles are all sized the same as the surface
    // at present
    update_dimensions(swapchain);

    vk::PipelineViewportStateCreateInfo viewport_info {
        .viewportCount = 1u,
        .pViewports = nullptr,
        .scissorCount = 1u,
        .pScissors = nullptr,
    };

    CONSOLE_TRACE("Viewport count: {}", viewport_info.viewportCount);
    CONSOLE_TRACE("Scissor count:  {}", viewport_info.scissorCount);

    // standard rasterization details: fill, clockwise, cull backfaces
    vk::PipelineRasterizationStateCreateInfo rasterizer {
        .depthClampEnable = false,
        .rasterizerDiscardEnable = false,
        .polygonMode = vk::PolygonMode::eFill,
        .cullMode    = vk::CullModeFlagBits::eBack,
        .frontFace   = vk::FrontFace::eClockwise,
        .depthBiasEnable         = false,
        .depthBiasConstantFactor = 0.0f,
        .depthBiasClamp          = 0.0f,
        .depthBiasSlopeFactor    = 0.0f,
        .lineWidth = 1.0f,
    };

    // nothing to do here yet, but it'll be fun when we can
    vk::PipelineMultisampleStateCreateInfo multisampling {
        .rasterizationSamples  = vk::SampleCountFlagBits::e1,
        .sampleShadingEnable   = false,
        .minSampleShading      = 1.0f,
        .pSampleMask           = nullptr,
        .alphaToCoverageEnable = false,
        .alphaToOneEnable      = false,
    };

    vk::PipelineDepthStencilStateCreateInfo depth_info {
        .depthTestEnable = VK_TRUE,
        .depthWriteEnable = VK_TRUE,
        .depthCompareOp = vk::CompareOp::eLess,
        .depthBoundsTestEnable = false,
        .stencilTestEnable = false,
        .front = {
            .failOp      = vk::StencilOp::eKeep,
            .passOp      = vk::StencilOp::eKeep,
            .depthFailOp = vk::StencilOp::eKeep,
            .compareOp   = vk::CompareOp::eNever,
            .compareMask = { },
            .writeMask   = { },
            .reference   = { },
        },
        .back = {
            .failOp      = vk::StencilOp::eKeep,
            .passOp      = vk::StencilOp::eKeep,
            .depthFailOp = vk::StencilOp::eKeep,
            .compareOp   = vk::CompareOp::eNever,
            .compareMask = { },
            .writeMask   = { },
            .reference   = { },
        },
        .minDepthBounds = 0.0f,
        .maxDepthBounds = 1.0f,
    };

    // likewise here - transparency is a luxury I cannot yet afford
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

    CONSOLE_TRACE("Blend attachment count: {}", blend_info.attachmentCount);

    vk::GraphicsPipelineCreateInfo pipeline_info {
        .stageCount = static_cast<uint32_t>(_shader_stages.size()),
        .pStages    = _shader_stages.data(),
        .pVertexInputState   = &vertex_info,
        .pInputAssemblyState = &assembly_info,
        .pTessellationState  = nullptr,
        .pViewportState      = &viewport_info,
        .pRasterizationState = &rasterizer,
        .pMultisampleState   = &multisampling,
        .pDepthStencilState  = &depth_info,
        .pColorBlendState    = &blend_info,
        .pDynamicState       = &dynamic_state_info,
        .layout              = _layout,
        .renderPass          = _renderpass,
        .subpass             = 0u,
        .basePipelineHandle  = nullptr,
        .basePipelineIndex   = -1,
    };

    auto pipeline_return =
        _instance.logical_device().createGraphicsPipeline({ }, pipeline_info);

    if(pipeline_return.result != vk::Result::eSuccess) {
        CONSOLE_CRITICAL("Unable to create graphics pipelines.");
    }

    _pipeline = pipeline_return.value;
}

// =============================================================================
void Pipeline::_init_depth_buffer(const Swapchain &swapchain) {
    if(_depth_buffer != nullptr) {
        delete _depth_buffer;
    }

    _depth_buffer = new DepthBuffer(_instance, swapchain);
    _depth_buffer->init_image(
        vk::ImageTiling::eOptimal,
        vk::FormatFeatureFlagBits::eDepthStencilAttachment
    );
    _depth_buffer->init_image_view();
}

// =============================================================================
Pipeline::Pipeline(const Instance &instance) :
    _vert         { nullptr },
    _frag         { nullptr },
    _viewport     { },
    _scissor      { },
    _depth_buffer { nullptr },
    _renderpass   { nullptr },
    _layout       { nullptr },
    _pipeline     { nullptr },
    _instance     { instance }
{
    CONSOLE_INFO("");
}

Pipeline::~Pipeline() {
    CONSOLE_INFO("");

    delete _depth_buffer;

    _instance.logical_device().destroy(_vert);
    _instance.logical_device().destroy(_frag);
    _instance.logical_device().destroy(_renderpass);
    _instance.logical_device().destroy(_layout);
    _instance.logical_device().destroy(_pipeline);
}
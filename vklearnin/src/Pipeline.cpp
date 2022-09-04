#include "vklearnin/common.hpp"
#include "vklearnin/Pipeline.hpp"

#include "vklearnin/Instance.hpp"
#include "vklearnin/Buffers/DepthBuffer.hpp"

// =============================================================================
void Pipeline::vertex_from_binary(const char *filepath) {
    CONSOLE_INFO("");
    
    _vert = Shader::module_from_binary(filepath, _instance.logical_device());
    _shader_stages.emplace_back(vk::PipelineShaderStageCreateInfo {
        .stage = vk::ShaderStageFlagBits::eVertex,
        .module = _vert,
        .pName = "main",
    });
}

// =============================================================================
void Pipeline::fragment_from_binary(const char *filepath) {
    CONSOLE_INFO("");
    
    _frag = Shader::module_from_binary(filepath, _instance.logical_device());
    _shader_stages.emplace_back(vk::PipelineShaderStageCreateInfo {
        .stage = vk::ShaderStageFlagBits::eFragment,
        .module = _frag,
        .pName = "main",
    });
}

// =============================================================================
void Pipeline::init_render_passes(const Swapchain &swapchain,
                                  const uint32_t msaa_sample_count)
{
    CONSOLE_INFO("");

    _attachments.clear();

    _check_msaa(msaa_sample_count);
    _init_depth_buffer(swapchain);

    vk::AttachmentReference color_refs[] {{
        .attachment = 0u,
        .layout     = vk::ImageLayout::eColorAttachmentOptimal,
    }};

    vk::AttachmentReference depth_ref {
        .attachment = 1u,
        .layout = vk::ImageLayout::eDepthStencilAttachmentOptimal,
    };

    vk::AttachmentReference resolve_ref {
        .attachment = 2u,
        .layout = vk::ImageLayout::eColorAttachmentOptimal,
    };

    vk::SubpassDescription subpasses[] {{
        .pipelineBindPoint    = vk::PipelineBindPoint::eGraphics,
        .inputAttachmentCount = 0u,
        .pInputAttachments    = nullptr,
        .colorAttachmentCount = static_cast<uint32_t>(std::size(color_refs)),
        .pColorAttachments    = color_refs,
        .pResolveAttachments =
            (_sample_count > 1u ? &resolve_ref : nullptr),
        .pDepthStencilAttachment = &depth_ref,
        .preserveAttachmentCount = 0u,
        .pPreserveAttachments    = 0u,
    }};

    if(_sample_count > 1u) {
        _init_color_buffer(swapchain);

        // color buffer (msaa) attachment description
        _attachments.emplace_back(vk::AttachmentDescription {
            .format         = swapchain.color_format(),
            .samples        = _sample_flags,
            .loadOp         = vk::AttachmentLoadOp::eClear,
            .storeOp        = vk::AttachmentStoreOp::eDontCare,
            .stencilLoadOp  = vk::AttachmentLoadOp::eDontCare,
            .stencilStoreOp = vk::AttachmentStoreOp::eDontCare,
            .initialLayout  = vk::ImageLayout::eUndefined,
            .finalLayout    = vk::ImageLayout::eColorAttachmentOptimal,
        });

        // depth buffer attachment description
        _attachments.emplace_back(_depth_buffer->attachment_desc());

        // final presentation attachment
        _attachments.emplace_back(vk::AttachmentDescription {
            .format         = swapchain.color_format(),
            .samples        = vk::SampleCountFlagBits::e1,
            .loadOp         = vk::AttachmentLoadOp::eDontCare,
            .storeOp        = vk::AttachmentStoreOp::eStore,
            .stencilLoadOp  = vk::AttachmentLoadOp::eDontCare,
            .stencilStoreOp = vk::AttachmentStoreOp::eDontCare,
            .initialLayout  = vk::ImageLayout::eUndefined,
            .finalLayout    = vk::ImageLayout::ePresentSrcKHR,
        });
    }
    else {
        // render target attachment
        _attachments.emplace_back(vk::AttachmentDescription {
            .format         = swapchain.color_format(),
            .samples        = vk::SampleCountFlagBits::e1,
            .loadOp         = vk::AttachmentLoadOp::eClear,
            .storeOp        = vk::AttachmentStoreOp::eStore,
            .stencilLoadOp  = vk::AttachmentLoadOp::eDontCare,
            .stencilStoreOp = vk::AttachmentStoreOp::eDontCare,
            .initialLayout  = vk::ImageLayout::eUndefined,
            .finalLayout    = vk::ImageLayout::ePresentSrcKHR,
        });

        // depth buffer attachment description
        _attachments.emplace_back(_depth_buffer->attachment_desc());
    }

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

    _renderpass = _instance.logical_device().createRenderPass(renderpass_info);
}

// =============================================================================
void
Pipeline::init_layout(const std::vector<vk::DescriptorSetLayout> &desc_layouts)
{
    CONSOLE_INFO("");

    vk::PipelineLayoutCreateInfo pipeline_layout_info {
        .setLayoutCount = static_cast<uint32_t>(desc_layouts.size()),
        .pSetLayouts = desc_layouts.data(),
        .pushConstantRangeCount = 0u,
        .pPushConstantRanges = nullptr
    };

    _layout =
        _instance.logical_device().createPipelineLayout(pipeline_layout_info);

    CONSOLE_TRACE("Setting a pipeline layout with {} descriptor sets",
                  pipeline_layout_info.setLayoutCount);
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
        .frontFace   = vk::FrontFace::eCounterClockwise,
        .depthBiasEnable         = false,
        .depthBiasConstantFactor = 0.0f,
        .depthBiasClamp          = 0.0f,
        .depthBiasSlopeFactor    = 0.0f,
        .lineWidth = 1.0f,
    };

    vk::PipelineMultisampleStateCreateInfo multisampling {
        .rasterizationSamples  = _sample_flags,
        .sampleShadingEnable   = true,
        .minSampleShading      = SAMPLE_SHADING_RATE,
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

void Pipeline::_check_msaa(const uint32_t msaa_sample_count) {
    switch(msaa_sample_count) {
        case 1u:  break;
        case 2u:  _sample_flags = vk::SampleCountFlagBits::e2;  break;
        case 4u:  _sample_flags = vk::SampleCountFlagBits::e4;  break;
        case 8u:  _sample_flags = vk::SampleCountFlagBits::e8;  break;
        case 16u: _sample_flags = vk::SampleCountFlagBits::e16; break;
        case 32u: _sample_flags = vk::SampleCountFlagBits::e32; break;
        case 64u: _sample_flags = vk::SampleCountFlagBits::e64; break;
        default:
            CONSOLE_CRITICAL(
                "Unknown MSAA pass count {} requested",
                msaa_sample_count
            );
            break;
    }

    if((_sample_flags & _instance.supported_msaa()) != _sample_flags)
    {
        CONSOLE_CRITICAL(
            "Requested MSAA level {} is not compatible with selected device.",
            msaa_sample_count
        );
    }

    _sample_count = msaa_sample_count;
}

// =============================================================================
void Pipeline::_init_color_buffer(const Swapchain &swapchain) {
    CONSOLE_INFO("");
    
    auto[width, height] = swapchain.extent();

    if(_color_buffer_alloc != nullptr) {
        _instance.logical_device().destroy(_color_buffer_view);
        ImageTools::destroy_image(_color_buffer_handle, _color_buffer_alloc);
        _color_buffer_alloc = nullptr;
    }

    ImageTools::init_image(
        { width, height, 1u },
        swapchain.color_format(),
        vk::ImageTiling::eOptimal,
        1u,
        _sample_flags,
        _color_buffer_handle,
        vk::ImageUsageFlagBits::eTransientAttachment |
        vk::ImageUsageFlagBits::eColorAttachment,
        _color_buffer_alloc,
        ::VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE,
        ::VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT,
        "color_buffer"
    );

    _color_buffer_view = ImageTools::init_view(
        _color_buffer_handle,
        swapchain.color_format(), 1u,
        vk::ImageAspectFlagBits::eColor,
        _instance.logical_device()
    );
}

// =============================================================================
void Pipeline::_init_depth_buffer(const Swapchain &swapchain) {
    CONSOLE_INFO("");

    if(_depth_buffer != nullptr) {
        delete _depth_buffer;
    }

    _depth_buffer = new DepthBuffer(_instance, swapchain, _sample_flags);
    _depth_buffer->init_image(
        vk::ImageTiling::eOptimal,
        vk::FormatFeatureFlagBits::eDepthStencilAttachment
    );
    _depth_buffer->init_image_view();
}

// =============================================================================
Pipeline::Pipeline(const Instance &instance) :
    _vert     { nullptr },
    _frag     { nullptr },
    _viewport { },
    _scissor  { },
    _sample_flags { vk::SampleCountFlagBits::e1 },
    _sample_count { 1u },
    _color_buffer_handle { },
    _color_buffer_alloc  { nullptr },
    _color_buffer_view   { },
    _depth_buffer { nullptr },
    _renderpass { nullptr },
    _layout     { nullptr },
    _pipeline   { nullptr },
    _instance   { instance }
{
    CONSOLE_INFO("");

    _attachments.reserve(10);
}

Pipeline::~Pipeline() {
    CONSOLE_INFO("");

    delete _depth_buffer;

    _instance.logical_device().destroy(_color_buffer_view);
    ImageTools::destroy_image(_color_buffer_handle, _color_buffer_alloc);

    _instance.logical_device().destroy(_vert);
    _instance.logical_device().destroy(_frag);
    _instance.logical_device().destroy(_renderpass);
    _instance.logical_device().destroy(_layout);
    _instance.logical_device().destroy(_pipeline);
}
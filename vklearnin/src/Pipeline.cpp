#include "vklearnin/common.hpp"
#include "vklearnin/Pipeline.hpp"

#include "vklearnin/Instance.hpp"
#include "vklearnin/Buffers/DepthBuffer.hpp"

// =============================================================================
void Pipeline::vertex_from_binary(const char *filepath) {
    _vert = Shader::module_from_binary(filepath, _instance.logical_device());

    _shader_stages.emplace_back(::VkPipelineShaderStageCreateInfo { });
    auto &stage = _shader_stages.back();

    stage.sType  = ::VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    stage.stage  = ::VK_SHADER_STAGE_VERTEX_BIT;
    stage.module = _vert;
    stage.pName  = "main";
}

// =============================================================================
void Pipeline::fragment_from_binary(const char *filepath) {
    _frag = Shader::module_from_binary(filepath, _instance.logical_device());

    _shader_stages.emplace_back(::VkPipelineShaderStageCreateInfo { });
    auto &stage = _shader_stages.back();

    stage.sType  = ::VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    stage.stage  = ::VK_SHADER_STAGE_FRAGMENT_BIT;
    stage.module = _frag;
    stage.pName  = "main";
}

// =============================================================================
void Pipeline::init_render_passes(const Swapchain &swapchain)
{
    CONSOLE_INFO("");

    _init_depth_buffer(swapchain);

    ::VkAttachmentReference color_refs[] {{
        // the zeroth attachment is the fragment shader's outColor layout
        // location
        .attachment = 0u,
        .layout     = ::VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
    }};

    ::VkAttachmentReference depth_ref {
        .attachment = 1,
        .layout = ::VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
    };

    ::VkSubpassDescription subpasses[] {{
        .flags = 0u,
        .pipelineBindPoint    = ::VK_PIPELINE_BIND_POINT_GRAPHICS,
        .inputAttachmentCount = 0u,
        .pInputAttachments    = nullptr,
        .colorAttachmentCount = std::size(color_refs),
        .pColorAttachments    = color_refs,
        .pResolveAttachments  = nullptr,
        .pDepthStencilAttachment = &depth_ref,
        .preserveAttachmentCount = 0u,
        .pPreserveAttachments    = 0u,
    }};

    ::VkAttachmentDescription attachments[] {
        // color attachment description
        {
            .flags          = 0u,
            .format         = swapchain.color_format(),
            .samples        = ::VK_SAMPLE_COUNT_1_BIT,
            .loadOp         = ::VK_ATTACHMENT_LOAD_OP_CLEAR,
            .storeOp        = ::VK_ATTACHMENT_STORE_OP_STORE,
            .stencilLoadOp  = ::VK_ATTACHMENT_LOAD_OP_DONT_CARE,
            .stencilStoreOp = ::VK_ATTACHMENT_STORE_OP_DONT_CARE,
            .initialLayout  = ::VK_IMAGE_LAYOUT_UNDEFINED,
            .finalLayout    = ::VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
        },
        // depth buffer attachment description
        {
            .flags          = 0u,
            .format         = _depth_buffer->format(),
            .samples        = ::VK_SAMPLE_COUNT_1_BIT,
            .loadOp         = ::VK_ATTACHMENT_LOAD_OP_CLEAR,
            .storeOp        = ::VK_ATTACHMENT_STORE_OP_DONT_CARE,
            .stencilLoadOp  = ::VK_ATTACHMENT_LOAD_OP_DONT_CARE,
            .stencilStoreOp = ::VK_ATTACHMENT_STORE_OP_DONT_CARE,
            .initialLayout  = ::VK_IMAGE_LAYOUT_UNDEFINED,
            .finalLayout = ::VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
        }
    };

    // given the single render pass for this setup, specifying the following
    // dependency ensures the render pass doesn't begin until there's an image
    // available
    ::VkSubpassDependency dependencies[] {{
        .srcSubpass      = VK_SUBPASS_EXTERNAL,
        .dstSubpass      = 0u,
        .srcStageMask    = ::VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT |
                           ::VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
        .dstStageMask    = ::VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT |
                           ::VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
        .srcAccessMask   = 0u,
        .dstAccessMask   = ::VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT |
                           ::VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
        .dependencyFlags = 0u
    }};

    ::VkRenderPassCreateInfo renderpass_info {
        .sType = ::VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0u,
        .attachmentCount = std::size(attachments),
        .pAttachments    = attachments,
        .subpassCount    = std::size(subpasses),
        .pSubpasses      = subpasses,
        .dependencyCount = std::size(dependencies),
        .pDependencies   = dependencies,
    };

    CONSOLE_TRACE(
        "Creating render pass with {} {} and {} {}",
        renderpass_info.attachmentCount,
        renderpass_info.attachmentCount == 1 ? "attachment" : "attachments",
        renderpass_info.subpassCount,
        renderpass_info.subpassCount == 1 ? "subpass" : "subpasses"
    );

    ::VkResult result = ::vkCreateRenderPass(
        _instance.logical_device(),
        &renderpass_info,
        nullptr,
        &_renderpass
    );

    if(result != ::VK_SUCCESS) {
        CONSOLE_ERROR("Could not create default render pass");
    }
}

// =============================================================================
void Pipeline::init_layout(const ::VkDescriptorSetLayout &desc_set_layout) {
    CONSOLE_INFO("");

    ::VkPipelineLayoutCreateInfo pipeline_layout_info {
        .sType = ::VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0u,
        .setLayoutCount = 1u,
        .pSetLayouts = &desc_set_layout,
        .pushConstantRangeCount = 0u,
        .pPushConstantRanges = nullptr
    };

    auto result = ::vkCreatePipelineLayout(
        _instance.logical_device(),
        &pipeline_layout_info,
        nullptr,
        &_layout
    );

    if(result != ::VK_SUCCESS) {
        CONSOLE_ERROR("Unable to initialize pipeline layout.");
    }
}

// =============================================================================
void Pipeline::init_pipeline(const Swapchain &swapchain)
{
    CONSOLE_INFO("");

    // these two will allow resizing of the viewport and/or scissor rectangle
    // without recreating the entire graphcis pipeline
    ::VkDynamicState dynamic_states[] = { 
        ::VK_DYNAMIC_STATE_VIEWPORT,
        ::VK_DYNAMIC_STATE_SCISSOR
    };

    CONSOLE_TRACE("Dynamic state count: {}", std::size(dynamic_states));

    ::VkPipelineDynamicStateCreateInfo dynamic_state_info {
        .sType = ::VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0u,
        .dynamicStateCount = std::size(dynamic_states),
        .pDynamicStates    = dynamic_states,
    };

    // grab the data from the newly formulated Vertex class
    auto binding_desc   = Vertex::binding_desc();
    auto attribute_desc = Vertex::attribute_desc();

    // and hand them over as part of the pipeline's input state
    ::VkPipelineVertexInputStateCreateInfo vertex_info {
        .sType = ::VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0u,
        .vertexBindingDescriptionCount =
            static_cast<uint32_t>(binding_desc.size()),
        .pVertexBindingDescriptions = binding_desc.data(),
        .vertexAttributeDescriptionCount =
            static_cast<uint32_t>(attribute_desc.size()),
        .pVertexAttributeDescriptions = attribute_desc.data(),
    };

    // assemble a triangle list, like anything at this point
    ::VkPipelineInputAssemblyStateCreateInfo assembly_info {
        .sType = ::VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0u,
        .topology = ::VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
        .primitiveRestartEnable = VK_FALSE
    };

    // viewports and scissor rectangles are all sized the same as the surface
    // at present
    update_dimensions(swapchain);

    ::VkPipelineViewportStateCreateInfo viewport_info {
        .sType = ::VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0u,
        .viewportCount = 1u,
        .pViewports = nullptr,
        .scissorCount = 1u,
        .pScissors = nullptr,
    };

    CONSOLE_TRACE("Viewport count: {}", viewport_info.viewportCount);
    CONSOLE_TRACE("Scissor count:  {}", viewport_info.scissorCount);

    // standard rasterization details: fill, clockwise, cull backfaces
    ::VkPipelineRasterizationStateCreateInfo rasterizer {
        .sType = ::VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0u,
        .depthClampEnable = VK_FALSE,
        .rasterizerDiscardEnable = VK_FALSE,
        .polygonMode = ::VK_POLYGON_MODE_FILL,
        .cullMode    = ::VK_CULL_MODE_BACK_BIT,
        .frontFace   = ::VK_FRONT_FACE_CLOCKWISE,
        .depthBiasEnable         = VK_FALSE,
        .depthBiasConstantFactor = 0.0f,
        .depthBiasClamp          = 0.0f,
        .depthBiasSlopeFactor    = 0.0f,
        .lineWidth = 1.0f,
    };

    // nothing to do here yet, but it'll be fun when we can
    ::VkPipelineMultisampleStateCreateInfo multisampling {
        .sType = ::VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0u,
        .rasterizationSamples  = ::VK_SAMPLE_COUNT_1_BIT,
        .sampleShadingEnable   = VK_FALSE,
        .minSampleShading      = 1.0f,
        .pSampleMask           = nullptr,
        .alphaToCoverageEnable = VK_FALSE,
        .alphaToOneEnable      = VK_FALSE,
    };

    ::VkPipelineDepthStencilStateCreateInfo depth_info {
        .sType = ::VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0u,
        .depthTestEnable = VK_TRUE,
        .depthWriteEnable = VK_TRUE,
        .depthCompareOp = ::VK_COMPARE_OP_LESS,
        .depthBoundsTestEnable = VK_FALSE,
        .stencilTestEnable = VK_FALSE,
        .front = {
            .failOp = ::VK_STENCIL_OP_KEEP,
            .passOp = ::VK_STENCIL_OP_KEEP,
            .depthFailOp = ::VK_STENCIL_OP_KEEP,
            .compareOp = ::VK_COMPARE_OP_NEVER,
            .compareMask = 0u,
            .writeMask = 0u,
            .reference = 0u,
        },
        .back = {
            .failOp = ::VK_STENCIL_OP_KEEP,
            .passOp = ::VK_STENCIL_OP_KEEP,
            .depthFailOp = ::VK_STENCIL_OP_KEEP,
            .compareOp = ::VK_COMPARE_OP_NEVER,
            .compareMask = 0u,
            .writeMask = 0u,
            .reference = 0u,
        },
        .minDepthBounds = 0.0f,
        .maxDepthBounds = 1.0f,
    };

    // likewise here - transparency is a luxury I cannot yet afford
    ::VkPipelineColorBlendAttachmentState blend_attachments[] = {{
        .blendEnable = VK_FALSE,

        .srcColorBlendFactor = ::VK_BLEND_FACTOR_ONE,
        .dstColorBlendFactor = ::VK_BLEND_FACTOR_ZERO,
        .colorBlendOp        = ::VK_BLEND_OP_ADD,

        .srcAlphaBlendFactor = ::VK_BLEND_FACTOR_ONE,
        .dstAlphaBlendFactor = ::VK_BLEND_FACTOR_ZERO,
        .alphaBlendOp        = ::VK_BLEND_OP_ADD,

        .colorWriteMask = ::VK_COLOR_COMPONENT_R_BIT |
                          ::VK_COLOR_COMPONENT_G_BIT |
                          ::VK_COLOR_COMPONENT_B_BIT |
                          ::VK_COLOR_COMPONENT_A_BIT
    }};

    ::VkPipelineColorBlendStateCreateInfo blend_info {
        .sType = ::VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0u,
        .logicOpEnable   = VK_FALSE,
        .logicOp         = ::VK_LOGIC_OP_AND,
        .attachmentCount = std::size(blend_attachments),
        .pAttachments    = blend_attachments,
        .blendConstants  = { 0.0f, 0.0f, 0.0f, 0.0f }
    };

    CONSOLE_TRACE("Blend attachment count: {}", blend_info.attachmentCount);

    ::VkGraphicsPipelineCreateInfo pipelines[] {{
        .sType      = ::VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
        .pNext      = nullptr,
        .flags      = 0u,
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
        .basePipelineHandle  = VK_NULL_HANDLE,
        .basePipelineIndex   = -1,
    }};

    ::VkResult result = ::vkCreateGraphicsPipelines(
        _instance.logical_device(),
        VK_NULL_HANDLE,
        static_cast<uint32_t>(std::size(pipelines)),
        pipelines,
        nullptr,
        &_pipeline
    );

    if(result != ::VK_SUCCESS) {
        CONSOLE_ERROR("Unable to create graphics pipelines.");
    }
    else {
        CONSOLE_TRACE(
            "Created {} graphics {}",
            std::size(pipelines),
            std::size(pipelines) == 1 ? "pipeline" : "pipelines"
        );
    }
}

// =============================================================================
void Pipeline::_init_depth_buffer(const Swapchain &swapchain) {
    if(_depth_buffer != nullptr) {
        delete _depth_buffer;
    }

    _depth_buffer = new DepthBuffer(_instance, swapchain);
    _depth_buffer->init_image(
        ::VK_IMAGE_TILING_OPTIMAL,
        ::VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
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

    if(_vert != nullptr) {
        ::vkDestroyShaderModule(_instance.logical_device(), _vert, nullptr);
    }
    if(_frag != nullptr) {
        ::vkDestroyShaderModule(_instance.logical_device(), _frag, nullptr);
    }
    if(_renderpass != nullptr) {
        ::vkDestroyRenderPass(_instance.logical_device(), _renderpass, nullptr);
    }
    if(_layout != nullptr) {
        ::vkDestroyPipelineLayout(_instance.logical_device(), _layout, nullptr);
    }
    if(_pipeline != nullptr) {
        ::vkDestroyPipeline(_instance.logical_device(), _pipeline, nullptr);
    }
}
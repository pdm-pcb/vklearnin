#include "common.hpp"
#include "Pipeline.hpp"

#include "Instance.hpp"
#include "Shader.hpp"
#include "Vertex.hpp"

// =============================================================================
void Pipeline::vertex_from_binary(const char *filepath) {
    _vert = Shader::module_from_binary(filepath, _device);

    _shader_stages.emplace_back(::VkPipelineShaderStageCreateInfo { });
    auto &stage = _shader_stages.back();

    stage.sType  = ::VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    stage.stage  = ::VK_SHADER_STAGE_VERTEX_BIT;
    stage.module = _vert;
    stage.pName  = "main";
}

// =============================================================================
void Pipeline::fragment_from_binary(const char *filepath) {
    _frag = Shader::module_from_binary(filepath, _device);

    _shader_stages.emplace_back(::VkPipelineShaderStageCreateInfo { });
    auto &stage = _shader_stages.back();

    stage.sType  = ::VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    stage.stage  = ::VK_SHADER_STAGE_FRAGMENT_BIT;
    stage.module = _frag;
    stage.pName  = "main";
}

// =============================================================================
void Pipeline::init_render_passes(const Swapchain &swapchain) {
    CONSOLE_INFO("");

    ::VkAttachmentReference attachment_refs[] {{
        // the zeroth attachment is the fragment shader's outColor layout
        // location
        .attachment = 0u,
        .layout     = ::VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
    }};

    // I am guessing this will all make more sense once I've given multipass
    // rendering a go
    ::VkSubpassDescription subpasses[] {{
        .flags = { },
        .pipelineBindPoint    = ::VK_PIPELINE_BIND_POINT_GRAPHICS,
        .inputAttachmentCount = 0u,
        .pInputAttachments    = nullptr,
        .colorAttachmentCount = 
            static_cast<uint32_t>(std::size(attachment_refs)),
        .pColorAttachments       = attachment_refs,
        .pResolveAttachments     = nullptr,
        .pDepthStencilAttachment = nullptr,
        .preserveAttachmentCount = 0u,
        .pPreserveAttachments    = 0u,
    }};

    // the sole attachment point for this render pass is the color buffer
    ::VkAttachmentDescription attachments[] {{
        .flags          = 0u,
        .format         = swapchain.color_format(),
        .samples        = ::VK_SAMPLE_COUNT_1_BIT,
        .loadOp         = ::VK_ATTACHMENT_LOAD_OP_CLEAR,
        .storeOp        = ::VK_ATTACHMENT_STORE_OP_STORE,
        .stencilLoadOp  = ::VK_ATTACHMENT_LOAD_OP_DONT_CARE,
        .stencilStoreOp = ::VK_ATTACHMENT_STORE_OP_DONT_CARE,
        .initialLayout  = ::VK_IMAGE_LAYOUT_UNDEFINED,
        .finalLayout    = ::VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
    }};

    // given the single render pass for this setup, specifying the following
    // dependency ensures the render pass doesn't begin until there's an image
    // available
    ::VkSubpassDependency dependencies[] {{
        .srcSubpass      = VK_SUBPASS_EXTERNAL,
        .dstSubpass      = 0u,
        .srcStageMask    = ::VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
        .dstStageMask    = ::VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
        .srcAccessMask   = 0u,
        .dstAccessMask   = ::VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
        .dependencyFlags = 0u
    }};

    ::VkRenderPassCreateInfo renderpass_info { };
    renderpass_info.sType = ::VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderpass_info.attachmentCount =
        static_cast<uint32_t>(std::size(attachments));
    renderpass_info.subpassCount = 
        static_cast<uint32_t>(std::size(subpasses));
    renderpass_info.pAttachments = attachments;
    renderpass_info.pSubpasses   = subpasses;
    renderpass_info.dependencyCount =
        static_cast<uint32_t>(std::size(dependencies));
    renderpass_info.pDependencies = dependencies;

    CONSOLE_TRACE(
        "Creating render pass with {} {} and {} {}",
        renderpass_info.attachmentCount,
        renderpass_info.attachmentCount == 1 ? "attachment" : "attachments",
        renderpass_info.subpassCount,
        renderpass_info.subpassCount == 1 ? "subpass" : "subpasses"
    );

    ::VkResult result = ::vkCreateRenderPass(
        _device,
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
        _device,
        &pipeline_layout_info,
        nullptr,
        &_pipeline_layout
    );

    if(result != ::VK_SUCCESS) {
        CONSOLE_ERROR("Unable to initialize pipeline layout.");
    }
}

void Pipeline::init_pipeline(const Swapchain &swapchain) {
    CONSOLE_INFO("");

    // these two will allow resizing of the viewport and/or scissor rectangle
    // without recreating the entire graphcis pipeline
    ::VkDynamicState dynamic_states[] = { 
        ::VK_DYNAMIC_STATE_VIEWPORT,
        ::VK_DYNAMIC_STATE_SCISSOR
    };

    CONSOLE_TRACE("Dynamic state count: {}", std::size(dynamic_states));

    ::VkPipelineDynamicStateCreateInfo dynamic_state_info { };
    dynamic_state_info.sType =
        ::VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamic_state_info.dynamicStateCount = 
        static_cast<uint32_t>(std::size(dynamic_states));
    dynamic_state_info.pDynamicStates = dynamic_states;

    // grab the data from the newly formulated Vertex class
    auto binding_desc   = Vertex::binding_desc();
    auto attribute_desc = Vertex::attribute_desc();

    // and hand them over as part of the pipeline's input state
    ::VkPipelineVertexInputStateCreateInfo vertex_info { };
    vertex_info.sType =
        ::VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertex_info.vertexBindingDescriptionCount = 1u;
    vertex_info.vertexAttributeDescriptionCount =
        static_cast<uint32_t>(attribute_desc.size());
    vertex_info.pVertexBindingDescriptions = &binding_desc;
    vertex_info.pVertexAttributeDescriptions = attribute_desc.data();

    // assemble a triangle list, like anything at this point
    ::VkPipelineInputAssemblyStateCreateInfo assembly_info { };
    assembly_info.sType =
        ::VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    assembly_info.topology = ::VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    assembly_info.primitiveRestartEnable = VK_FALSE;

    // viewports and scissor rectangles are all sized the same as the surface
    // at present
    update_dimensions(swapchain);

    ::VkPipelineViewportStateCreateInfo viewport_info { };
    viewport_info.sType =
        ::VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewport_info.viewportCount = 1u;
    viewport_info.scissorCount  = 1u;

    CONSOLE_TRACE("Viewport count: {}", viewport_info.viewportCount);
    CONSOLE_TRACE("Scissor count:  {}", viewport_info.scissorCount);

    // standard rasterization details: fill, clockwise, cull backfaces
    ::VkPipelineRasterizationStateCreateInfo rasterizer { };
    rasterizer.sType =
        ::VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.polygonMode = ::VK_POLYGON_MODE_FILL;
    rasterizer.frontFace   = ::VK_FRONT_FACE_CLOCKWISE;
    rasterizer.cullMode    = ::VK_CULL_MODE_BACK_BIT;
    rasterizer.depthClampEnable        = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.depthBiasEnable         = VK_FALSE;
    rasterizer.depthBiasClamp          = 0.0f;
    rasterizer.depthBiasConstantFactor = 0.0f;
    rasterizer.depthBiasSlopeFactor    = 0.0f;
    rasterizer.lineWidth = 1.0f;

    // nothing to do here yet, but it'll be fun when we can
    ::VkPipelineMultisampleStateCreateInfo multisampling { };
    multisampling.sType =
        ::VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable   = VK_FALSE;
    multisampling.rasterizationSamples  = ::VK_SAMPLE_COUNT_1_BIT;
    multisampling.minSampleShading      = 1.0f;
    multisampling.pSampleMask           = nullptr;
    multisampling.alphaToCoverageEnable = VK_FALSE;
    multisampling.alphaToOneEnable      = VK_FALSE;

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

    ::VkPipelineColorBlendStateCreateInfo blend_info { };
    blend_info.sType =
        ::VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    blend_info.logicOpEnable = VK_FALSE;
    blend_info.logicOp = ::VK_LOGIC_OP_AND;
    blend_info.attachmentCount = 
        static_cast<uint32_t>(std::size(blend_attachments));
    blend_info.pAttachments = blend_attachments;

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
        .pDepthStencilState  = nullptr,
        .pColorBlendState    = &blend_info,
        .pDynamicState       = &dynamic_state_info,
        .layout              = _pipeline_layout,
        .renderPass          = _renderpass,
        .subpass             = 0u,
        .basePipelineHandle  = VK_NULL_HANDLE,
        .basePipelineIndex   = -1,
    }};

    ::VkResult result = ::vkCreateGraphicsPipelines(
        _device,
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
Pipeline::Pipeline(const ::VkDevice &device) :
    _device     { device  },
    _vert       { nullptr },
    _frag       { nullptr },
    _viewport   { },
    _scissor    { },
    _renderpass      { nullptr },
    _pipeline_layout { nullptr },
    _pipeline        { nullptr }
{
    CONSOLE_INFO("");
}

Pipeline::~Pipeline() {
    CONSOLE_INFO("");

    if(_vert != nullptr) {
        ::vkDestroyShaderModule(_device, _vert, nullptr);
    }
    if(_frag != nullptr) {
        ::vkDestroyShaderModule(_device, _frag, nullptr);
    }
    if(_renderpass != nullptr) {
        ::vkDestroyRenderPass(_device, _renderpass, nullptr);
    }
    if(_pipeline_layout != nullptr) {
        ::vkDestroyPipelineLayout(_device, _pipeline_layout, nullptr);
    }
    if(_pipeline != nullptr) {
        ::vkDestroyPipeline(_device, _pipeline, nullptr);
    }
}
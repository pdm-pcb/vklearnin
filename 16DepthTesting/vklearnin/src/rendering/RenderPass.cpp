#include "vklearnin/vklearnin.hpp"
#include "vklearnin/rendering/RenderPass.hpp"

#include "vklearnin/engine/Swapchain.hpp"
#include "vklearnin/rendering/devices/PhysicalDevice.hpp"
#include "vklearnin/rendering/devices/LogicalDevice.hpp"

namespace vkl {

// =============================================================================
void RenderPass::create_framebuffers(const Swapchain &swapchain) {
    for(uint32_t image_index = 0;
        image_index < RenderConfig::swapchain_image_count;
        ++image_index)
    {
        _framebuffers[image_index].create(
            swapchain.extent(),
            swapchain.image_view(image_index),
            _depth_stencil.view,
            this->native()
        );
    }
}

// =============================================================================
void RenderPass::destroy_framebuffers() {
    for(auto &framebuffer : _framebuffers) {
        framebuffer.destroy();
    }
}

//==============================================================================
void RenderPass::create(const Swapchain &swapchain) {
    _default_attachments(swapchain);
    _default_subpasses();
    _default_subpass_dependencies();

    vk::RenderPassCreateInfo renderpass_info {
        .attachmentCount = static_cast<uint32_t>(_attachments.size()),
        .pAttachments    = _attachments.data(),
        .subpassCount    = static_cast<uint32_t>(_subpasses.size()),
        .pSubpasses      = _subpasses.data(),
        .dependencyCount = static_cast<uint32_t>(_subpass_dependencies.size()),
        .pDependencies   = _subpass_dependencies.data()
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
    _render_pass = result.value;

    create_framebuffers(swapchain);
}

//==============================================================================
void RenderPass::destroy() {
    destroy_framebuffers();
    ImageTools::destroy_image(_depth_stencil);
    LogicalDevice::native().destroy(_render_pass);
}

//==============================================================================
void RenderPass::_default_attachments(const Swapchain &swapchain) {
    _depth_stencil = ImageTools::create_image(
        {
            .width = swapchain.extent().width,
            .height = swapchain.extent().height,
            .depth = 1u
        },
        1u,
        _find_depth_stencil_format(),
        vk::ImageAspectFlagBits::eDepth,
        vk::ImageTiling::eOptimal,
        1u,
        vk::SampleCountFlagBits::e1,
        vk::ImageUsageFlagBits::eDepthStencilAttachment,
        vk::MemoryPropertyFlagBits::eDeviceLocal
    );

    _attachments.clear();
    _attachments = {
        {
            .format         = swapchain.surface_format(),
            .samples        = vk::SampleCountFlagBits::e1,
            .loadOp         = vk::AttachmentLoadOp::eClear,
            .storeOp        = vk::AttachmentStoreOp::eStore,
            .stencilLoadOp  = vk::AttachmentLoadOp::eDontCare,
            .stencilStoreOp = vk::AttachmentStoreOp::eDontCare,
            .initialLayout  = vk::ImageLayout::eUndefined,
            .finalLayout    = vk::ImageLayout::ePresentSrcKHR,
        },
        {
            .format         = _find_depth_stencil_format(),
            .samples        = vk::SampleCountFlagBits::e1,
            .loadOp         = vk::AttachmentLoadOp::eClear,
            .storeOp        = vk::AttachmentStoreOp::eDontCare,
            .stencilLoadOp  = vk::AttachmentLoadOp::eDontCare,
            .stencilStoreOp = vk::AttachmentStoreOp::eDontCare,
            .initialLayout  = vk::ImageLayout::eUndefined,
            .finalLayout    = vk::ImageLayout::eDepthStencilAttachmentOptimal,
        },
    };
}

//==============================================================================
void RenderPass::_default_subpasses() {
    _color_attachments.clear();
    _color_attachments = {{
        .attachment = 0u,
        .layout     = vk::ImageLayout::eColorAttachmentOptimal,
    }};

    _depth_attachment = {
        .attachment = 1u,
        .layout     = vk::ImageLayout::eDepthStencilAttachmentOptimal,
    };

    _subpasses.clear();
    _subpasses = {{
        .pipelineBindPoint    = vk::PipelineBindPoint::eGraphics,
        .inputAttachmentCount = 0u,
        .pInputAttachments    = nullptr,
        .colorAttachmentCount =
            static_cast<uint32_t>(_color_attachments.size()),
        .pColorAttachments    = _color_attachments.data(),
        .pResolveAttachments  = nullptr,
        .pDepthStencilAttachment = &_depth_attachment,
        .preserveAttachmentCount = 0u,
        .pPreserveAttachments    = 0u,
    }};
}

//==============================================================================
void RenderPass::_default_subpass_dependencies() {
    _subpass_dependencies.clear();
    _subpass_dependencies = {
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
        .srcSubpass      = VK_SUBPASS_EXTERNAL,
        .dstSubpass      = 0u,
        .srcStageMask    = (vk::PipelineStageFlagBits::eEarlyFragmentTests |
                            vk::PipelineStageFlagBits::eLateFragmentTests),
        .dstStageMask    = (vk::PipelineStageFlagBits::eEarlyFragmentTests |
                            vk::PipelineStageFlagBits::eLateFragmentTests),
        .srcAccessMask   = vk::AccessFlagBits::eNone,
        .dstAccessMask   = vk::AccessFlagBits::eDepthStencilAttachmentWrite,
        .dependencyFlags = vk::DependencyFlagBits::eByRegion
    }};
}

//==============================================================================
vk::Format RenderPass::_find_depth_stencil_format() {
    std::vector<vk::Format> depth_options {
        vk::Format::eD32Sfloat,
        vk::Format::eD32SfloatS8Uint,
        vk::Format::eD24UnormS8Uint
    };

    for(const auto &option : depth_options) {
        auto props = PhysicalDevice::native().getFormatProperties(option);
        if(props.optimalTilingFeatures &
           vk::FormatFeatureFlagBits::eDepthStencilAttachment)
        {
            CONSOLE_TRACE(
                "Using depth stencil format {}",
                to_string(option)
            );
            return option;            
        }
    }

    CONSOLE_CRITICAL("Unable to find suitable depth stencil format");
    return vk::Format::eUndefined;
}

//==============================================================================
RenderPass::RenderPass() :
    _render_pass { nullptr }
{
    _framebuffers.resize(RenderConfig::swapchain_image_count);
}

} // namespace vkl
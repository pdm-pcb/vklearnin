#include "vklearnin/vklearnin.hpp"
#include "vklearnin/rendering/RenderPass.hpp"

#include "vklearnin/engine/Swapchain.hpp"
#include "vklearnin/rendering/devices/PhysicalDevice.hpp"
#include "vklearnin/rendering/devices/LogicalDevice.hpp"

namespace vkl {

// =============================================================================
void RenderPass::create_framebuffers() {
    _init_color_buffer();
    _init_depth_buffer();

    for(uint32_t image_index = 0;
        image_index < RenderConfig::swapchain_image_count;
        ++image_index)
    {
        std::vector<vk::ImageView> attachments;
        
        if(RenderConfig::msaa > 1) {
            attachments = {
                _color_buffer.view,
                _depth_buffer.view,
                _swapchain.image_view(image_index)
            };
        }
        else {
            attachments = {
                _swapchain.image_view(image_index),
                _depth_buffer.view
            };
        }

        _framebuffers[image_index].create(
            _swapchain.extent(),
            attachments,
            this->native()
        );
    }
}

// =============================================================================
void RenderPass::destroy_framebuffers() {
    for(auto &framebuffer : _framebuffers) {
        framebuffer.destroy();
    }

    if(_color_buffer.image) {
        ImageTools::destroy_image(_color_buffer);
    }

    if(_depth_buffer.image) {
        ImageTools::destroy_image(_depth_buffer);
    }
}

//==============================================================================
void RenderPass::create() {
    _default_attachments();
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

    create_framebuffers();
}

//==============================================================================
void RenderPass::destroy() {
    destroy_framebuffers();
    LogicalDevice::native().destroy(_render_pass);
}

//==============================================================================
void RenderPass::_default_attachments() {
    _attachments.clear();

    if(RenderConfig::msaa > 1u) {
        _attachments = {
            {   // color buffer (msaa) attachment description
                .format         = _swapchain.surface_format(),
                .samples        = _sample_flags,
                .loadOp         = vk::AttachmentLoadOp::eClear,
                .storeOp        = vk::AttachmentStoreOp::eDontCare,
                .stencilLoadOp  = vk::AttachmentLoadOp::eDontCare,
                .stencilStoreOp = vk::AttachmentStoreOp::eDontCare,
                .initialLayout  = vk::ImageLayout::eUndefined,
                .finalLayout    = vk::ImageLayout::eColorAttachmentOptimal,
            },
            {   // depth buffer attachment description
                .format         = _find_depth_buffer_format(),
                .samples        = _sample_flags,
                .loadOp         = vk::AttachmentLoadOp::eClear,
                .storeOp        = vk::AttachmentStoreOp::eDontCare,
                .stencilLoadOp  = vk::AttachmentLoadOp::eDontCare,
                .stencilStoreOp = vk::AttachmentStoreOp::eDontCare,
                .initialLayout  = vk::ImageLayout::eUndefined,
                .finalLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal,
            },
            {   // final presentation attachment
                .format         = _swapchain.surface_format(),
                .samples        = vk::SampleCountFlagBits::e1,
                .loadOp         = vk::AttachmentLoadOp::eDontCare,
                .storeOp        = vk::AttachmentStoreOp::eStore,
                .stencilLoadOp  = vk::AttachmentLoadOp::eDontCare,
                .stencilStoreOp = vk::AttachmentStoreOp::eDontCare,
                .initialLayout  = vk::ImageLayout::eUndefined,
                .finalLayout    = vk::ImageLayout::ePresentSrcKHR,
            }
        };
    }
    else {
        _attachments = {
            {
                .format         = _swapchain.surface_format(),
                .samples        = vk::SampleCountFlagBits::e1,
                .loadOp         = vk::AttachmentLoadOp::eClear,
                .storeOp        = vk::AttachmentStoreOp::eStore,
                .stencilLoadOp  = vk::AttachmentLoadOp::eDontCare,
                .stencilStoreOp = vk::AttachmentStoreOp::eDontCare,
                .initialLayout  = vk::ImageLayout::eUndefined,
                .finalLayout    = vk::ImageLayout::ePresentSrcKHR,
            },
            {
                .format         = _find_depth_buffer_format(),
                .samples        = vk::SampleCountFlagBits::e1,
                .loadOp         = vk::AttachmentLoadOp::eClear,
                .storeOp        = vk::AttachmentStoreOp::eDontCare,
                .stencilLoadOp  = vk::AttachmentLoadOp::eDontCare,
                .stencilStoreOp = vk::AttachmentStoreOp::eDontCare,
                .initialLayout  = vk::ImageLayout::eUndefined,
                .finalLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal,
            },
        };
    }
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

    _resolve_attachments = {{
        .attachment = 2u,
        .layout = vk::ImageLayout::eColorAttachmentOptimal,
    }};

    _subpasses.clear();
    _subpasses = {{
        .pipelineBindPoint    = vk::PipelineBindPoint::eGraphics,
        .inputAttachmentCount = 0u,
        .pInputAttachments    = nullptr,
        .colorAttachmentCount =
            static_cast<uint32_t>(_color_attachments.size()),
        .pColorAttachments    = _color_attachments.data(),
        .pResolveAttachments = RenderConfig::msaa > 1u ?
                               _resolve_attachments.data() :
                               nullptr,
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
void RenderPass::_init_depth_buffer() {
    if(_depth_buffer.image) {
        ImageTools::destroy_image(_depth_buffer);
    }

    _depth_buffer = ImageTools::create_image(
        {
            .width = _swapchain.extent().width,
            .height = _swapchain.extent().height,
            .depth = 1u
        },
        1u,
        _find_depth_buffer_format(),
        vk::ImageAspectFlagBits::eDepth,
        vk::ImageTiling::eOptimal,
        1u,
        _sample_flags,
        vk::ImageUsageFlagBits::eDepthStencilAttachment,
        vk::MemoryPropertyFlagBits::eDeviceLocal,
        "depth_buffer"
    );
}

//==============================================================================
void RenderPass::_init_color_buffer() {
    if(_color_buffer.image) {
        ImageTools::destroy_image(_color_buffer);
    }

    auto[width, height] = _swapchain.extent();

    _color_buffer = ImageTools::create_image(
        { width, height, 1u },
        0u,
        _swapchain.surface_format(),
        vk::ImageAspectFlagBits::eColor,
        vk::ImageTiling::eOptimal,
        1u,
        _sample_flags,
        vk::ImageUsageFlagBits::eTransientAttachment |
        vk::ImageUsageFlagBits::eColorAttachment,
        vk::MemoryPropertyFlagBits::eDeviceLocal,
        "color_buffer"
    );
}

//==============================================================================
vk::Format RenderPass::_find_depth_buffer_format() {
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
RenderPass::RenderPass(const Swapchain &swapchain) :
    _sample_flags { vk::SampleCountFlagBits::e1 },
    _render_pass  { nullptr },
    _swapchain    { swapchain }
{
    _framebuffers.resize(RenderConfig::swapchain_image_count);

    switch(RenderConfig::msaa) {
        case 64u: _sample_flags = vk::SampleCountFlagBits::e64; break;
        case 32u: _sample_flags = vk::SampleCountFlagBits::e32; break;
        case 16u: _sample_flags = vk::SampleCountFlagBits::e16; break;
        case  8u: _sample_flags = vk::SampleCountFlagBits::e8;  break;
        case  4u: _sample_flags = vk::SampleCountFlagBits::e4;  break;
        case  2u: _sample_flags = vk::SampleCountFlagBits::e2;  break;
        case  1u: break;
        default:
            CONSOLE_WARN(
                "Unsupported MSAA sample count {}, defaulting to 1x",
                RenderConfig::msaa
            );
            break;
    }
}

} // namespace vkl
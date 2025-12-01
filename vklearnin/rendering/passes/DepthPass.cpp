#include "vklearnin/vklearnin.hpp"
#include "vklearnin/rendering/passes/DepthPass.hpp"

#include "vklearnin/vulkan/swapchain/vkSurface.hpp"
#include "vklearnin/vulkan/devices/vkDevice.hpp"

namespace vkl {

// =============================================================================
bool DepthPass::create(vkSurface const &surface,
                       std::span<vk::ClearValue const> const clear_values,
                       vk::Format const depth_format,
                       vkDevice const &device)
{
    if(_render_pass.native()) {
        Log::error("Depth pass {} already exists.", _render_pass.native());
        return false;
    }

    if(!surface.native()) {
        Log::error("Cannot create depth pass with invalid surface.");
        return false;
    }

    if(!device.native()) {
        Log::error("Cannot create depth pass with invalid device.");
        return false;
    }

    if(depth_format == vk::Format::eUndefined) {
        Log::error("Cannot create depth pass with undefined depth format.");
        return false;
    }

    _color_format = surface.format().format;
    _depth_format = depth_format;

    _init_attachments();
    _init_subpasses();

    if(!_render_pass.create(_attachment_descriptions,
                            _subpass_descriptions,
                            _subpass_deps,
                            device))
    {
        Log::error("Failed to create depth pass.");
        _reset_object();
        return false;
    }

    if(!_create_depth_buffer(surface, device)) {
        Log::error("Failed to create depth pass depth buffer.");
        _reset_object();
        return false;
    }

    _begin_info = vk::RenderPassBeginInfo {
        .pNext = nullptr,
        .renderPass = _render_pass.native(),
        .framebuffer = { },
        .renderArea = vk::Rect2D {
            .offset = vk::Offset2D { },
            .extent = surface.extent(),
        },
        .clearValueCount = static_cast<uint32_t>(clear_values.size()),
        .pClearValues = clear_values.data(),
    };

    return true;
}

// =============================================================================
bool DepthPass::destroy() {
    if(!_render_pass.native()) {
        Log::error("Create depth pass before calling destroy.");
        return false;
    }

    _reset_object();

    return true;
}

// =============================================================================
void DepthPass::update_render_area(vkSurface const &surface) {
    _begin_info.renderArea = vk::Rect2D {
        .offset = vk::Offset2D { },
        .extent = surface.extent(),
    };
}

// =============================================================================
void DepthPass::destroy_swapchain_resources() {
    _begin_info.renderArea = vk::Rect2D { };

    _color_format = vk::Format::eUndefined;
    _depth_format = vk::Format::eUndefined;

    _destroy_depth_buffer();
}

// =============================================================================
void DepthPass::create_swapchain_resources(
    vkSurface const &surface,
    vk::Format const depth_format,
    vkDevice const &device)
{
    _color_format = surface.format().format;
    _depth_format = depth_format;

    update_render_area(surface);

    _init_attachments();
    _init_subpasses();

    _create_depth_buffer(surface, device);
}

// =============================================================================
void DepthPass::_init_attachments() {
    _attachment_descriptions = {
        vk::AttachmentDescription {
            // color buffer attachment description
            .format         = _color_format,
            .samples        = vk::SampleCountFlagBits::e1,
            .loadOp         = vk::AttachmentLoadOp::eClear,
            .storeOp        = vk::AttachmentStoreOp::eStore,
            .stencilLoadOp  = vk::AttachmentLoadOp::eDontCare,
            .stencilStoreOp = vk::AttachmentStoreOp::eDontCare,
            .initialLayout  = vk::ImageLayout::eUndefined,
            .finalLayout    = vk::ImageLayout::ePresentSrcKHR,
        },
        vk::AttachmentDescription {
            // depth buffer attachment description
            .format         = _depth_format,
            .samples        = vk::SampleCountFlagBits::e1,
            .loadOp         = vk::AttachmentLoadOp::eClear,
            .storeOp        = vk::AttachmentStoreOp::eDontCare,
            .stencilLoadOp  = vk::AttachmentLoadOp::eDontCare,
            .stencilStoreOp = vk::AttachmentStoreOp::eDontCare,
            .initialLayout  = vk::ImageLayout::eUndefined,
            .finalLayout    = vk::ImageLayout::eDepthStencilAttachmentOptimal,
        }
    };

    _color_refs = {{ vk::AttachmentReference {
        .attachment = 0u,
        .layout = vk::ImageLayout::eColorAttachmentOptimal,
    }}};

    _depth_ref = vk::AttachmentReference {
        .attachment = 1u,
        .layout = vk::ImageLayout::eDepthStencilAttachmentOptimal,
    };
}

// =============================================================================
void DepthPass::_init_subpasses() {
    _subpass_descriptions = {{ vk::SubpassDescription {
        // This subpass is a graphical one
        .pipelineBindPoint = vk::PipelineBindPoint::eGraphics,

        // ...Which has no input of any kind
        .inputAttachmentCount = 0u,
        .pInputAttachments    = nullptr,

        // But does have a single color attachment
        .colorAttachmentCount = static_cast<uint32_t>(_color_refs.size()),
        .pColorAttachments    = _color_refs.data(),

        // With no MSAA samples
        .pResolveAttachments = nullptr,

        // With a depth stencil
        .pDepthStencilAttachment = &_depth_ref,

        // As we've only got a single subpass, there's nothing to preserve
        // between subpasses
        .preserveAttachmentCount = 0u,
        .pPreserveAttachments    = nullptr,
    }}};

    _subpass_deps = {{ vk::SubpassDependency {
        .srcSubpass = VK_SUBPASS_EXTERNAL,
        .dstSubpass = 0u,

        .srcStageMask  = vk::PipelineStageFlagBits::eColorAttachmentOutput
                         | vk::PipelineStageFlagBits::eLateFragmentTests,

        .dstStageMask  = vk::PipelineStageFlagBits::eColorAttachmentOutput
                         | vk::PipelineStageFlagBits::eEarlyFragmentTests,

        .srcAccessMask = vk::AccessFlagBits::eDepthStencilAttachmentWrite,

        .dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite
                         | vk::AccessFlagBits::eDepthStencilAttachmentWrite,

        .dependencyFlags = { },
    }}};


}

// =============================================================================
bool DepthPass::_create_depth_buffer(vkSurface const &surface,
                                     vkDevice const &device)
{
    vkImage::Details const details {
        .type         = vk::ImageType::e2D,
        .samples      = vk::SampleCountFlagBits::e1,
        .usage_flags  = vk::ImageUsageFlagBits::eDepthStencilAttachment,
        .memory_flags = vk::MemoryPropertyFlagBits::eDeviceLocal,
    };

    if(!_depth_buffer.create(surface.extent(),
                             _depth_format,
                             details,
                             device))
    {
        Log::error("Failed to create depth buffer.");
        return false;
    }

    if(!_depth_view.create(vkImageView::Details {
                               .image        = _depth_buffer.native(),
                               .format       = _depth_buffer.format(),
                               .type         = vk::ImageViewType::e2D,
                               .aspect_flags = vk::ImageAspectFlagBits::eDepth,
                           },
                           device))
    {
        Log::error("Failed to create depth view.");
        _depth_buffer.destroy();
        return false;
    }

    return true;
}

// =============================================================================
void DepthPass::_destroy_depth_buffer() {
    if(_depth_view.native()) {
        _depth_view.destroy();
    }

    if(_depth_buffer.native()) {
        _depth_buffer.destroy();
    }
}

// =============================================================================
void DepthPass::_reset_object() {
    if(_render_pass.native()) {
        _render_pass.destroy();
    }

    _begin_info = vk::RenderPassBeginInfo { };
    _attachment_descriptions.clear();
    _color_refs.clear();
    _depth_ref = vk::AttachmentReference { };

    _subpass_descriptions.clear();
    _subpass_deps.clear();

    _color_format = vk::Format::eUndefined;
    _depth_format = vk::Format::eUndefined;

    _destroy_depth_buffer();
}

} // namespace vkl
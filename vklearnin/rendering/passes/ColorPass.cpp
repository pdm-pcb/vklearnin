#include "vklearnin/vklearnin.hpp"
#include "vklearnin/rendering/passes/ColorPass.hpp"

#include "vklearnin/vulkan/swapchain/vkSurface.hpp"
#include "vklearnin/vulkan/devices/vkDevice.hpp"

namespace vkl {

// =============================================================================
bool ColorPass::create(vkSurface const &surface,
                       std::span<vk::ClearValue const> const clear_values,
                       vkDevice const &device)
{
    if(_render_pass.native()) {
        Log::error(
            "Color pass {} already exists.",
            _render_pass.native()
        );
        return false;
    }

    if(!surface.native()) {
        Log::error("Cannot create color pass with invalid surface.");
        return false;
    }

    if(!device.native()) {
        Log::error("Cannot create color pass with invalid device.");
        return false;
    }

    _attachment_descriptions = {{ vk::AttachmentDescription {
        .flags          = { },
        .format         = surface.format().format,
        .samples        = vk::SampleCountFlagBits::e1,
        .loadOp         = vk::AttachmentLoadOp::eClear,
        .storeOp        = vk::AttachmentStoreOp::eStore,
        .stencilLoadOp  = vk::AttachmentLoadOp::eDontCare,
        .stencilStoreOp = vk::AttachmentStoreOp::eDontCare,
        .initialLayout  = vk::ImageLayout::eUndefined,
        .finalLayout    = vk::ImageLayout::ePresentSrcKHR,
    }}};

    _color_refs = {{ vk::AttachmentReference {
        .attachment = 0u,
        .layout     = vk::ImageLayout::eColorAttachmentOptimal,
    }}};

    _subpass_descriptions = {{ vk::SubpassDescription {
        .flags                   = { },
        .pipelineBindPoint       = vk::PipelineBindPoint::eGraphics,
        .inputAttachmentCount    = 0u,
        .pInputAttachments       = nullptr,
        .colorAttachmentCount    = static_cast<uint32_t>(_color_refs.size()),
        .pColorAttachments       = _color_refs.data(),
        .pResolveAttachments     = nullptr,
        .pDepthStencilAttachment = nullptr,
        .preserveAttachmentCount = 0u,
        .pPreserveAttachments    = nullptr,
    }}};

    _subpass_deps = {{ vk::SubpassDependency {
        .srcSubpass      = VK_SUBPASS_EXTERNAL,
        .dstSubpass      = 0u,
        .srcStageMask    = vk::PipelineStageFlagBits::eColorAttachmentOutput,
        .dstStageMask    = vk::PipelineStageFlagBits::eColorAttachmentOutput,
        .srcAccessMask   = { },
        .dstAccessMask   = vk::AccessFlagBits::eColorAttachmentWrite,
        .dependencyFlags = { },
    }}};

    if(!_render_pass.create(
        _attachment_descriptions,
        _subpass_descriptions,
        _subpass_deps,
        device
    ))
    {
        Log::error("Failed to create color pass.");

        _attachment_descriptions.clear();
        _color_refs.clear();
        _subpass_descriptions.clear();
        _subpass_deps.clear();

        return false;
    }

    _begin_info = vk::RenderPassBeginInfo {
        .pNext = nullptr,
        .renderPass = _render_pass.native(),
        .framebuffer = { },
        .renderArea = vk::Rect2D {
            .offset = { },
            .extent = surface.extent()
        },
        .clearValueCount = static_cast<uint32_t>(clear_values.size()),
        .pClearValues = clear_values.data(),
    };

    return true;
}

// =============================================================================
bool ColorPass::destroy() {
    if(!_render_pass.native()) {
        Log::error("Create color pass before calling destroy.");
        return false;
    }

    _begin_info = vk::RenderPassBeginInfo { };
    _render_pass.destroy();

    _attachment_descriptions.clear();
    _color_refs.clear();
    _subpass_descriptions.clear();
    _subpass_deps.clear();

    return true;
}

// =============================================================================
void ColorPass::update_render_area(vkSurface const &surface) {
    _begin_info.renderArea = vk::Rect2D {
        .offset = vk::Offset2D { },
        .extent = surface.extent(),
    };
}

} // namespace vkl
#include "vklearnin/vklearnin.hpp"
#include "vklearnin/render_passes/ColorPass.hpp"

#include "vklearnin/vulkan/swapchain/vkSurface.hpp"
#include "vklearnin/vulkan/devices/vkDevice.hpp"
#include "vklearnin/vulkan/swapchain/vkFrameBuffer.hpp"
#include "vklearnin/vulkan/devices/vkCmdBuffer.hpp"

namespace vkl {

// =============================================================================
bool ColorPass::create(vkSurface const &surface, vkDevice const &device) {
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

    _attachment_descriptions.emplace_back(vk::AttachmentDescription {
        .flags          = { },
        .format         = surface.format().format,
        .samples        = vk::SampleCountFlagBits::e1,
        .loadOp         = vk::AttachmentLoadOp::eClear,
        .storeOp        = vk::AttachmentStoreOp::eStore,
        .stencilLoadOp  = vk::AttachmentLoadOp::eDontCare,
        .stencilStoreOp = vk::AttachmentStoreOp::eDontCare,
        .initialLayout  = vk::ImageLayout::eUndefined,
        .finalLayout    = vk::ImageLayout::ePresentSrcKHR,
    });

    _color_refs.emplace_back(vk::AttachmentReference {
        .attachment = static_cast<uint32_t>(_color_refs.size()),
        .layout     = vk::ImageLayout::eColorAttachmentOptimal,
    });

    _subpass_descs.emplace_back(vk::SubpassDescription {
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
    });

    _subpass_deps.emplace_back(vk::SubpassDependency {
        .srcSubpass      = VK_SUBPASS_EXTERNAL,
        .dstSubpass      = 0u,
        .srcStageMask    = vk::PipelineStageFlagBits::eColorAttachmentOutput,
        .dstStageMask    = vk::PipelineStageFlagBits::eColorAttachmentOutput,
        .srcAccessMask   = { },
        .dstAccessMask   = vk::AccessFlagBits::eColorAttachmentWrite,
        .dependencyFlags = { },
    });

    if(!_render_pass.create(
        _attachment_descriptions,
        _subpass_descs,
        _subpass_deps,
        device
    ))
    {
        Log::error("Failed to create color pass.");

        _attachment_descriptions.clear();
        _color_refs.clear();
        _subpass_descs.clear();
        _subpass_deps.clear();

        return false;
    }

    _render_area = vk::Rect2D {
        .offset = { },
        .extent = surface.extent()
    };

    return true;
}

// =============================================================================
bool ColorPass::destroy() {
    if(!_render_pass.native()) {
        Log::error("Create color pass before calling destroy.");
        return false;
    }

    _render_pass.destroy();

    _attachment_descriptions.clear();
    _color_refs.clear();
    _subpass_descs.clear();
    _subpass_deps.clear();

    _render_area = vk::Rect2D { };

    return true;
}

// =============================================================================
void ColorPass::update_render_area(vkSurface const &surface) {
    _render_area = vk::Rect2D {
        .offset = { },
        .extent = surface.extent()
    };
}

// =============================================================================
void ColorPass::begin(vkFrameBuffer const &frame_buffer,
                      std::span<vk::ClearValue const> const clear_values,
                      vkCmdBuffer const &cmd_buffer)
{
    auto const begin_info = vk::RenderPassBeginInfo {
        .pNext = nullptr,
        .renderPass = _render_pass.native(),
        .framebuffer = frame_buffer.native(),
        .renderArea = _render_area,
        .clearValueCount = static_cast<uint32_t>(clear_values.size()),
        .pClearValues = clear_values.data(),
    };

    cmd_buffer.native().beginRenderPass(begin_info,
                                        vk::SubpassContents::eInline);
}

} // namespace vkl
#include "vklearnin/vklearnin.hpp"
#include "vklearnin/vulkan/pipelines/vkRenderPass.hpp"

#include "vklearnin/vulkan/devices/vkDevice.hpp"

namespace vkl {

// =============================================================================
bool vkRenderPass::create(Attachments const attachments,
                          Subpasses const subpasses,
                          SubpassDeps const subpass_deps,
                          vkDevice const &device)
{
    if(_handle) {
        Log::error("Render pass {} already exists.", _handle);
        return false;
    }

    if(!device.native()) {
        Log::error("Cannot create render pass with invalid device.");
        return false;
    }

    _device = device.native();

    vk::RenderPassCreateInfo create_info {
        .pNext           = nullptr,
        .flags           = { },
        .attachmentCount = static_cast<uint32_t>(attachments.size()),
        .pAttachments    = attachments.data(),
        .subpassCount    = static_cast<uint32_t>(subpasses.size()),
        .pSubpasses      = subpasses.data(),
        .dependencyCount = static_cast<uint32_t>(subpass_deps.size()),
        .pDependencies   = subpass_deps.data(),
    };

    auto const [ result, value ] = _device.createRenderPass(create_info);
    if(result != vk::Result::eSuccess) {
        Log::error(
            "Failed to create render pass: '{}'",
            vk::to_string(result)
        );
        return false;
    }

    _handle = value;
    Log::trace("Created render pass {}", _handle);
    return true;
}

// =============================================================================
bool vkRenderPass::destroy() {
    if(!_handle) {
        Log::error("Must create render pass before calling destroy.");
        return false;
    }

    Log::trace("Destroying render pass {}.", _handle);
    _device.destroyRenderPass(_handle);

    _handle = nullptr;
    _device = nullptr;

    return true;
}

} // namespace vkl
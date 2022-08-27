#include "vklearnin/common.hpp"
#include "vklearnin/Buffers/Framebuffers.hpp"

#include "vklearnin/Instance.hpp"
#include "vklearnin/Swapchain.hpp"
#include "vklearnin/Pipeline.hpp"

// =============================================================================
void Framebuffers::init_buffers(const Swapchain &swapchain,
                                const Pipeline &pipeline)
{
    CONSOLE_INFO("");

    _buffers.resize(swapchain.image_views().size());

    // ensure the dimensions match
    auto [width, height] = swapchain.extent();

    // run through and associate one framebuffer per swapchain image
    for(size_t buffer_idx = 0; buffer_idx < _buffers.size(); ++buffer_idx) {
        ::VkImageView attachments[] = {
            swapchain.image_views()[buffer_idx],
            pipeline.depth_buffer().image_view()
        };

        ::VkFramebufferCreateInfo buffer_info {
            .sType = ::VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0u,
            .renderPass = pipeline.renderpass(),
            .attachmentCount = std::size(attachments),
            .pAttachments = attachments,
            .width = width,
            .height = height,
            .layers = 1u,
        };

        ::VkResult result = ::vkCreateFramebuffer(
            _device,
            &buffer_info,
            nullptr,
            &_buffers[buffer_idx]
        );

        if(result != ::VK_SUCCESS) {
            CONSOLE_ERROR("Failed to create framebuffer {}", buffer_idx);
        }
    }

    CONSOLE_TRACE(
        "Created {} {}",
        _buffers.size(),
        _buffers.size() == 1 ? "framebuffer" : "framebuffers"
    );
}

// =============================================================================
void Framebuffers::destroy() {
    CONSOLE_INFO("");

    for(auto buffer : _buffers) {
        ::vkDestroyFramebuffer(_device, buffer, nullptr);
    }
}

// =============================================================================
void Framebuffers::create(const Swapchain &swapchain, const Pipeline &pipeline)
{
    init_buffers(swapchain, pipeline);
}

// =============================================================================
Framebuffers::Framebuffers(const ::VkDevice &device) :
    _device { device }
{
    CONSOLE_INFO("");
}

Framebuffers::~Framebuffers() {
    CONSOLE_INFO("");

    destroy();
}
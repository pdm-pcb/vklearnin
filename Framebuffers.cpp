#include "common.hpp"
#include "Framebuffers.hpp"

#include "Instance.hpp"
#include "Swapchain.hpp"
#include "Pipeline.hpp"

// =============================================================================
void Framebuffers::init_buffers(const Swapchain &swapchain,
                                const Pipeline &pipeline)
{
    CONSOLE_INFO("");

    _buffers.resize(swapchain.image_views().size());

    // run through and associate one framebuffer per swapchain image
    for(size_t buffer_idx = 0; buffer_idx < _buffers.size(); ++buffer_idx) {
        ::VkImageView attachments[] = {
            swapchain.image_views()[buffer_idx]
        };

        ::VkFramebufferCreateInfo buffer_info { };
        buffer_info.sType = ::VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        buffer_info.renderPass      = pipeline.renderpass();
        buffer_info.attachmentCount = 1u;
        buffer_info.pAttachments    = attachments;

        // ensure the dimensions match
        auto [width, height] = swapchain.extent();
        buffer_info.width    = width;
        buffer_info.height   = height;

        buffer_info.layers = 1u;

        ::VkResult result = ::vkCreateFramebuffer(
            _device,
            &buffer_info,
            nullptr,
            &_buffers[buffer_idx]
        );

        if(result != ::VK_SUCCESS) {
            CONSOLE_CRITICAL("Failed to create framebuffer {}", buffer_idx);
        }
    }

    CONSOLE_TRACE(
        "Created {} {}",
        _buffers.size(),
        _buffers.size() == 1 ? "framebuffer" : "framebuffers"
    );
}

void Framebuffers::destroy() {
    CONSOLE_INFO("");

    for(auto buffer : _buffers) {
        ::vkDestroyFramebuffer(_device, buffer, nullptr);
    }
}

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
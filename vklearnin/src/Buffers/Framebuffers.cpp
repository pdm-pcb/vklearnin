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
        _attachments.clear();

        if(pipeline.sample_count() > 1u) {
            _attachments.emplace_back(pipeline.color_buffer());
            _attachments.emplace_back(pipeline.depth_buffer());
            _attachments.emplace_back(swapchain.image_views()[buffer_idx]);
        }
        else {
            _attachments.emplace_back(swapchain.image_views()[buffer_idx]);
            _attachments.emplace_back(pipeline.depth_buffer());
        }

        vk::FramebufferCreateInfo buffer_info {
            .renderPass = pipeline.renderpass(),
            .attachmentCount = static_cast<uint32_t>(_attachments.size()),
            .pAttachments = _attachments.data(),
            .width = width,
            .height = height,
            .layers = 1u,
        };

        _buffers[buffer_idx] = _device.createFramebuffer(buffer_info);
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
        _device.destroy(buffer);
    }
}

// =============================================================================
void Framebuffers::create(const Swapchain &swapchain, const Pipeline &pipeline)
{
    CONSOLE_INFO("");

    init_buffers(swapchain, pipeline);
}

// =============================================================================
Framebuffers::Framebuffers(const vk::Device &device) :
    _device { device }
{
    CONSOLE_INFO("");

    _attachments.reserve(10);
}

Framebuffers::~Framebuffers() {
    CONSOLE_INFO("");

    destroy();
}
#include "Demo.hpp"

// =============================================================================
void Demo::run_renderpasses(const vkl::CmdBuffer &command_buffer,
                            const uint32_t frame_index)
{
    const auto &cmd_buffer = command_buffer.native();

    // The flags in this structure can be used to mark a command buffer as
    // single-use, among other things. In this case, we'll be using a vanilla
    // command buffer.
    const vk::CommandBufferBeginInfo begin_info { };

    // Calling begin tells the command buffer to start recording commands
    auto result = cmd_buffer.begin(begin_info);
    if(result != vk::Result::eSuccess) {
        CONSOLE_ERROR(
            "Failed to begin command buffer recording: '{}'",
            to_string(result)
        );
    }

    // The clear color specifies what value to fill the swapchain image pixels
    // with when we nuke that area of memory before beginning to draw
    static const vk::ClearValue clear_values[] = {
        { .color { vkl::RenderConfig::CLEAR_COLOR }}
    };

    const vk::RenderPassBeginInfo pass_info {
        .renderPass      = _render_pass.native(),
        .framebuffer     = _framebuffers[frame_index].native(),
        .renderArea      = vkl::Swapchain::render_area(),
        .clearValueCount = static_cast<uint32_t>(std::size(clear_values)),
        .pClearValues    = clear_values,
    };

    // The command buffer is ready to record commands, and now it knows we
    // mean to record graphics commands in particular. Marking the subpass
    // contents as inline means we're not executing from a secondary command
    // buffer.
    cmd_buffer.beginRenderPass(pass_info, vk::SubpassContents::eInline);

    // ...??? Something graphical will live here, soon.

    // We've issued all of the graphics commands we want, so wrap this render
    // pass up
    cmd_buffer.endRenderPass();

    // And put a bow on the whole thing
    result = cmd_buffer.end();
    if(result != vk::Result::eSuccess) {
        CONSOLE_ERROR(
            "Failed to end command buffer recording: '{}'",
            to_string(result)
        );
    }

    // Here's some commands, swapchain. See to it that they make their way to
    // the GPU
    vkl::Swapchain::submit({ cmd_buffer });
}

// =============================================================================
void Demo::init() {
     _render_pass.create();

    for(uint32_t frame = 0; frame < _framebuffers.size(); ++frame) {
        _framebuffers[frame].create(
            { vkl::Swapchain::image(frame)->view() },
            _render_pass
        );
    }
}

// =============================================================================
void Demo::shutdown() {
    for(auto &framebuffer : _framebuffers) {
        framebuffer.destroy();
    }

    _render_pass.destroy();
}

// =============================================================================
Demo::Demo() :
    _render_pass { }
{
    _framebuffers.resize(vkl::RenderConfig::image_count);
}

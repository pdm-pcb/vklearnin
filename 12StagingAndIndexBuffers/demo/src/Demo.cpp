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

        // The first step in executing any pipeline is to bind it
        cmd_buffer.bindPipeline(
            vk::PipelineBindPoint::eGraphics,
            _pipeline.native()
        );

            // As the above pipeline has a dynamic viewport and scissor state,
            // these need to be specified after binding.
            cmd_buffer.setViewport(0u, _pipeline.viewport());
            cmd_buffer.setScissor(0u, _pipeline.scissor());

            const std::vector<vk::Buffer> vertex_buffers {
                _xy_plane.vertex_buffer().native()
            };
            const std::vector<vk::DeviceSize> offsets {
                0u
            };

            cmd_buffer.bindVertexBuffers(
                0u,
                vertex_buffers,
                offsets
            );

            cmd_buffer.bindIndexBuffer(
                _xy_plane.index_buffer().native(),
                0u,
                vkl::INDEX_TYPE
            );

            cmd_buffer.drawIndexed(
                static_cast<uint32_t>(_xy_plane.index_count()),
                1u,
                0u,
                0u,
                0u
            );

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

    // Don't forget to load the new shaders we just wrote
    _pipeline.vert_from_spirv("shaders/02flat_color.vert");
    _pipeline.frag_from_spirv("shaders/02flat_color.frag");

    // The pipeline now expects us to describe the vertex input data we'll
    // provide
    _pipeline.describe_vertex_input(
        vkl::Vertex::binding_desc(),
        vkl::Vertex::attrib_desc()
    );

    _pipeline.create(_render_pass);

    // Initalize the XYPlane object with a suitable scale and color values
    _xy_plane.init(
        // Use a scale of 1.0f if you want the plane to fill the entire
        // rendering surface. Using 0.5f allows us to still see the clear color
        // and validate that the plane is shaped and positioned like we expect.
        0.5f,
        {{
            { 1.0f, 0.0f, 0.0f, 1.0f }, // Red
            { 0.0f, 1.0f, 0.0f, 1.0f }, // Green
            { 0.0f, 0.0f, 1.0f, 1.0f }, // Blue
            { 1.0f, 1.0f, 1.0f, 1.0f }, // White
        }}
    );
}

// =============================================================================
void Demo::shutdown() {
    _xy_plane.shutdown();

    _pipeline.destroy();

    for(auto &framebuffer : _framebuffers) {
        framebuffer.destroy();
    }

    _render_pass.destroy();
}

// =============================================================================
Demo::Demo() :
    _render_pass { },
    _pipeline    { }
{
    _framebuffers.resize(vkl::RenderConfig::image_count);
}

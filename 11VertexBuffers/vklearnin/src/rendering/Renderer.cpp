#include "vklearnin/vklearnin.hpp"
#include "vklearnin/rendering/Renderer.hpp"

#include "vklearnin/rendering/swapchain/Swapchain.hpp"
#include "vklearnin/resources/images/Image2D.hpp"
#include "vklearnin/meshes/Mesh.hpp"

namespace vkl {

static constexpr vk::ClearValue clear_values[] = {
    { .color { vkl::RenderConfig::CLEAR_COLOR }}
};

// =============================================================================
void Renderer::submit(const DrawSubmission &draw) {
    _draws.push_back(draw);
}

// =============================================================================
void Renderer::render_pass(const vk::CommandBuffer &cmd_buffer) {
    const vk::RenderPassBeginInfo pass_info {
        .renderPass      = _render_pass.native(),
        .framebuffer     = _framebuffers[Swapchain::image_index()].native(),
        .renderArea      = vkl::Swapchain::render_area(),
        .clearValueCount = static_cast<uint32_t>(std::size(clear_values)),
        .pClearValues    = clear_values,
    };
    cmd_buffer.beginRenderPass(pass_info, vk::SubpassContents::eInline);

        cmd_buffer.bindPipeline(
            vk::PipelineBindPoint::eGraphics,
            _pipeline.native()
        );
        cmd_buffer.setViewport(0u, _pipeline.viewport());
        cmd_buffer.setScissor(0u,  _pipeline.scissor());

        for(const auto &draw : _draws) {
            const auto &mesh = draw.mesh;
            cmd_buffer.bindVertexBuffers(
                0u,
                { mesh.vertex_buffer().native() },
                { 0u }
            );
            cmd_buffer.draw(
                static_cast<uint32_t>(mesh.vertex_count()),
                1u, 0u, 0u
            );
        }

    cmd_buffer.endRenderPass();

    _draws.clear();
}

// =============================================================================
void Renderer::init() {
    _render_pass.create();

    _init_framebuffers();
    _init_pipeline();
}

// =============================================================================
void Renderer::shutdown() {
    _pipeline.destroy();

    for(auto &framebuffer : _framebuffers) {
        framebuffer.destroy();
    }

    _render_pass.destroy();
}

// =============================================================================
void Renderer::_init_framebuffers() {
    for(uint32_t frame = 0; frame < _framebuffers.size(); ++frame) {
        _framebuffers[frame].create(
            { vkl::Swapchain::image(frame)->view() },
            _render_pass
        );
    }
}

// =============================================================================
void Renderer::_init_pipeline() {
    _pipeline.vert_from_spirv("shaders/02flat_color.vert");
    _pipeline.frag_from_spirv("shaders/02flat_color.frag");

    _pipeline.describe_vertex_input(
        vkl::Vertex::binding_desc(),
        vkl::Vertex::attrib_desc()
    );

    _pipeline.create(_render_pass);
}

// =============================================================================
Renderer::Renderer() :
_render_pass { },
_pipeline    { },
_draws       { }
{
    _framebuffers.resize(vkl::RenderConfig::image_count);
}

} // namespace vkl
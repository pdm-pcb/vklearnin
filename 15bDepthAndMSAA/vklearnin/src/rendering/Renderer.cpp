#include "vklearnin/vklearnin.hpp"
#include "vklearnin/rendering/Renderer.hpp"

#include "vklearnin/rendering/swapchain/Swapchain.hpp"
#include "vklearnin/resources/images/Image2D.hpp"
#include "vklearnin/meshes/Mesh.hpp"

namespace vkl {

static constexpr vk::ClearValue clear_values[] = {
    { .color { vkl::RenderConfig::CLEAR_COLOR }},
    { .depthStencil = 1.0f }
};

// =============================================================================
void Renderer::update_view_proj(const ViewProjMats &matrices) {
    for(uint32_t set = 0u; set < RenderConfig::image_count; ++set) {
        _desc_sets[set].update_ubo(_ubo_iters[set], &matrices);
    }
}

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

        cmd_buffer.bindDescriptorSets(
            vk::PipelineBindPoint::eGraphics,
            _pipeline.layout(),
            0u,
            { _desc_sets[Swapchain::image_index()].native() },
            nullptr
        );

        for(const auto &draw : _draws) {
            uint32_t running_offset = 0u;
            for(const auto &push_constant : draw.push_constants) {
                cmd_buffer.pushConstants(
                    _pipeline.layout(),
                    push_constant.stage_flags,
                    running_offset,
                    static_cast<uint32_t>(push_constant.size),
                    push_constant.data
                );

                running_offset += push_constant.size;
            }

            const auto &mesh = draw.mesh;
            cmd_buffer.bindVertexBuffers(
                0u,
                { mesh.vertex_buffer().native() },
                { 0u }
            );
            cmd_buffer.bindIndexBuffer(
                mesh.index_buffer().native(),
                0u,
                INDEX_TYPE
            );
            cmd_buffer.drawIndexed(
                static_cast<uint32_t>(mesh.index_count()),
                1u, 0u, 0u, 0u
            );
        }

    cmd_buffer.endRenderPass();

    _draws.clear();
}

// =============================================================================
void Renderer::init() {
    _render_pass.create();

    _init_framebuffers();
    _init_descriptors();
    _init_pipeline();
}

// =============================================================================
void Renderer::shutdown() {
    _pipeline.destroy();

    for(auto &set : _desc_sets) {
        set.destroy();
    }

    _desc_layout.destroy();
    _desc_pool.destroy();

    for(auto &framebuffer : _framebuffers) {
        framebuffer.destroy();
    }

    _render_pass.destroy();
}

// =============================================================================
void Renderer::_init_framebuffers() {
    for(uint32_t frame = 0; frame < _framebuffers.size(); ++frame) {
        _framebuffers[frame].create(
            {
                Swapchain::image(frame).view,
                _render_pass.depth_buffer_view()
            },
            _render_pass.native()
        );
    }
}

// =============================================================================
void Renderer::_init_descriptors() {
    _desc_pool.create(
        10u,
        {{
            .type = vk::DescriptorType::eUniformBuffer,
            .descriptorCount = 100u,
        }}
    );

    _desc_layout.add_binding({
        .binding = static_cast<uint32_t>(_desc_layout.bindings().size()),
        .descriptorType     = vk::DescriptorType::eUniformBuffer,
        .descriptorCount    = 1u,
        .stageFlags         = vk::ShaderStageFlagBits::eVertex,
        .pImmutableSamplers = nullptr
    });

    _desc_layout.create();

    for(uint32_t set_index = 0; set_index < _desc_sets.size(); ++set_index) {
        _ubo_iters[set_index] =
            _desc_sets[set_index].add_ubo(sizeof(ViewProjMats));
        _desc_sets[set_index].create(_desc_pool, _desc_layout);
    }
}

// =============================================================================
void Renderer::_init_pipeline() {
    _pipeline.vert_from_spirv("shaders/02flat_color.vert");
    _pipeline.frag_from_spirv("shaders/02flat_color.frag");

    _pipeline.add_descriptor_set(_desc_layout.native());

    _pipeline.add_push_constant(
        vk::ShaderStageFlagBits::eVertex,
        sizeof(Mat4)
    );

    _pipeline.describe_vertex_input(
        vkl::Vertex::binding_desc(),
        vkl::Vertex::attrib_desc()
    );

    _pipeline.create(_render_pass);
}

// =============================================================================
Renderer::Renderer() :
_desc_pool   { },
_render_pass { },
_pipeline    { },
_draws       { }
{
    _framebuffers.resize(vkl::RenderConfig::image_count);
    _desc_sets.resize(vkl::RenderConfig::image_count);
    _ubo_iters.resize(vkl::RenderConfig::image_count);
}

} // namespace vkl
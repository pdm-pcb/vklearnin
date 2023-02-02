#include "vklearnin/vklearnin.hpp"
#include "vklearnin/rendering/Renderer.hpp"

#include "vklearnin/rendering/swapchain/Swapchain.hpp"
#include "vklearnin/resources/buffers/BufferObject.hpp"
#include "vklearnin/resources/images/ImageObject.hpp"
#include "vklearnin/meshes/Mesh.hpp"

namespace vkl {

static constexpr vk::ClearValue clear_values[] = {
    { .color { vkl::RenderConfig::CLEAR_COLOR }},
    { .depthStencil { .depth = 1.0f }}
};

DescriptorPool         Renderer::_desc_pool    { };
Renderer::Pipelines    Renderer::_pipelines    { };
Renderer::Framebuffers Renderer::_framebuffers { vkl::RenderConfig::image_count };
vkl::RenderPass        Renderer::_render_pass  { };

// =============================================================================
void Renderer::submit(const PipelineIndex index, const DrawSubmission &draw) {
    _pipelines[index].draws.push_back(draw);
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

    for(auto& pipeline : _pipelines) {
        cmd_buffer.bindPipeline(
            vk::PipelineBindPoint::eGraphics,
            pipeline.pipeline.native()
        );
        cmd_buffer.setViewport(0u, pipeline.pipeline.viewport());
        cmd_buffer.setScissor(0u,  pipeline.pipeline.scissor());

        cmd_buffer.bindDescriptorSets(
            vk::PipelineBindPoint::eGraphics,
            pipeline.pipeline.layout(),
            0u,
            { pipeline.desc_set.native() },
            nullptr
        );

        for(auto const& draw : pipeline.draws) {
            size_t running_offset = 0u;
            for(auto const& push_constant : draw.push_constants) {
                cmd_buffer.pushConstants(
                    pipeline.pipeline.layout(),
                    push_constant.stage_flags,
                    static_cast<uint32_t>(running_offset),
                    static_cast<uint32_t>(push_constant.size),
                    push_constant.data
                );

                running_offset += push_constant.size;
            }

            cmd_buffer.bindVertexBuffers(
                0u,
                { draw.vertex_buffer },
                { 0u }
            );
            cmd_buffer.bindIndexBuffer(
                draw.index_buffer,
                0u,
                INDEX_TYPE
            );
            cmd_buffer.drawIndexed(
                static_cast<uint32_t>(draw.index_count),
                1u, 0u, 0u, 0u
            );
        }

        pipeline.draws.clear();
    }

    cmd_buffer.endRenderPass();
}

// =============================================================================
void Renderer::init() {
    _render_pass.create();
    _init_framebuffers();
    _init_descriptors();
}

// =============================================================================
void Renderer::shutdown() {
    for(auto& pipeline : _pipelines) {
        pipeline.desc_set.destroy();
        pipeline.pipeline.destroy();
    }

    _desc_pool.destroy();

    for(auto &framebuffer : _framebuffers) {
        framebuffer.destroy();
    }

    _render_pass.destroy();
}

// =============================================================================
void Renderer::add_ubo(const PipelineIndex index,
                       const DescriptorSet::BufferObjects &buffers,
                       const vk::ShaderStageFlags stage_flags)
{
    _pipelines[index].desc_set.add_ubo(buffers, stage_flags);
}

// =============================================================================
void Renderer::add_texture2D(const PipelineIndex index,
                             const ImageObject &texture)
{
    _pipelines[index].desc_set.add_texture2D(texture);
}

// =============================================================================
void Renderer::create_pipelines() {
    // {
    //     // Color Pipeline
    //     auto& pipe = _pipelines[PipelineIndex::COLOR];
    //     pipe.pipeline.vert_from_spirv("shaders/01color.vert");
    //     pipe.pipeline.frag_from_spirv("shaders/01color.frag");

    //     pipe.desc_layout.create();
    //     for(auto& set : pipe.desc_sets) {
    //         set.create(_desc_pool, pipe.desc_layout);
    //     }

    //     pipe.pipeline.add_descriptor_set(pipe.desc_layout.native());

    //     pipe.pipeline.add_push_constant(
    //         vk::ShaderStageFlagBits::eVertex,
    //         sizeof(Mat4)
    //     );
    //     pipe.pipeline.describe_vertex_input(
    //         VertexColor::bindings,
    //         VertexColor::attributes
    //     );

    //     pipe.pipeline.create(_render_pass);
    // }
    {
        // Texture Pipeline
        auto& pipe = _pipelines[PipelineIndex::TEXTURE];
        pipe.pipeline.vert_from_spirv("shaders/02texture.vert");
        pipe.pipeline.frag_from_spirv("shaders/02texture.frag");

        pipe.desc_set.create(_desc_pool);
        pipe.pipeline.add_descriptor_set(pipe.desc_set.layout().native());

        pipe.pipeline.add_push_constant(
            vk::ShaderStageFlagBits::eVertex,
            sizeof(Mat4)
        );
        pipe.pipeline.describe_vertex_input(
            VertexTexture::bindings,
            VertexTexture::attributes
        );

        pipe.pipeline.create(_render_pass);
    }
}

// =============================================================================
void Renderer::_init_framebuffers() {
    for(uint32_t frame = 0; frame < _framebuffers.size(); ++frame) {
        _framebuffers[frame].create(
            {
                _render_pass.color_buffer_view(),
                _render_pass.depth_buffer_view(),
                Swapchain::image(frame).view
            },
            _render_pass.native()
        );
    }
}

// =============================================================================
void Renderer::_init_descriptors() {
    _desc_pool.create(
        vkl::RenderConfig::image_count * PipelineIndex::MAX,
        {{
            .type = vk::DescriptorType::eUniformBuffer,
            .descriptorCount = 10u,
        },
        {
            .type = vk::DescriptorType::eCombinedImageSampler,
            .descriptorCount = 10u,
        }}
    );
}

} // namespace vkl
#include "vklearnin/vklearnin.hpp"
#include "vklearnin/rendering/Renderer.hpp"

#include "vklearnin/rendering/swapchain/Swapchain.hpp"
#include "vklearnin/resources/buffers/BufferObject.hpp"
#include "vklearnin/resources/images/ImageObject.hpp"
#include "vklearnin/meshes/Mesh.hpp"

namespace vkl {

static constexpr vk::ClearValue clear_values[] = {
    { .color { vkl::RenderConfig::CLEAR_COLOR }},
    { .depthStencil {
            .depth = 1.0f,
            .stencil = 1u,
        }
    }
};

const enum DescBindSlot {
    GLOBAL_UNIFORM,
    PER_TEXTURE,
    PER_DRAW
};

    DescriptorPool Renderer::_desc_pool { };

DescriptorSetLayout Renderer::_global_uniform_set_layout { };
DescriptorSetLayout Renderer::_flat_texture_set_layout   { };
DescriptorSetLayout Renderer::_skybox_set_layout         { };

Renderer::DescriptorSets Renderer::_global_uniform_sets {
    RenderConfig::image_count
};

Renderer::DescriptorSets Renderer::_flat_texture_sets { };
DescriptorSet Renderer::_skybox_set { };

RenderPass Renderer::_render_pass { };

std::vector<Framebuffer> Renderer::_framebuffers { RenderConfig::image_count };

Pipeline Renderer::_flat_color_pipeline;
Pipeline Renderer::_flat_texture_pipeline;
Pipeline Renderer::_skybox_pipeline;

Renderer::ColorDraws         Renderer::_flat_color_draws;
Renderer::FlatTextureDraws   Renderer::_flat_texture_draws;
DrawSubmission<VertexSkybox> Renderer::_skybox_draw;

// =============================================================================
void Renderer::render_pass(vk::CommandBuffer const &cmd_buffer) {
    vk::RenderPassBeginInfo const pass_info {
        .renderPass      = _render_pass.native(),
        .framebuffer     = _framebuffers[Swapchain::image_index()].native(),
        .renderArea      = vkl::Swapchain::render_area(),
        .clearValueCount = static_cast<uint32_t>(std::size(clear_values)),
        .pClearValues    = clear_values,
    };
    cmd_buffer.beginRenderPass(pass_info, vk::SubpassContents::eInline);

        _execute_flat_color_pipeline(cmd_buffer);
        _execute_flat_texture_pipeline(cmd_buffer);
        _execute_skybox_pipeline(cmd_buffer);

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
    _flat_color_pipeline.destroy();
    _flat_texture_pipeline.destroy();
    _skybox_pipeline.destroy();

    for(auto &set : _global_uniform_sets) {
        set.destroy();
    }

    _global_uniform_set_layout.destroy();
    _flat_texture_set_layout.destroy();
    _skybox_set_layout.destroy();

    _desc_pool.destroy();

    for(auto &framebuffer : _framebuffers) {
        framebuffer.destroy();
    }

    _render_pass.destroy();
}

// =============================================================================
void Renderer::set_global_uniforms(std::vector<BufferObject> const &ubos) {
    if(ubos.size() != RenderConfig::image_count) {
        CONSOLE_CRITICAL(
            "UBO vector size of {} does not match image count of {}",
            ubos.size(),
            RenderConfig::image_count
        );
    }

    _global_uniform_set_layout.add_binding({
        .binding            = 0u,
        .descriptorType     = vk::DescriptorType::eUniformBuffer,
        .descriptorCount    = 1u,
        .stageFlags         = vk::ShaderStageFlagBits::eVertex,
        .pImmutableSamplers = nullptr
    });

    for(uint32_t image = 0u; image < RenderConfig::image_count; ++image) {
        _global_uniform_sets[image].add_ubo(ubos[image]);
    }
}

// =============================================================================
void Renderer::add_flat_texture(Texture2D const &texture) {
    _flat_texture_sets.resize(_flat_texture_sets.size() + 1);
    _flat_texture_sets.back().add_texture2D(texture.image());

    _flat_texture_draws.insert({
        reinterpret_cast<uint64_t>(VkImage(texture.image().handle)),
        FlatTextureDrawQueue {
            .set_index = _flat_texture_sets.size() - 1,
            .queue = { }
        }
    });
}

// =============================================================================
void Renderer::set_skybox_texture(Texture2D const &texture) {
    _skybox_set.add_texture2D(texture.image());
}

// =============================================================================
void Renderer::create_pipelines() {
    _global_uniform_set_layout.create();
    for(auto &set : _global_uniform_sets) {
        set.create(_desc_pool, _global_uniform_set_layout);
    }

    _init_flat_color_pipeline();
    _init_flat_texture_pipeline();
    _init_skybox_pipeline();
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
        100u,
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

// =============================================================================
void Renderer::_init_flat_color_pipeline() {
    _flat_color_pipeline.vert_from_spirv("shaders/01color.vert");
    _flat_color_pipeline.frag_from_spirv("shaders/01color.frag");

    _flat_color_pipeline.describe_vertex_input(
        VertexFlatColor::bindings,
        VertexFlatColor::attributes
    );

    _flat_color_pipeline.add_descriptor_set(
        _global_uniform_set_layout.native()
    );

    _flat_color_pipeline.add_push_constant(
        vk::ShaderStageFlagBits::eVertex,
        sizeof(Mat4)
    );

    _flat_color_pipeline.create(_render_pass);
}

// =============================================================================
void Renderer::_init_flat_texture_pipeline() {
    _flat_texture_set_layout.add_binding({
        .binding            = 0u,
        .descriptorType     = vk::DescriptorType::eCombinedImageSampler,
        .descriptorCount    = 1u,
        .stageFlags         = vk::ShaderStageFlagBits::eFragment,
        .pImmutableSamplers = nullptr
    });

    _flat_texture_set_layout.create();
    for(auto &set : _flat_texture_sets) {
        set.create(_desc_pool, _flat_texture_set_layout);
    }

    _flat_texture_pipeline.vert_from_spirv("shaders/02texture.vert");
    _flat_texture_pipeline.frag_from_spirv("shaders/02texture.frag");

    _flat_texture_pipeline.describe_vertex_input(
        VertexFlatTexture::bindings,
        VertexFlatTexture::attributes
    );

    _flat_texture_pipeline.add_descriptor_set(
        _global_uniform_set_layout.native()
    );

    _flat_texture_pipeline.add_push_constant(
        vk::ShaderStageFlagBits::eVertex,
        sizeof(Mat4)
    );

    _flat_texture_pipeline.add_descriptor_set(
        _flat_texture_set_layout.native()
    );

    _flat_texture_pipeline.create(_render_pass);
}

// =============================================================================
void Renderer::_init_skybox_pipeline() {
    _skybox_set_layout.add_binding({
        .binding            = 0u,
        .descriptorType     = vk::DescriptorType::eCombinedImageSampler,
        .descriptorCount    = 1u,
        .stageFlags         = vk::ShaderStageFlagBits::eFragment,
        .pImmutableSamplers = nullptr
    });

    _skybox_set_layout.create();
    _skybox_set.create(_desc_pool, _skybox_set_layout);

    _skybox_pipeline.vert_from_spirv("shaders/03skybox.vert");
    _skybox_pipeline.frag_from_spirv("shaders/03skybox.frag");

    _skybox_pipeline.describe_vertex_input(
        VertexSkybox::bindings,
        VertexSkybox::attributes
    );

    _skybox_pipeline.add_descriptor_set(_global_uniform_set_layout.native());
    _skybox_pipeline.add_descriptor_set(_skybox_set_layout.native());

    _skybox_pipeline.create(_render_pass);
}

// =============================================================================
void
Renderer::_execute_flat_color_pipeline(vk::CommandBuffer const &cmd_buffer) {
    auto const &pipeline = _flat_color_pipeline;
    cmd_buffer.bindPipeline(
        vk::PipelineBindPoint::eGraphics,
        pipeline.native()
    );
    cmd_buffer.setViewport(0u, pipeline.viewport());
    cmd_buffer.setScissor(0u,  pipeline.scissor());

    _bind_global_uniforms(pipeline, cmd_buffer);

    for(auto const &draw : _flat_color_draws) {
        _send_push_constants(pipeline, draw, cmd_buffer);

        cmd_buffer.bindVertexBuffers(
            0u,
            { draw.mesh->vertex_buffer().buffer().handle },
            { 0u }
        );
        cmd_buffer.bindIndexBuffer(
            draw.mesh->index_buffer().buffer().handle,
            0u,
            INDEX_TYPE
        );
        cmd_buffer.drawIndexed(
            static_cast<uint32_t>(draw.mesh->index_count()),
            1u, 0u, 0u, 0u
        );
    }

    _flat_color_draws.clear();
}

// =============================================================================
void
Renderer::_execute_flat_texture_pipeline(vk::CommandBuffer const &cmd_buffer) {
    auto const &pipeline = _flat_texture_pipeline;
    cmd_buffer.bindPipeline(
        vk::PipelineBindPoint::eGraphics,
        pipeline.native()
    );
    cmd_buffer.setViewport(0u, pipeline.viewport());
    cmd_buffer.setScissor(0u,  pipeline.scissor());

    _bind_global_uniforms(pipeline, cmd_buffer);

    for(auto &[texture_id, draw_queue] : _flat_texture_draws) {
        cmd_buffer.bindDescriptorSets(
            vk::PipelineBindPoint::eGraphics,
            pipeline.layout(),
            DescBindSlot::PER_TEXTURE,
            { _flat_texture_sets[draw_queue.set_index].native() },
            nullptr
        );

        for(auto const &draw : draw_queue.queue) {
            _send_push_constants(pipeline, draw, cmd_buffer);

            cmd_buffer.bindVertexBuffers(
                0u,
                { draw.mesh->vertex_buffer().buffer().handle },
                { 0u }
            );
            cmd_buffer.bindIndexBuffer(
                draw.mesh->index_buffer().buffer().handle,
                0u,
                INDEX_TYPE
            );
            cmd_buffer.drawIndexed(
                static_cast<uint32_t>(draw.mesh->index_count()),
                1u, 0u, 0u, 0u
            );
        }

        draw_queue.queue.clear();
    }
}

// =============================================================================
void Renderer::_execute_skybox_pipeline(vk::CommandBuffer const &cmd_buffer) {
    auto const &pipeline = _skybox_pipeline;
    cmd_buffer.bindPipeline(
        vk::PipelineBindPoint::eGraphics,
        pipeline.native()
    );
    cmd_buffer.setViewport(0u, pipeline.viewport());
    cmd_buffer.setScissor(0u,  pipeline.scissor());

    _bind_global_uniforms(pipeline, cmd_buffer);

    cmd_buffer.bindDescriptorSets(
        vk::PipelineBindPoint::eGraphics,
        pipeline.layout(),
        DescBindSlot::PER_TEXTURE,
        { _skybox_set.native() },
        nullptr
    );

    cmd_buffer.bindVertexBuffers(
        0u,
        { _skybox_draw.mesh->vertex_buffer().buffer().handle },
        { 0u }
    );
    cmd_buffer.bindIndexBuffer(
        _skybox_draw.mesh->index_buffer().buffer().handle,
        0u,
        INDEX_TYPE
    );
    cmd_buffer.drawIndexed(
        static_cast<uint32_t>(_skybox_draw.mesh->index_count()),
        1u, 0u, 0u, 0u
    );
}

// =============================================================================
void Renderer::_bind_global_uniforms(Pipeline const &pipeline,
                                     vk::CommandBuffer const &cmd_buffer)
{
    cmd_buffer.bindDescriptorSets(
        vk::PipelineBindPoint::eGraphics,
        pipeline.layout(),
        DescBindSlot::GLOBAL_UNIFORM,
        { _global_uniform_sets[Swapchain::image_index()].native() },
        nullptr
    );
}

} // namespace vkl
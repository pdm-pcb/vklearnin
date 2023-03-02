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

DescriptorPool Renderer::_desc_pool { };

DescriptorSetLayout Renderer::_global_uniform_set_layout { };
DescriptorSetLayout Renderer::_material_set_layout       { };
DescriptorSetLayout Renderer::_draw_set_layout           { };

Renderer::DescriptorSets Renderer::_global_uniform_sets {
    RenderConfig::image_count
};

Renderer::DescriptorSets Renderer::_material_sets   { };
Renderer::DescriptorSets Renderer::_draw_sets       { };
DescriptorSet            Renderer::_skybox_desc_set { };

RenderPass Renderer::_render_pass { };
std::array<Pipeline, Renderer::PipelineType::MAX> Renderer::_pipelines { };

std::vector<Framebuffer> Renderer::_framebuffers { RenderConfig::image_count };

std::vector<DrawSubmission> Renderer::_color_draws;
Renderer::TextureDraws      Renderer::_texture_draws;
DrawSubmission const        Renderer::_skybox_draw { };

// =============================================================================
void Renderer::submit(PipelineType const pipeline, DrawSubmission const &draw) {
    switch(pipeline) {
        case FLAT_COLOR: _color_draws.push_back(draw); break;
        case FLAT_TEXTURE: {
            auto mat_index = reinterpret_cast<uint64_t>(
                VkImage(draw.material.image().handle)
            );
            _texture_draws.at(mat_index).queue.push_back(draw);
            break;
        }
        // case SKYBOX:
        //     _skybox_draw = draw;
        //     break;
        default:
            CONSOLE_CRITICAL("Submitting draw to unknown pipeline type");
    }
}

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

        _bind_global_uniforms(*_pipelines.begin(), cmd_buffer);

        _execute_flat_color_pipeline(cmd_buffer);
        _execute_flat_texture_pipeline(cmd_buffer);
        // _execute_skybox_pipeline(cmd_buffer);

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
        pipeline.destroy();
    }

    for(auto &set : _global_uniform_sets) {
        set.destroy();
    }

    _global_uniform_set_layout.destroy();
    _material_set_layout.destroy();

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
void Renderer::add_material(ImageObject const &material) {
    _material_sets.resize(_material_sets.size() + 1);
    _material_sets.back().add_texture2D(material);

    _texture_draws.insert({
        reinterpret_cast<uint64_t>(VkImage(material.handle)),
        MaterialDrawQueue {
            .set_index = _material_sets.size() - 1,
            .queue = { }
        }
    });
}

// =============================================================================
void Renderer::create_pipelines() {
    _global_uniform_set_layout.create();
    for(auto &set : _global_uniform_sets) {
        set.create(_desc_pool, _global_uniform_set_layout);
    }

    _material_set_layout.add_binding({
        .binding            = 0u,
        .descriptorType     = vk::DescriptorType::eCombinedImageSampler,
        .descriptorCount    = 1u,
        .stageFlags         = vk::ShaderStageFlagBits::eFragment,
        .pImmutableSamplers = nullptr
    });

    _material_set_layout.create();
    for(auto &set : _material_sets) {
        set.create(_desc_pool, _material_set_layout);
    }

    _init_flat_color_pipeline();
    _init_flat_texture_pipeline();
    // _init_skybox_pipeline();

    for(auto& pipeline : _pipelines) {
        pipeline.add_descriptor_set(_global_uniform_set_layout.native());
        pipeline.add_push_constant(
            vk::ShaderStageFlagBits::eVertex,
            sizeof(Mat4)
        );
    }

    auto &flat_texture_pipeline = _pipelines[PipelineType::FLAT_TEXTURE];
    flat_texture_pipeline.add_descriptor_set(_material_set_layout.native());

    for(auto& pipeline : _pipelines) {
        pipeline.create(_render_pass);
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
    auto &pipeline = _pipelines[PipelineType::FLAT_COLOR];
    pipeline.vert_from_spirv("shaders/01color.vert");
    pipeline.frag_from_spirv("shaders/01color.frag");

    pipeline.describe_vertex_input(
        VertexFlatColor::bindings,
        VertexFlatColor::attributes
    );
}

// =============================================================================
void Renderer::_init_flat_texture_pipeline() {
    auto &pipeline = _pipelines[PipelineType::FLAT_TEXTURE];
    pipeline.vert_from_spirv("shaders/02texture.vert");
    pipeline.frag_from_spirv("shaders/02texture.frag");

    pipeline.describe_vertex_input(
        VertexFlatTexture::bindings,
        VertexFlatTexture::attributes
    );
}

// =============================================================================
void Renderer::_init_skybox_pipeline() {
    // auto &pipeline = _pipelines[PipelineType::SKYBOX];
    // pipeline.vert_from_spirv("shaders/03skybox.vert");
    // pipeline.frag_from_spirv("shaders/03skybox.frag");

    // pipeline.describe_vertex_input(
    //     VertexFlatTexture::bindings,
    //     VertexFlatTexture::attributes
    // );
}

// =============================================================================
void
Renderer::_execute_flat_color_pipeline(vk::CommandBuffer const &cmd_buffer) {
    auto const &pipeline = _pipelines[PipelineType::FLAT_COLOR];
    cmd_buffer.bindPipeline(
        vk::PipelineBindPoint::eGraphics,
        pipeline.native()
    );
    cmd_buffer.setViewport(0u, pipeline.viewport());
    cmd_buffer.setScissor(0u,  pipeline.scissor());

    for(auto const &draw : _color_draws) {
        _send_push_constants(pipeline, draw, cmd_buffer);

        cmd_buffer.bindVertexBuffers(
            0u,
            { draw.vertex_buffer.handle },
            { 0u }
        );
        cmd_buffer.bindIndexBuffer(
            draw.index_buffer.handle,
            0u,
            INDEX_TYPE
        );
        cmd_buffer.drawIndexed(
            static_cast<uint32_t>(draw.index_count),
            1u, 0u, 0u, 0u
        );
    }

    _color_draws.clear();
}

// =============================================================================
void
Renderer::_execute_flat_texture_pipeline(vk::CommandBuffer const &cmd_buffer) {
    auto const &pipeline = _pipelines[PipelineType::FLAT_TEXTURE];
    cmd_buffer.bindPipeline(
        vk::PipelineBindPoint::eGraphics,
        pipeline.native()
    );
    cmd_buffer.setViewport(0u, pipeline.viewport());
    cmd_buffer.setScissor(0u,  pipeline.scissor());

    for(auto &[material_id, draw_queue] : _texture_draws) {
        cmd_buffer.bindDescriptorSets(
            vk::PipelineBindPoint::eGraphics,
            pipeline.layout(),
            DescBindFreq::PER_MATERIAL,
            { _material_sets[draw_queue.set_index].native() },
            nullptr
        );

        for(auto const &draw : draw_queue.queue) {
            _send_push_constants(pipeline, draw, cmd_buffer);

            cmd_buffer.bindVertexBuffers(
                0u,
                { draw.vertex_buffer.handle },
                { 0u }
            );
            cmd_buffer.bindIndexBuffer(
                draw.index_buffer.handle,
                0u,
                INDEX_TYPE
            );
            cmd_buffer.drawIndexed(
                static_cast<uint32_t>(draw.index_count),
                1u, 0u, 0u, 0u
            );
        }

        draw_queue.queue.clear();
    }
}

// =============================================================================
void Renderer::_execute_skybox_pipeline(vk::CommandBuffer const &cmd_buffer) {
    // auto const &pipeline = _pipelines[PipelineType::SKYBOX];

    // cmd_buffer.bindPipeline(
    //     vk::PipelineBindPoint::eGraphics,
    //     pipeline.native()
    // );
    // cmd_buffer.setViewport(0u, pipeline.viewport());
    // cmd_buffer.setScissor(0u,  pipeline.scissor());

    // cmd_buffer.bindDescriptorSets(
    //     vk::PipelineBindPoint::eGraphics,
    //     pipeline.layout(),
    //     DescBindFreq::GLOBAL_UNIFORM,
    //     { _global_uniform_sets[Swapchain::image_index()].native() },
    //     nullptr
    // );

    // cmd_buffer.bindDescriptorSets(
    //     vk::PipelineBindPoint::eGraphics,
    //     pipeline.layout(),
    //     DescBindFreq::PER_MATERIAL,
    //     { _skybox_desc_set.native() },
    //     nullptr
    // );

    // _send_push_constants(pipeline, _skybox_draw, cmd_buffer);

    // cmd_buffer.bindVertexBuffers(
    //     0u,
    //     { _skybox_draw.vertex_buffer.handle },
    //     { 0u }
    // );
    // cmd_buffer.bindIndexBuffer(
    //     _skybox_draw.index_buffer.handle,
    //     0u,
    //     INDEX_TYPE
    // );
    // cmd_buffer.drawIndexed(
    //     static_cast<uint32_t>(_skybox_draw.index_count),
    //     1u, 0u, 0u, 0u
    // );
}

// =============================================================================
void Renderer::_bind_global_uniforms(Pipeline const &pipeline,
                                     vk::CommandBuffer const &cmd_buffer)
{
    cmd_buffer.bindDescriptorSets(
        vk::PipelineBindPoint::eGraphics,
        pipeline.layout(),
        DescBindFreq::GLOBAL_UNIFORM,
        { _global_uniform_sets[Swapchain::image_index()].native() },
        nullptr
    );
}

// =============================================================================
void Renderer::_send_push_constants(Pipeline const &pipeline,
                                    DrawSubmission const &draw,
                                    vk::CommandBuffer const &cmd_buffer)
{
    size_t running_offset = 0u;
    for(auto const& push_constant : draw.push_constants) {
        cmd_buffer.pushConstants(
            pipeline.layout(),
            push_constant.stage_flags,
            static_cast<uint32_t>(running_offset),
            static_cast<uint32_t>(push_constant.size),
            push_constant.data
        );

        running_offset += push_constant.size;
    }
}

} // namespace vkl
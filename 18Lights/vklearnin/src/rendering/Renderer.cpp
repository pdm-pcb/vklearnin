#include "vklearnin/vklearnin.hpp"
#include "vklearnin/rendering/Renderer.hpp"

#include "vklearnin/system/devices/LogicalDevice.hpp"
#include "vklearnin/rendering/swapchain/Swapchain.hpp"
#include "vklearnin/rendering/FrameData.hpp"
#include "vklearnin/resources/buffers/BufferObject.hpp"
#include "vklearnin/resources/images/ImageObject.hpp"
#include "vklearnin/meshes/Mesh.hpp"

namespace vkl {

static vk::ClearValue const clear_values[] = {
    { .color { vkl::RenderConfig::CLEAR_COLOR }},
    { .depthStencil
        {
            .depth = 1.0f,
            .stencil = 1u,
        }
    }
};

RenderPass               Renderer::_render_pass;
std::vector<Framebuffer> Renderer::_framebuffers;
std::vector<FrameData>   Renderer::_frame_data;
uint32_t                 Renderer::_frame_index = 0u;
uint64_t                 Renderer::_frame_count = 0u;

// Descriptors -----------------------------------------------------------------
DescriptorPool Renderer::_desc_pool;

DescriptorSetLayout Renderer::_global_buffer_layout;
Renderer::PerFrameSets Renderer::_global_buffer_sets;

// Shader Resources ------------------------------------------------------------
Renderer::PerFrameBuffers Renderer::_global_buffers;

Pipeline Renderer::_flat_color_pipeline;

Renderer::FlatColorDraws Renderer::_flat_color_draws;

// =============================================================================
void Renderer::update_global_buffer(GlobalBuffer const &buffer) {
    BufferTools::update_buffer(_global_buffers[_frame_index], &buffer);
}

// =============================================================================
void Renderer::render_pass() {
    _frame_data[_frame_index].wait_on_queue_fence();

    auto &cmd_buffer = _frame_data[_frame_index].command_buffer().native();
    vk::CommandBufferBeginInfo const begin_info;
    auto const begin_result = cmd_buffer.begin(&begin_info);
    if(begin_result != vk::Result::eSuccess) {
        CONSOLE_CRITICAL(
            "Unable to begin recording to command buffer: '{}'",
            to_string(begin_result)
        );
        return;
    }

    vk::RenderPassBeginInfo const pass_info {
        .renderPass      = _render_pass.native(),
        .framebuffer     = _framebuffers[_frame_index].native(),
        .renderArea      = vkl::Swapchain::render_area(),
        .clearValueCount = static_cast<uint32_t>(std::size(clear_values)),
        .pClearValues    = clear_values,
    };
    cmd_buffer.beginRenderPass(pass_info, vk::SubpassContents::eInline);

        _execute_flat_color_pipeline(cmd_buffer);
        // _execute_flat_texture_pipeline(cmd_buffer);
        // _execute_skybox_pipeline(cmd_buffer);
        // _execute_lit_color_pipeline(cmd_buffer);

    cmd_buffer.endRenderPass();
    cmd_buffer.end();

    Swapchain::submit_and_present(_frame_data[_frame_index]);

    _frame_count += 1;
    _frame_index = _frame_count % RenderConfig::swapchain_image_count;
}

// =============================================================================
void Renderer::init() {
    _render_pass.create();
    _init_framebuffers();
    _init_frame_data();
    _init_descriptor_pool();
}

// =============================================================================
void Renderer::shutdown() {
    _flat_color_pipeline.destroy();
    // _lit_color_pipeline.destroy();
    // _flat_texture_pipeline.destroy();
    // _skybox_pipeline.destroy();

    _global_buffer_layout.destroy();
    // _lit_color_set_layout.destroy();
    // _flat_texture_set_layout.destroy();
    // _skybox_set_layout.destroy();

    _desc_pool.destroy();

    _shutdown_shader_resources();

    for(auto &framebuffer : _framebuffers) {
        framebuffer.destroy();
    }

    for(auto &frame : _frame_data) {
        frame.shutdown();
    }

    _render_pass.destroy();
}

// // =============================================================================
// void Renderer::add_flat_texture(Texture2D const &texture) {
//     _flat_texture_sets.resize(_flat_texture_sets.size() + 1);
//     _flat_texture_sets.back().add_texture2D(texture.image());

//     _flat_texture_draws.insert({
//         reinterpret_cast<uint64_t>(VkImage(texture.image().handle)),
//         FlatTextureDrawQueue {
//             .set_index = _flat_texture_sets.size() - 1,
//             .queue = { }
//         }
//     });
// }

// // =============================================================================
// void Renderer::set_skybox_texture(Texture2D const &texture) {
//     _skybox_set.add_texture2D(texture.image());
// }

// // =============================================================================
// void Renderer::set_light_ubos(std::vector<BufferObject> const &ubos) {
//     if(ubos.size() != RenderConfig::swapchain_image_count) {
//         CONSOLE_CRITICAL(
//             "UBO vector must match concurrent frame count {}",
//             ubos.size(),
//             RenderConfig::swapchain_image_count
//         );
//     }

//     for(uint32_t frame = 0u; frame < RenderConfig::swapchain_image_count; ++frame) {
//         _lit_color_sets[frame].resize(_lit_color_sets[frame].size() + 1);
//         _lit_color_sets[frame].back().add_ubo(ubos[frame]);
//     }
// }

// =============================================================================
void Renderer::create_pipelines() {
    _init_descriptor_sets();
    _init_shader_resources();

    _init_flat_color_pipeline();
    // _init_flat_texture_pipeline();
    // _init_skybox_pipeline();
    // _init_lit_color_pipeline();
}

// =============================================================================
void Renderer::_init_framebuffers() {
    _framebuffers.reserve(RenderConfig::swapchain_image_count);

    for(auto const &swapchain_image : Swapchain::images()) {
        _framebuffers.push_back({ });
        _framebuffers.back().create(
            {
                _render_pass.color_buffer_view(),
                _render_pass.depth_buffer_view(),
                swapchain_image.view
            },
            _render_pass.native()
        );
    }
}

// =============================================================================
void Renderer::_init_frame_data() {
    _frame_data.resize(RenderConfig::swapchain_image_count);
    for(auto &frame : _frame_data) {
        frame.init();
    }
}

// =============================================================================
void Renderer::_init_descriptor_pool() {
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
void Renderer::_init_descriptor_sets() {
    _global_buffer_layout
        .add_binding(
            vk::DescriptorType::eUniformBuffer,
            vk::ShaderStageFlagBits::eAll
        )
        .create();

    _global_buffer_sets.resize(RenderConfig::swapchain_image_count);
    for(auto &set : _global_buffer_sets) {
        set.create(_desc_pool, _global_buffer_layout);
    }

    // _flat_texture_set_layout.add_binding(
    //     vk::DescriptorType::eCombinedImageSampler,
    //     vk::ShaderStageFlagBits::eFragment
    // );
    // _flat_texture_set_layout.create();

    // for(auto &set : _flat_texture_sets) {
    //     set.create(_desc_pool, _flat_texture_set_layout);
    // }

    // _skybox_set_layout.add_binding(
    //     vk::DescriptorType::eCombinedImageSampler,
    //     vk::ShaderStageFlagBits::eFragment
    // );
    // _skybox_set_layout.create();
    // _skybox_set.create(_desc_pool, _skybox_set_layout);

    // _lit_color_set_layout.add_binding(
    //     vk::DescriptorType::eUniformBuffer,
    //     vk::ShaderStageFlagBits::eFragment
    // );
    // _lit_color_set_layout.create();

    // for(auto &frame : _lit_color_sets) {
    //     for(auto &set : frame) {
    //         set.create(_desc_pool, _lit_color_set_layout);
    //     }
    // }
}

// =============================================================================
void Renderer::_init_shader_resources() {
    _global_buffers.resize(RenderConfig::swapchain_image_count);
    for(auto &buffer : _global_buffers) {
        buffer.size = sizeof(GlobalBuffer);
        vkl::BufferTools::create(
            buffer,
            vk::BufferUsageFlagBits::eUniformBuffer,
            (vk::MemoryPropertyFlagBits::eHostVisible |
             vk::MemoryPropertyFlagBits::eHostCoherent)
        );
    }

    for(uint32_t frame = 0u;
        frame < RenderConfig::swapchain_image_count;
        ++frame)
    {
        _global_buffer_sets[frame]
            .add_buffer(_global_buffers[frame])
            .write_set();
    }
}

// =============================================================================
void Renderer::_shutdown_shader_resources() {
    for(auto &buffer : _global_buffers) {
        BufferTools::destroy(buffer);
    }
}

// =============================================================================
void Renderer::_init_flat_color_pipeline() {
    _flat_color_pipeline
        .vert_from_spirv("shaders/01color.vert")
        .frag_from_spirv("shaders/01color.frag")
        .describe_vertex_input(
            VertexFlatColor::bindings,
            VertexFlatColor::attributes
        )
        .add_descriptor_set(_global_buffer_layout.native())
        .add_push_constant(
            vk::ShaderStageFlagBits::eAll,
            sizeof(Mat4)
        )
        .create(
            _render_pass,
            Pipeline::Config {
                .polygon_mode  = vk::PolygonMode::eFill,
                .cull_mode     = vk::CullModeFlagBits::eBack,
                .front_face    = vk::FrontFace::eClockwise,
                .max_msaa_samples  = max_msaa_flag(),
                .subpass_index = 0u,
            }
        );
}

// // =============================================================================
// void Renderer::_init_flat_texture_pipeline() {
//     _flat_texture_pipeline.vert_from_spirv("shaders/02texture.vert");
//     _flat_texture_pipeline.frag_from_spirv("shaders/02texture.frag");

//     _flat_texture_pipeline.describe_vertex_input(
//         VertexFlatTexture::bindings,
//         VertexFlatTexture::attributes
//     );

//     _flat_texture_pipeline.add_descriptor_set(
//         _camera_set_layout.native()
//     );

//     _flat_texture_pipeline.add_push_constant(
//         vk::ShaderStageFlagBits::eAll,
//         sizeof(Mat4)
//     );

//     _flat_texture_pipeline.add_descriptor_set(
//         _flat_texture_set_layout.native()
//     );

//     _flat_texture_pipeline.create(
//         _render_pass,
//         Pipeline::Config {
//             .polygon_mode = vk::PolygonMode::eFill,
//             .cull_mode    = vk::CullModeFlagBits::eBack,
//             .front_face   = vk::FrontFace::eClockwise,
//             .max_msaa_samples = max_msaa_flag(),
//         }
//     );
// }

// // =============================================================================
// void Renderer::_init_skybox_pipeline() {
//     _skybox_pipeline.vert_from_spirv("shaders/03skybox.vert");
//     _skybox_pipeline.frag_from_spirv("shaders/03skybox.frag");

//     _skybox_pipeline.describe_vertex_input(
//         VertexSkybox::bindings,
//         VertexSkybox::attributes
//     );

//     _skybox_pipeline.add_descriptor_set(_camera_set_layout.native());
//     _skybox_pipeline.add_descriptor_set(_skybox_set_layout.native());
//     _skybox_pipeline.create(
//         _render_pass,
//         Pipeline::Config {
//             .polygon_mode = vk::PolygonMode::eFill,
//             .cull_mode    = vk::CullModeFlagBits::eBack,
//             .front_face   = vk::FrontFace::eClockwise,
//             .max_msaa_samples = max_msaa_flag(),
//         }
//     );
// }

// // =============================================================================
// void Renderer::_init_lit_color_pipeline() {
//     _lit_color_pipeline.vert_from_spirv("shaders/04litcolor.vert");
//     _lit_color_pipeline.frag_from_spirv("shaders/04litcolor.frag");

//     _lit_color_pipeline.describe_vertex_input(
//         VertexLitColor::bindings,
//         VertexLitColor::attributes
//     );

//     _lit_color_pipeline.add_descriptor_set(
//         _camera_set_layout.native()
//     );

//     _lit_color_pipeline.add_descriptor_set(
//         _lit_color_set_layout.native()
//     );

//     _lit_color_pipeline.add_push_constant(
//         vk::ShaderStageFlagBits::eAll,
//         sizeof(Mat4)
//     );

//     _lit_color_pipeline.create(
//         _render_pass,
//         Pipeline::Config {
//             .polygon_mode = vk::PolygonMode::eFill,
//             .cull_mode    = vk::CullModeFlagBits::eBack,
//             .front_face   = vk::FrontFace::eClockwise,
//             .max_msaa_samples = max_msaa_flag(),
//         }
//     );
// }

// =============================================================================
void
Renderer::_execute_flat_color_pipeline(vk::CommandBuffer const &cmd_buffer) {
    _flat_color_pipeline.bind(cmd_buffer);

    cmd_buffer.bindDescriptorSets(
        vk::PipelineBindPoint::eGraphics,
        _flat_color_pipeline.layout(),
        0u,
        { _global_buffer_sets[_frame_index].native() },
        nullptr
    );

    for(auto const &draw : _flat_color_draws) {
        _send_push_constants(_flat_color_pipeline, draw, cmd_buffer);

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

// // =============================================================================
// void Renderer::_execute_lit_color_pipeline(vk::CommandBuffer const &cmd_buffer)
// {
//     cmd_buffer.bindPipeline(
//         vk::PipelineBindPoint::eGraphics,
//         _lit_color_pipeline.native()
//     );
//     cmd_buffer.setViewport(0u, _lit_color_pipeline.viewport());
//     cmd_buffer.setScissor(0u,  _lit_color_pipeline.scissor());

//     _bind_camera_uniforms(_lit_color_pipeline, cmd_buffer);

//     cmd_buffer.bindDescriptorSets(
//         vk::PipelineBindPoint::eGraphics,
//         _lit_color_pipeline.layout(),
//         DescBindSlot::PER_TEXTURE,
//         { _lit_color_sets[Engine::frame_index()].back().native() },
//         nullptr
//     );

//     for(auto const &draw : _lit_color_draws) {
//         _send_push_constants(_lit_color_pipeline, draw, cmd_buffer);

//         cmd_buffer.bindVertexBuffers(
//             0u,
//             { draw.mesh->vertex_buffer().buffer().handle },
//             { 0u }
//         );
//         cmd_buffer.bindIndexBuffer(
//             draw.mesh->index_buffer().buffer().handle,
//             0u,
//             INDEX_TYPE
//         );
//         cmd_buffer.drawIndexed(
//             static_cast<uint32_t>(draw.mesh->index_count()),
//             1u, 0u, 0u, 0u
//         );
//     }

//     _lit_color_draws.clear();
// }

// // =============================================================================
// void
// Renderer::_execute_flat_texture_pipeline(vk::CommandBuffer const &cmd_buffer) {
//     cmd_buffer.bindPipeline(
//         vk::PipelineBindPoint::eGraphics,
//         _flat_texture_pipeline.native()
//     );
//     cmd_buffer.setViewport(0u, _flat_texture_pipeline.viewport());
//     cmd_buffer.setScissor(0u,  _flat_texture_pipeline.scissor());

//     _bind_camera_uniforms(_flat_texture_pipeline, cmd_buffer);

//     for(auto &[texture_id, draw_queue] : _flat_texture_draws) {
//         cmd_buffer.bindDescriptorSets(
//             vk::PipelineBindPoint::eGraphics,
//             _flat_texture_pipeline.layout(),
//             DescBindSlot::PER_TEXTURE,
//             { _flat_texture_sets[draw_queue.set_index].native() },
//             nullptr
//         );

//         for(auto const &draw : draw_queue.queue) {
//             _send_push_constants(_flat_texture_pipeline, draw, cmd_buffer);

//             cmd_buffer.bindVertexBuffers(
//                 0u,
//                 { draw.mesh->vertex_buffer().buffer().handle },
//                 { 0u }
//             );
//             cmd_buffer.bindIndexBuffer(
//                 draw.mesh->index_buffer().buffer().handle,
//                 0u,
//                 INDEX_TYPE
//             );
//             cmd_buffer.drawIndexed(
//                 static_cast<uint32_t>(draw.mesh->index_count()),
//                 1u, 0u, 0u, 0u
//             );
//         }

//         draw_queue.queue.clear();
//     }
// }

// // =============================================================================
// void Renderer::_execute_skybox_pipeline(vk::CommandBuffer const &cmd_buffer) {
//     if(_skybox_draw.mesh == nullptr || _skybox_draw.material == nullptr) {
//         return;
//     }

//     cmd_buffer.bindPipeline(
//         vk::PipelineBindPoint::eGraphics,
//         _skybox_pipeline.native()
//     );
//     cmd_buffer.setViewport(0u, _skybox_pipeline.viewport());
//     cmd_buffer.setScissor(0u,  _skybox_pipeline.scissor());

//     _bind_camera_uniforms(_skybox_pipeline, cmd_buffer);

//     cmd_buffer.bindDescriptorSets(
//         vk::PipelineBindPoint::eGraphics,
//         _skybox_pipeline.layout(),
//         DescBindSlot::PER_TEXTURE,
//         { _skybox_set.native() },
//         nullptr
//     );

//     cmd_buffer.bindVertexBuffers(
//         0u,
//         { _skybox_draw.mesh->vertex_buffer().buffer().handle },
//         { 0u }
//     );
//     cmd_buffer.bindIndexBuffer(
//         _skybox_draw.mesh->index_buffer().buffer().handle,
//         0u,
//         INDEX_TYPE
//     );
//     cmd_buffer.drawIndexed(
//         static_cast<uint32_t>(_skybox_draw.mesh->index_count()),
//         1u, 0u, 0u, 0u
//     );
// }

} // namespace vkl
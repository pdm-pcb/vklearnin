#include "vklearnin/vklearnin.hpp"
#include "vklearnin/rendering/Renderer.hpp"

#include "vklearnin/rendering/swapchain/Swapchain.hpp"
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

DescriptorSetLayout   Renderer::_global_buffer_layout;
Renderer::DescSetList Renderer::_global_buffer_sets;

DescriptorSetLayout   Renderer::_flat_texture_layout;
Renderer::DescSetList Renderer::_flat_texture_sets;

DescriptorSet Renderer::_skybox_texture_set;

// Shader Resources ------------------------------------------------------------
Renderer::BufferList Renderer::_global_buffers;
Skybox<VertexSkybox> Renderer::_skybox_mesh;
Texture2D            Renderer::_skybox_texture;

// Pipelines -------------------------------------------------------------------
Pipeline Renderer::_flat_color_pipeline;
Pipeline Renderer::_flat_texture_pipeline;
Pipeline Renderer::_skybox_pipeline;

// Draw Queues -----------------------------------------------------------------
Renderer::FlatColorDrawQueue Renderer::_flat_color_draws;
Renderer::FlatTextureDrawQueue Renderer::_flat_texture_draws;

// =============================================================================
void Renderer::update_global_buffer(GlobalBuffer const &buffer) {
    BufferTools::update_buffer(_global_buffers[_frame_index], &buffer);
}

// =============================================================================
void Renderer::submit_draw(Mesh<VertexFlatColor> const &mesh,
                           Mat4 const &model_matrix)
{
    _flat_color_draws.push_back(
        DrawSubmission<VertexFlatColor> {
            .mesh = &mesh,
            .push_constants = {{
                .stage_flags = vk::ShaderStageFlagBits::eAll,
                .size        = sizeof(vkl::Mat4),
                .data        = &model_matrix,
            }}
        }
    );
}

// =============================================================================
void Renderer::submit_draw(Mesh<VertexFlatTexture> const &mesh,
                           Texture2D const &texture,
                           Mat4 const &model_matrix)
{
    auto texture_id = reinterpret_cast<uint64_t>(
        VkImage(texture.image().handle)
    );

    _flat_texture_draws[texture_id].queue.push_back(
        DrawSubmission<VertexFlatTexture> {
            .mesh = &mesh,
            .texture = &texture,
            .push_constants = {{
                .stage_flags = vk::ShaderStageFlagBits::eAll,
                .size        = sizeof(vkl::Mat4),
                .data        = &model_matrix,
            }}
        }
    );
}

// =============================================================================
void Renderer::record_commands() {
    auto const &frame_data = _frame_data[_frame_index];

    // Whatever frame index we're on, we need to wait on the fence signaling
    // completion of this frame's last submission to the device queue
    frame_data.wait_on_queue_fence();

    // Once we're sure the frame's work is done, it's safe to reset the command
    // pool, which implicitly resets the command buffer/s
    frame_data.cmd_pool().reset();

    vk::RenderPassBeginInfo const render_pass_info {
        .renderPass      = _render_pass.native(),
        .framebuffer     = _framebuffers[_frame_index].native(),
        .renderArea      = vkl::Swapchain::render_area(),
        .clearValueCount = static_cast<uint32_t>(std::size(clear_values)),
        .pClearValues    = clear_values
    };

    frame_data.cmd_buffer().begin_one_time_submit();
    frame_data.cmd_buffer().begin_render_pass(render_pass_info);

        _execute_flat_color_pipeline();
        _execute_flat_texture_pipeline();
        _execute_skybox_pipeline();

    frame_data.cmd_buffer().end_render_pass();
    frame_data.cmd_buffer().end_recording();
}

// =============================================================================
void Renderer::submit_commands_and_present() {
    auto &frame_data = _frame_data[_frame_index];

    // The first task after completing recording to the command buffer is to
    // query the presentation engine for which swapchain image it wants us to
    // write to next
    Swapchain::acquire_next_image_index(frame_data);

    // Once we know which image this command buffer is being written to, we can
    // submit it to the graphics card
    frame_data.submit_to_device();

    // And finally, ask the presenatation engine to show the completed image
    Swapchain::present(frame_data);

    ++_frame_count;
    _frame_index = _frame_count % RenderConfig::swapchain_image_count;
}

// =============================================================================
void Renderer::init() {
    Swapchain::create();

    _render_pass.create();
    _init_framebuffers();
    _init_frame_data();
    _init_descriptor_pool();
}

// =============================================================================
void Renderer::shutdown() {
    _flat_color_pipeline.destroy();
    _flat_texture_pipeline.destroy();
    _skybox_pipeline.destroy();

    _global_buffer_layout.destroy();
    _flat_texture_layout.destroy();

    _desc_pool.destroy();

    for(auto &buffer : _global_buffers) {
        BufferTools::destroy(buffer);
    }

    _skybox_mesh.shutdown();
    _skybox_texture.shutdown();

    for(auto &framebuffer : _framebuffers) {
        framebuffer.destroy();
    }

    for(auto &frame : _frame_data) {
        frame.shutdown();
    }

    _render_pass.destroy();

    Swapchain::destroy();
}

// =============================================================================
void Renderer::set_flat_textures(std::vector<Texture2D> const &textures) {
    for(auto const &texture : textures) {
        auto const set_index = _flat_texture_sets.size();

        _flat_texture_sets.emplace_back();
        _flat_texture_sets.back().add_image(texture.image());

        // WANRING: Vulkan handles are reused by the driver, so this is a
        //          terrible way to key an unordered map if you don't keep
        //          every resource loaded into VRAM all the time
        auto const texture_id = reinterpret_cast<uint64_t>(
            VkImage(texture.image().handle)
        );

        _flat_texture_draws.insert({
            texture_id,
            PerFlatTextureDraws {
                .set_index = set_index,
                .queue = { }
            }
        });

        // TODO: these magic numbers are nonsense and you know it
        _flat_texture_draws[texture_id].queue.reserve(100);
    }
}

// =============================================================================
void Renderer::set_skybox_texture(Texture2D::CubeFilepaths const &filepaths) {
    _skybox_texture.cubemap_from_files(filepaths);
    _skybox_texture.init_sampler(
        vk::Filter::eLinear,
        vk::Filter::eLinear,
        vk::SamplerMipmapMode::eLinear,
        vk::SamplerAddressMode::eRepeat,
        vk::SamplerAddressMode::eRepeat
    );
}

// =============================================================================
void Renderer::create_pipelines() {
    _init_global_buffers();
    _init_flat_textures();
    _init_skybox_resources();

    _init_flat_color_pipeline();
    _init_flat_texture_pipeline();
    _init_skybox_pipeline();
}

// =============================================================================
void Renderer::_init_framebuffers() {
    _framebuffers.reserve(RenderConfig::swapchain_image_count);

    for(auto const &swapchain_image : Swapchain::images()) {
        _framebuffers.emplace_back(Framebuffer { });
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
    // TODO: these magic numbers are nonsense and you know it
    _desc_pool.create(
        100u,
        {{
            .type = vk::DescriptorType::eUniformBuffer,
            .descriptorCount = 100u,
        },
        {
            .type = vk::DescriptorType::eCombinedImageSampler,
            .descriptorCount = 100u,
        }}
    );
}

// =============================================================================
void Renderer::_init_global_buffers() {
    _global_buffer_layout
        .add_binding(
            vk::DescriptorType::eUniformBuffer,
            vk::ShaderStageFlagBits::eAll
        )
        .create();

    _global_buffer_sets.resize(RenderConfig::swapchain_image_count);

    for(auto &set : _global_buffer_sets) {
        set.allocate(_desc_pool, _global_buffer_layout);
    }

    _global_buffers.resize(_global_buffer_sets.size());
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

    CONSOLE_INFO(
        "Renderer will use {} global buffer descriptor sets",
        _global_buffer_sets.size()
    );
}

// =============================================================================
void Renderer::_init_flat_textures() {
    _flat_texture_layout.add_binding(
        vk::DescriptorType::eCombinedImageSampler,
        vk::ShaderStageFlagBits::eFragment
    );
    _flat_texture_layout.create();

    for(auto &descriptor_set : _flat_texture_sets) {
        descriptor_set
            .allocate(_desc_pool, _flat_texture_layout)
            .write_set();
    }

    CONSOLE_INFO(
        "Renderer will use {} flat texture descriptor sets",
        _flat_texture_sets.size()
    );
}

// =============================================================================
void Renderer::_init_skybox_resources() {
    _skybox_mesh.init(500.0f);

    _skybox_texture_set
        .add_image(_skybox_texture.image())
        .allocate(_desc_pool, _flat_texture_layout)
        .write_set();
}

// =============================================================================
void Renderer::_init_flat_color_pipeline() {
    _flat_color_pipeline
        .vert_from_spirv("shaders/01flat_color.vert")
        .frag_from_spirv("shaders/01flat_color.frag")
        .describe_vertex_input(
            VertexFlatColor::bindings,
            VertexFlatColor::attributes
        )
        .add_descriptor_set(_global_buffer_layout)
        .add_push_constant(
            vk::ShaderStageFlagBits::eAll,
            sizeof(Mat4)
        )
        .create(
            _render_pass,
            Pipeline::Config {
                .polygon_mode     = vk::PolygonMode::eFill,
                .cull_mode        = vk::CullModeFlagBits::eBack,
                .front_face       = vk::FrontFace::eClockwise,
                .max_msaa_samples = max_msaa_flag(),
                .subpass_index    = 0u,
            }
        );
}

// =============================================================================
void Renderer::_init_flat_texture_pipeline() {
    _flat_texture_pipeline
        .vert_from_spirv("shaders/02flat_texture.vert")
        .frag_from_spirv("shaders/02flat_texture.frag")
        .describe_vertex_input(
            VertexFlatTexture::bindings,
            VertexFlatTexture::attributes
        )
        .add_descriptor_set(_global_buffer_layout)
        .add_descriptor_set(_flat_texture_layout)
        .add_push_constant(
            vk::ShaderStageFlagBits::eAll,
            sizeof(Mat4)
        )
        .create(
            _render_pass,
            Pipeline::Config {
                .polygon_mode     = vk::PolygonMode::eFill,
                .cull_mode        = vk::CullModeFlagBits::eBack,
                .front_face       = vk::FrontFace::eClockwise,
                .max_msaa_samples = max_msaa_flag(),
                .subpass_index    = 0u,
            }
        );
}

// =============================================================================
void Renderer::_init_skybox_pipeline() {
    _skybox_pipeline
        .vert_from_spirv("shaders/03skybox.vert")
        .frag_from_spirv("shaders/03skybox.frag")
        .describe_vertex_input(
            VertexSkybox::bindings,
            VertexSkybox::attributes
        )
        .add_descriptor_set(_global_buffer_layout)
        .add_descriptor_set(_flat_texture_layout)
        .add_push_constant(
            vk::ShaderStageFlagBits::eAll,
            sizeof(Mat4)
        )
        .create(
            _render_pass,
            Pipeline::Config {
                .polygon_mode     = vk::PolygonMode::eFill,
                .cull_mode        = vk::CullModeFlagBits::eBack,
                .front_face       = vk::FrontFace::eClockwise,
                .max_msaa_samples = max_msaa_flag(),
                .subpass_index    = 0u,
            }
        );
}

// =============================================================================
void Renderer::_execute_flat_color_pipeline() {
    auto const &frame_data        = _frame_data[_frame_index];
    auto const &cmd_buffer        = frame_data.cmd_buffer();
    auto const &global_buffer_set = _global_buffer_sets[_frame_index];

    _flat_color_pipeline.bind(cmd_buffer);
    _flat_color_pipeline.bind_descriptor_set(cmd_buffer, global_buffer_set);

    for(auto const &draw : _flat_color_draws) {
        _send_push_constants(_flat_color_pipeline, draw, frame_data);
        draw.mesh->draw_indexed(cmd_buffer);
    }

    _flat_color_draws.clear();
}

// =============================================================================
void Renderer::_execute_flat_texture_pipeline() {
    auto const &frame_data        = _frame_data[_frame_index];
    auto const &cmd_buffer        = frame_data.cmd_buffer();
    auto const &global_buffer_set = _global_buffer_sets[_frame_index];

    _flat_texture_pipeline.bind(cmd_buffer);
    _flat_texture_pipeline.bind_descriptor_set(cmd_buffer, global_buffer_set);

    for(auto &[texture_id, draw_queue] : _flat_texture_draws) {
        _flat_texture_pipeline.bind_descriptor_set(
            cmd_buffer,
            _flat_texture_sets[draw_queue.set_index]
        );

        for(auto const &draw : draw_queue.queue) {
            _send_push_constants(_flat_texture_pipeline, draw, frame_data);
            draw.mesh->draw_indexed(cmd_buffer);
        }

        draw_queue.queue.clear();
    }
}

// =============================================================================
void Renderer::_execute_skybox_pipeline() {
    auto const &frame_data        = _frame_data[_frame_index];
    auto const &cmd_buffer        = frame_data.cmd_buffer();
    auto const &global_buffer_set = _global_buffer_sets[_frame_index];

    _skybox_pipeline.bind(cmd_buffer);
    _skybox_pipeline.bind_descriptor_set(cmd_buffer, global_buffer_set);
    _skybox_pipeline.bind_descriptor_set(cmd_buffer, _skybox_texture_set);

    _skybox_mesh.draw_indexed(cmd_buffer);
}

} // namespace vkl
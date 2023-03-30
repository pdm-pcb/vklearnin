#include "vklearnin/vklearnin.hpp"
#include "vklearnin/rendering/Renderer.hpp"

#include "vklearnin/rendering/swapchain/Swapchain.hpp"
#include "vklearnin/resources/buffers/BufferObject.hpp"
#include "vklearnin/resources/images/ImageObject.hpp"
#include "vklearnin/meshes/GeneratedMesh.hpp"

namespace vkl {

static vk::ClearValue const color_pass_clear[] = {
    { .color { vkl::RenderConfig::CLEAR_COLOR }},
    { .depthStencil
        {
            .depth = 1.0f,
            .stencil = 1u,
        }
    }
};

static vk::ClearValue const shadow_pass_clear[] = {
    { .depthStencil
        {
            .depth = 1.0f,
            .stencil = 1u,
        }
    }
};

RenderPass Renderer::_color_pass;
RenderPass Renderer::_shadow_map_pass;

std::vector<Framebuffer> Renderer::_color_framebuffers;
std::vector<Framebuffer> Renderer::_shadow_map_framebuffers;

uint32_t Renderer::_shadow_map_resolution = 0u;

std::vector<FrameData>   Renderer::_frame_data;
uint32_t                 Renderer::_frame_index = 0u;
uint64_t                 Renderer::_frame_count = 0u;

// Descriptors -----------------------------------------------------------------
static constexpr uint32_t DESC_POOL_MAGIC_NUMBER = 100u; // TODO: nonsense

DescriptorPool Renderer::_desc_pool;

DescriptorSetLayout   Renderer::_global_data_layout;
Renderer::DescSetList Renderer::_global_data_sets;

DescriptorSetLayout   Renderer::_texture_layout;
Renderer::DescSetList Renderer::_texture_sets;

DescriptorSet Renderer::_skybox_texture_set;

DescriptorSetLayout   Renderer::_light_props_layout;
Renderer::DescSetList Renderer::_light_props_sets;

DescriptorSetLayout   Renderer::_material_layout;
Renderer::DescSetList Renderer::_material_sets;

DescriptorSetLayout   Renderer::_shadow_map_transform_layout;
Renderer::DescSetList Renderer::_shadow_map_transform_sets;
Renderer::DescSetList Renderer::_shadow_map_sets;

// Shader Resources ------------------------------------------------------------
Renderer::BufferList Renderer::_camera_buffers;
Skybox               Renderer::_skybox_mesh;
Texture2D            Renderer::_skybox_texture;
Renderer::BufferList Renderer::_light_props_buffers;
Renderer::BufferList Renderer::_shadow_map_transform_buffers;

// Pipelines -------------------------------------------------------------------
Pipeline Renderer::_flat_color_pipeline;
Pipeline Renderer::_texture_pipeline;
Pipeline Renderer::_skybox_pipeline;
Pipeline Renderer::_lit_color_pipeline;
Pipeline Renderer::_material_pipeline;
Pipeline Renderer::_shadow_map_pipeline;

// Draw Queues -----------------------------------------------------------------
static constexpr uint32_t DRAW_QUEUE_MAGIC_NUMBER = 100u; // TODO: nonsense

Renderer::DrawQueue         Renderer::_flat_color_draws;
Renderer::TextureDrawQueue  Renderer::_texture_draws;
Renderer::DrawQueue         Renderer::_lit_color_draws;
Renderer::MaterialDrawQueue Renderer::_material_draws;

// =============================================================================
void Renderer::update_camera_data(CameraData const &data) {
    BufferTools::update_buffer(_camera_buffers[_frame_index], &data);
}

// =============================================================================
void Renderer::update_light_props(LightProps const &data) {
    BufferTools::update_buffer(_light_props_buffers[_frame_index], &data);

    auto const light_proj_mat = math::persp_proj_rh_zo_inf(
        0.1f, 45.0f, 1.0f
    );
    auto const light_view_mat = math::look_at(
        data.spot.position,
        Vec4::origin,
        Vec4::unit_y
    );

    ShadowMapTransforms const transforms {
        .light_vp_matrix = light_proj_mat * light_view_mat,
    };

    BufferTools::update_buffer(_shadow_map_transform_buffers[_frame_index],
                               &transforms);
}

// =============================================================================
void Renderer::submit_draw_flat(GeneratedMesh const &mesh,
                                Mat4 const &model_matrix)
{
    _flat_color_draws.push_back(
        DrawSubmission {
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
void Renderer::submit_draw(GeneratedMesh const &mesh,
                           Texture2D const &texture,
                           Mat4 const &model_matrix)
{
    auto texture_id = reinterpret_cast<uint64_t>(
        VkImage(texture.image().handle)
    );

    _texture_draws[texture_id].queue.push_back(
        DrawSubmission {
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
void Renderer::submit_draw_lit(GeneratedMesh const &mesh,
                                Mat4 const &model_matrix)
{

    _lit_color_draws.push_back(
        DrawSubmission {
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
void Renderer::submit_draw(GeneratedMesh const &mesh,
                           Material const &material,
                           Mat4 const &model_matrix)
{
    auto material_id = reinterpret_cast<uint64_t>(
        VkImage(material.diffuse.image().handle)
    );

    _material_draws[material_id].queue.push_back(
        DrawSubmission {
            .mesh = &mesh,
            .material = &material,
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

    vk::RenderPassBeginInfo const shadow_pass_info {
        .renderPass      = _shadow_map_pass.native(),
        .framebuffer     = _shadow_map_framebuffers[_frame_index].native(),
        .renderArea      = _shadow_map_framebuffers[_frame_index].render_area(),
        .clearValueCount = static_cast<uint32_t>(std::size(shadow_pass_clear)),
        .pClearValues    = shadow_pass_clear
    };

    vk::RenderPassBeginInfo const color_pass_info {
        .renderPass      = _color_pass.native(),
        .framebuffer     = _color_framebuffers[_frame_index].native(),
        .renderArea      = _color_framebuffers[_frame_index].render_area(),
        .clearValueCount = static_cast<uint32_t>(std::size(color_pass_clear)),
        .pClearValues    = color_pass_clear
    };

    frame_data.cmd_buffer().begin_one_time_submit();
    frame_data.cmd_buffer().begin_render_pass(shadow_pass_info);

        _execute_shadow_map_pipeline();

    frame_data.cmd_buffer().end_render_pass();
    frame_data.cmd_buffer().begin_render_pass(color_pass_info);

        _execute_flat_color_pipeline();
        _execute_texture_pipeline();
        _execute_skybox_pipeline();
        _execute_lit_color_pipeline();
        _execute_material_pipeline();

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

    _init_color_pass();
    _init_color_framebuffers();

    _init_shadow_map_pass();
    _init_shadow_map_framebuffers();

    _init_frame_data();

    _init_descriptor_pool();
}

// =============================================================================
void Renderer::shutdown() {
    _flat_color_pipeline.destroy();
    _texture_pipeline.destroy();
    _skybox_pipeline.destroy();
    _lit_color_pipeline.destroy();
    _material_pipeline.destroy();
    _shadow_map_pipeline.destroy();

    _global_data_layout.destroy();
    _texture_layout.destroy();
    _light_props_layout.destroy();
    _material_layout.destroy();
    _shadow_map_transform_layout.destroy();

    _desc_pool.destroy();

    for(auto &buffer : _camera_buffers) {
        BufferTools::destroy(buffer);
    }

    _skybox_mesh.destroy();
    _skybox_texture.destroy();

    for(auto &buffer : _light_props_buffers) {
        BufferTools::destroy(buffer);
    }

    for(auto &buffer : _shadow_map_transform_buffers) {
        BufferTools::destroy(buffer);
    }

    for(auto &framebuffer : _shadow_map_framebuffers) {
        framebuffer.destroy();
    }

    for(auto &framebuffer : _color_framebuffers) {
        framebuffer.destroy();
    }

    for(auto &frame : _frame_data) {
        frame.shutdown();
    }

    _shadow_map_pass.destroy();
    _color_pass.destroy();

    Swapchain::destroy();
}

// =============================================================================
void Renderer::set_textures(std::vector<Texture2D> const &textures) {
    for(auto const &texture : textures) {
        auto const set_index = _texture_sets.size();

        _texture_sets.emplace_back();
        _texture_sets.back().add_image(texture.image());

        // WANRING: Vulkan handles are reused by the driver, so this is a
        //          terrible way to key an unordered map if you don't keep
        //          every resource loaded into VRAM all the time
        auto const texture_id = reinterpret_cast<uint64_t>(
            VkImage(texture.image().handle)
        );

        _texture_draws.insert({
            texture_id,
            PerTextureDraws {
                .set_index = set_index,
                .queue = { }
            }
        });
    }
}

// =============================================================================
void Renderer::set_skybox_texture(Texture2D::CubeFilepaths const &filepaths) {
    _skybox_texture.cubemap_from_files(filepaths);
    _skybox_texture.create_sampler(
        vk::Filter::eLinear,
        vk::Filter::eLinear,
        vk::SamplerMipmapMode::eLinear,
        vk::SamplerAddressMode::eRepeat,
        vk::SamplerAddressMode::eRepeat,
        VK_FALSE,
        vk::CompareOp::eAlways
    );
    _skybox_texture.generate_mipmap(vk::Filter::eLinear);
}

// =============================================================================
void Renderer::set_materials(std::vector<Material> const &materials) {
    for(auto const &material : materials) {
        auto const set_index = _material_sets.size();

        _material_sets.emplace_back();
        _material_sets.back().add_image(material.diffuse.image());

        // WANRING: Vulkan handles are reused by the driver, so this is a
        //          terrible way to key an unordered map if you don't keep
        //          every resource loaded into VRAM all the time
        auto const material_id = reinterpret_cast<uint64_t>(
            VkImage(material.diffuse.image().handle)
        );

        _material_draws.insert({
            material_id,
            PerMaterialDraws {
                .set_index = set_index,
                .queue = { }
            }
        });
    }
}

// =============================================================================
void Renderer::create_pipelines() {
    _init_camera_buffers();
    _init_global_data_sets();
    _init_texture_sets();
    _init_skybox_resources();
    _init_light_props_buffers();
    _init_light_props_sets();
    _init_material_sets();
    _init_shadow_map_resources();
    _init_shadow_map_sets();

    _flat_color_draws.reserve(DRAW_QUEUE_MAGIC_NUMBER);
    _lit_color_draws.reserve(DRAW_QUEUE_MAGIC_NUMBER);

    for(auto &[texture_id, per_texture] : _texture_draws) {
        per_texture.queue.reserve(DRAW_QUEUE_MAGIC_NUMBER);
    }

    for(auto &[material_id, per_material] : _material_draws) {
        per_material.queue.reserve(DRAW_QUEUE_MAGIC_NUMBER);
    }

    _init_flat_color_pipeline();
    _init_texture_pipeline();
    _init_skybox_pipeline();
    _init_lit_color_pipeline();
    _init_material_pipeline();
    _init_shadow_map_pipeline();
}

// =============================================================================
void Renderer::_init_color_pass() {
    _color_pass
        .default_color_attachments()
        .default_color_subpass()
        .create();
}

// =============================================================================
void Renderer::_init_color_framebuffers() {
    _color_framebuffers.clear();

    for(auto const &swapchain_image : Swapchain::images()) {
        _color_framebuffers.emplace_back();
        _color_framebuffers.back()
            .create_color_buffer(Swapchain::extent())
            .create_depth_buffer(Swapchain::extent())
            .add_image_view(swapchain_image.view)
            .create(
                Swapchain::render_area(),
                _color_pass.native()
            );
    }
}

// =============================================================================
void Renderer::_init_shadow_map_pass() {
    _shadow_map_resolution =
        Swapchain::extent().width > Swapchain::extent().height ?
        Swapchain::extent().width :
        Swapchain::extent().height;

    _shadow_map_pass
        .default_shadow_map_attachments()
        .default_shadow_map_subpass()
        .create();
}

// =============================================================================
void Renderer::_init_shadow_map_framebuffers() {
    _shadow_map_framebuffers.clear();
    _shadow_map_framebuffers.resize(RenderConfig::swapchain_image_count);

    vk::Extent2D const shadow_map_extent {
        .width  = _shadow_map_resolution,
        .height = _shadow_map_resolution,
    };

    vk::Rect2D const shadow_map_render_area {
        .offset { .x = 0, .y = 0 },
        .extent = shadow_map_extent
    };

    for(auto &framebuffer : _shadow_map_framebuffers) {
        framebuffer
            .create_shadow_map(shadow_map_extent)
            .create(
                shadow_map_render_area,
                _shadow_map_pass.native()
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
        DESC_POOL_MAGIC_NUMBER,
        {{
            .type = vk::DescriptorType::eUniformBuffer,
            .descriptorCount = DESC_POOL_MAGIC_NUMBER,
        },
        {
            .type = vk::DescriptorType::eCombinedImageSampler,
            .descriptorCount = DESC_POOL_MAGIC_NUMBER,
        }}
    );
}

// =============================================================================
void Renderer::_init_camera_buffers() {
     _camera_buffers.resize(RenderConfig::swapchain_image_count);
    for(auto &buffer : _camera_buffers) {
        buffer.size = sizeof(CameraData);
        vkl::BufferTools::create(
            buffer,
            vk::BufferUsageFlagBits::eUniformBuffer,
            (vk::MemoryPropertyFlagBits::eHostVisible |
             vk::MemoryPropertyFlagBits::eHostCoherent)
        );
    }
}

// =============================================================================
void Renderer::_init_global_data_sets() {
    _global_data_layout
        .add_binding(
            vk::DescriptorType::eUniformBuffer,
            vk::ShaderStageFlagBits::eAll
        )
        .create();

    _global_data_sets.resize(RenderConfig::swapchain_image_count);

    for(uint32_t frame = 0u;
        frame < RenderConfig::swapchain_image_count;
        ++frame)
    {
        _global_data_sets[frame]
            .allocate(_desc_pool, _global_data_layout)
            .add_buffer(_camera_buffers[frame])
            .write_set();
    }

    CONSOLE_TRACE(
        "Renderer will use {} global data descriptor sets",
        _global_data_sets.size()
    );
}

// =============================================================================
void Renderer::_init_texture_sets() {
    _texture_layout
        .add_binding(
            vk::DescriptorType::eCombinedImageSampler,
            vk::ShaderStageFlagBits::eFragment
        )
        .create();

    for(auto &descriptor_set : _texture_sets) {
        descriptor_set
            .allocate(_desc_pool, _texture_layout)
            .write_set();
    }

    CONSOLE_TRACE(
        "Renderer will use {} texture descriptor sets",
        _texture_sets.size()
    );
}

// =============================================================================
void Renderer::_init_skybox_resources() {
    _skybox_mesh.init(500.0f);

    _skybox_texture_set
        .allocate(_desc_pool, _texture_layout)
        .add_image(_skybox_texture.image())
        .write_set();
}

// =============================================================================
void Renderer::_init_material_sets() {
    _material_layout
        .add_binding(
            vk::DescriptorType::eCombinedImageSampler,
            vk::ShaderStageFlagBits::eFragment
        )
        .create();

    for(auto &descriptor_set : _material_sets) {
        descriptor_set
            .allocate(_desc_pool, _material_layout)
            .write_set();
    }

    CONSOLE_TRACE(
        "Renderer will use {} material descriptor sets",
        _material_sets.size()
    );
}

// =============================================================================
void Renderer::_init_light_props_buffers() {
    _light_props_buffers.resize(RenderConfig::swapchain_image_count);
    for(auto &buffer : _light_props_buffers) {
        buffer.size = sizeof(LightProps);
        vkl::BufferTools::create(
            buffer,
            vk::BufferUsageFlagBits::eUniformBuffer,
            (vk::MemoryPropertyFlagBits::eHostVisible |
             vk::MemoryPropertyFlagBits::eHostCoherent)
        );
    }
}

// =============================================================================
void Renderer::_init_light_props_sets() {
    _light_props_layout
        .add_binding(
            vk::DescriptorType::eUniformBuffer,
            vk::ShaderStageFlagBits::eAll
        )
        .create();

    _light_props_sets.resize(RenderConfig::swapchain_image_count);

    for(uint32_t frame = 0u;
        frame < RenderConfig::swapchain_image_count;
        ++frame)
    {
        _light_props_sets[frame]
            .allocate(_desc_pool, _light_props_layout)
            .add_buffer(_light_props_buffers[frame])
            .write_set();
    }
}

// =============================================================================
void Renderer::_init_shadow_map_resources() {
    _shadow_map_transform_buffers.resize(RenderConfig::swapchain_image_count);
    for(auto &buffer : _shadow_map_transform_buffers) {
        buffer.size = sizeof(ShadowMapTransforms);
        vkl::BufferTools::create(
            buffer,
            vk::BufferUsageFlagBits::eUniformBuffer,
            (vk::MemoryPropertyFlagBits::eHostVisible |
             vk::MemoryPropertyFlagBits::eHostCoherent)
        );
    }
}

// =============================================================================
void Renderer::_init_shadow_map_sets() {
    _shadow_map_transform_layout
        .add_binding(
            vk::DescriptorType::eUniformBuffer,
            vk::ShaderStageFlagBits::eAll
        )
        .create();

    _shadow_map_transform_sets.resize(RenderConfig::swapchain_image_count);
    for(uint32_t frame = 0u;
        frame < RenderConfig::swapchain_image_count;
        ++frame)
    {
        _shadow_map_transform_sets[frame]
            .allocate(_desc_pool, _shadow_map_transform_layout)
            .add_buffer(_shadow_map_transform_buffers[frame])
            .write_set();
    }

    _shadow_map_sets.resize(RenderConfig::swapchain_image_count);
    for(uint32_t frame = 0u;
        frame < RenderConfig::swapchain_image_count;
        ++frame)
    {
        _shadow_map_sets[frame]
            .allocate(_desc_pool, _texture_layout)
            .add_image(_shadow_map_framebuffers[frame].shadow_map().image())
            .write_set();
    }
}

// =============================================================================
void Renderer::_init_flat_color_pipeline() {
    _flat_color_pipeline
        .vert_from_spirv("shaders/01flat_color.vert")
        .frag_from_spirv("shaders/01flat_color.frag")
        .describe_vertex_input(
            Vertex::bindings,
            Vertex::attributes
        )
        .add_descriptor_set(_global_data_layout)
        .add_push_constant(
            vk::ShaderStageFlagBits::eAll,
            sizeof(Mat4)
        )
        .create(
            _color_pass,
            Pipeline::Config {
                .viewport_extent   = Swapchain::extent(),
                .viewport_offset   = Swapchain::offset(),
                .msaa_samples      = RenderConfig::max_msaa_flag(),
                .enable_depth_test = VK_TRUE,
                .depth_compare     = vk::CompareOp::eLess,
            }
        );
}

// =============================================================================
void Renderer::_init_texture_pipeline() {
    _texture_pipeline
        .vert_from_spirv("shaders/02texture.vert")
        .frag_from_spirv("shaders/02texture.frag")
        .describe_vertex_input(
            Vertex::bindings,
            Vertex::attributes
        )
        .add_descriptor_set(_global_data_layout)
        .add_descriptor_set(_texture_layout)
        .add_push_constant(
            vk::ShaderStageFlagBits::eAll,
            sizeof(Mat4)
        )
        .create(
            _color_pass,
            Pipeline::Config {
                .viewport_extent   = Swapchain::extent(),
                .viewport_offset   = Swapchain::offset(),
                .msaa_samples      = RenderConfig::max_msaa_flag(),
                .enable_depth_test = VK_TRUE,
                .depth_compare     = vk::CompareOp::eLess,
            }
        );
}

// =============================================================================
void Renderer::_init_skybox_pipeline() {
    _skybox_pipeline
        .vert_from_spirv("shaders/03skybox.vert")
        .frag_from_spirv("shaders/03skybox.frag")
        .describe_vertex_input(
            Vertex::bindings,
            Vertex::attributes
        )
        .add_descriptor_set(_global_data_layout)
        .add_descriptor_set(_texture_layout)
        .add_push_constant(
            vk::ShaderStageFlagBits::eAll,
            sizeof(Mat4)
        )
        .create(
            _color_pass,
            Pipeline::Config {
                .viewport_extent   = Swapchain::extent(),
                .viewport_offset   = Swapchain::offset(),
                .msaa_samples      = RenderConfig::max_msaa_flag(),
                .enable_depth_test = VK_TRUE,
                .depth_compare     = vk::CompareOp::eLess,
            }
        );
}

// =============================================================================
void Renderer::_init_lit_color_pipeline() {
    _lit_color_pipeline
        .vert_from_spirv("shaders/04lit_color.vert")
        .frag_from_spirv("shaders/04lit_color.frag")
        .describe_vertex_input(
            Vertex::bindings,
            Vertex::attributes
        )
        .add_descriptor_set(_global_data_layout)
        .add_descriptor_set(_light_props_layout)
        .add_descriptor_set(_shadow_map_transform_layout)
        .add_descriptor_set(_texture_layout)
        .add_push_constant(
            vk::ShaderStageFlagBits::eAll,
            sizeof(Mat4)
        )
        .create(
            _color_pass,
            Pipeline::Config {
                .viewport_extent   = Swapchain::extent(),
                .viewport_offset   = Swapchain::offset(),
                .msaa_samples      = RenderConfig::max_msaa_flag(),
                .enable_depth_test = VK_TRUE,
                .depth_compare     = vk::CompareOp::eLess,
            }
        );
}

// =============================================================================
void Renderer::_init_material_pipeline() {
    _material_pipeline
        .vert_from_spirv("shaders/05material.vert")
        .frag_from_spirv("shaders/05material.frag")
        .describe_vertex_input(
            Vertex::bindings,
            Vertex::attributes
        )
        .add_descriptor_set(_global_data_layout)
        .add_descriptor_set(_light_props_layout)
        .add_descriptor_set(_material_layout)
        .add_push_constant(
            vk::ShaderStageFlagBits::eAll,
            sizeof(Mat4)
        )
        .create(
            _color_pass,
            Pipeline::Config {
                .viewport_extent   = Swapchain::extent(),
                .viewport_offset   = Swapchain::offset(),
                .msaa_samples      = RenderConfig::max_msaa_flag(),
                .enable_depth_test = VK_TRUE,
                .depth_compare     = vk::CompareOp::eLess,
            }
        );
}

// =============================================================================
void Renderer::_init_shadow_map_pipeline() {
    _shadow_map_pipeline
        .vert_from_spirv("shaders/06shadow_map.vert")
        .describe_vertex_input(
            Vertex::bindings,
            Vertex::attributes
        )
        .add_descriptor_set(_shadow_map_transform_layout)
        .add_push_constant(
            vk::ShaderStageFlagBits::eAll,
            sizeof(Mat4)
        )
        .create(
            _shadow_map_pass,
            Pipeline::Config {
                .viewport_extent = vk::Extent2D {
                    .width  = _shadow_map_resolution,
                    .height = _shadow_map_resolution,
                },
                .enable_depth_test   = VK_TRUE,
                .depth_compare       = vk::CompareOp::eLessOrEqual,
                .enable_depth_bias   = VK_TRUE,
                .depth_bias_constant = 0.0f,
                .depth_bias_slope    = 2.0f,
            }
        );
}

// =============================================================================
void Renderer::_execute_flat_color_pipeline() {
    auto const &frame_data        = _frame_data[_frame_index];
    auto const &cmd_buffer        = frame_data.cmd_buffer();
    auto const &global_buffer_set = _global_data_sets[_frame_index];

    _flat_color_pipeline.bind(cmd_buffer);
    _flat_color_pipeline.bind_descriptor_set(cmd_buffer, global_buffer_set);

    for(auto const &draw : _flat_color_draws) {
        _send_push_constants(_flat_color_pipeline, draw, frame_data);
        draw.mesh->draw_indexed(cmd_buffer);
    }

    _flat_color_draws.clear();
}

// =============================================================================
void Renderer::_execute_texture_pipeline() {
    auto const &frame_data        = _frame_data[_frame_index];
    auto const &cmd_buffer        = frame_data.cmd_buffer();
    auto const &global_buffer_set = _global_data_sets[_frame_index];

    _texture_pipeline.bind(cmd_buffer);
    _texture_pipeline.bind_descriptor_set(cmd_buffer, global_buffer_set);

    for(auto &[texture_id, per_texture] : _texture_draws) {
        _texture_pipeline.bind_descriptor_set(
            cmd_buffer,
            _texture_sets[per_texture.set_index]
        );

        for(auto const &draw : per_texture.queue) {
            _send_push_constants(_texture_pipeline, draw, frame_data);
            draw.mesh->draw_indexed(cmd_buffer);
        }

        per_texture.queue.clear();
    }
}

// =============================================================================
void Renderer::_execute_skybox_pipeline() {
    auto const &frame_data        = _frame_data[_frame_index];
    auto const &cmd_buffer        = frame_data.cmd_buffer();
    auto const &global_buffer_set = _global_data_sets[_frame_index];

    _skybox_pipeline.bind(cmd_buffer);
    _skybox_pipeline.bind_descriptor_set(cmd_buffer, global_buffer_set);
    _skybox_pipeline.bind_descriptor_set(cmd_buffer, _skybox_texture_set);

    _skybox_mesh.draw_indexed(cmd_buffer);
}

// =============================================================================
void Renderer::_execute_lit_color_pipeline() {
    auto const &frame_data        = _frame_data[_frame_index];
    auto const &cmd_buffer        = frame_data.cmd_buffer();
    auto const &global_buffer_set = _global_data_sets[_frame_index];
    auto const &light_props_set   = _light_props_sets[_frame_index];
    auto const &shadow_map_transform_set =
        _shadow_map_transform_sets[_frame_index];
    auto const &shadow_map_set    = _shadow_map_sets[_frame_index];

    _lit_color_pipeline.bind(cmd_buffer);
    _lit_color_pipeline.bind_descriptor_set(cmd_buffer, global_buffer_set);
    _lit_color_pipeline.bind_descriptor_set(cmd_buffer, light_props_set);
    _lit_color_pipeline.bind_descriptor_set(cmd_buffer,
                                            shadow_map_transform_set);
    _lit_color_pipeline.bind_descriptor_set(cmd_buffer, shadow_map_set);

    for(auto const &draw : _lit_color_draws) {
        _send_push_constants(_lit_color_pipeline, draw, frame_data);
        draw.mesh->draw_indexed(cmd_buffer);
    }

    _lit_color_draws.clear();
}

// =============================================================================
void Renderer::_execute_material_pipeline() {
    auto const &frame_data        = _frame_data[_frame_index];
    auto const &cmd_buffer        = frame_data.cmd_buffer();
    auto const &global_buffer_set = _global_data_sets[_frame_index];
    auto const &light_props_set   = _light_props_sets[_frame_index];

    _material_pipeline.bind(cmd_buffer);
    _material_pipeline.bind_descriptor_set(cmd_buffer, global_buffer_set);
    _material_pipeline.bind_descriptor_set(cmd_buffer, light_props_set);

    for(auto &[material_id, per_material] : _material_draws) {
        _material_pipeline.bind_descriptor_set(
            cmd_buffer,
            _material_sets[per_material.set_index]
        );

        for(auto const &draw : per_material.queue) {
            _send_push_constants(_material_pipeline, draw, frame_data);
            draw.mesh->draw_indexed(cmd_buffer);
        }

        per_material.queue.clear();
    }
}

// =============================================================================
void Renderer::_execute_shadow_map_pipeline() {
    auto const &frame_data = _frame_data[_frame_index];
    auto const &cmd_buffer = frame_data.cmd_buffer();
    auto const &shadow_map_transform_set =
        _shadow_map_transform_sets[_frame_index];

    _shadow_map_pipeline.bind(cmd_buffer);
    _shadow_map_pipeline.bind_descriptor_set(cmd_buffer,
                                             shadow_map_transform_set);

    for(auto const &draw : _lit_color_draws) {
        _send_push_constants(_shadow_map_pipeline, draw, frame_data);
        draw.mesh->draw_indexed(cmd_buffer);
    }

    for(auto &[material_id, per_material] : _material_draws) {
        for(auto const &draw : per_material.queue) {
            _send_push_constants(_shadow_map_pipeline, draw, frame_data);
            draw.mesh->draw_indexed(cmd_buffer);
        }
    }
}

// =============================================================================
void Renderer::_send_push_constants(Pipeline const &pipeline,
                                    DrawSubmission const &draw,
                                    FrameData const &frame_data)
{
    size_t offset = 0u;
    for(auto const& push_constant : draw.push_constants) {
        frame_data.cmd_buffer().native().pushConstants(
            pipeline.layout(),
            push_constant.stage_flags,
            static_cast<uint32_t>(offset),
            static_cast<uint32_t>(push_constant.size),
            push_constant.data
        );

        offset += push_constant.size;
    }
}

} // namespace vkl
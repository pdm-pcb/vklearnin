#include "Demo.hpp"

#include "vklearnin/rendering/swapchain/Swapchain.hpp"

static size_t constexpr CUBES_PER_SIDE = 5;
static float  constexpr CUBE_STEP      = 0.5f;

// =============================================================================
void Demo::submit_draws() {
    vkl::Camera::ViewProjMats vpm {
        .view = _persp_camera.view_matrix(),
        .proj = _persp_camera.proj_matrix()
    };

    vkl::BufferTools::update_buffer(
        _view_proj_ubos[vkl::Swapchain::image_index()],
        &vpm
    );

    _color_model_matrices.clear();
    _texture_model_matrices.clear();

    float xpos = std::floorf(CUBES_PER_SIDE * 0.5f) * -CUBE_STEP;
    for(size_t x = 0; x < CUBES_PER_SIDE; ++x) {
        float zpos = std::floorf(CUBES_PER_SIDE * 0.5f) * -CUBE_STEP;

        for(size_t z = 0; z < CUBES_PER_SIDE; ++z) {
            auto translate_top = vkl::math::translated(
                vkl::Mat4::identity,
                { xpos, CUBE_STEP, zpos }
            );

            auto rotate_top = vkl::math::rotated(
                vkl::Mat4::identity,
                {
                    vkl::Timekeeper::runtime() * 15.0f,
                    vkl::Timekeeper::runtime() * 20.0f,
                    0.0f
                }
            );

            auto translate_middle = vkl::math::translated(
                vkl::Mat4::identity,
                { xpos, 0.0f, zpos }
            );

            auto rotate_middle = vkl::math::rotated(
                vkl::Mat4::identity,
                {
                    0.0f,
                    vkl::Timekeeper::runtime() * 20.0f,
                    0.0f
                }
            );

            auto translate_bottom = vkl::math::translated(
                vkl::Mat4::identity,
                { xpos, -CUBE_STEP, zpos }
            );

            auto rotate_bottom = vkl::math::rotated(
                vkl::Mat4::identity,
                {
                    0.0f,
                    vkl::Timekeeper::runtime() * 20.0f,
                    vkl::Timekeeper::runtime() * 25.0f
                }
            );

            auto scale = vkl::math::scaled(
                vkl::Mat4::identity,
                { 0.1f, 0.1f, 0.1f, }
            );

            _texture_model_matrices.push_back(scale * rotate_top * translate_top);
            _color_model_matrices.push_back(scale * rotate_middle * translate_middle);
            _texture_model_matrices.push_back(scale * rotate_bottom * translate_bottom);

            zpos += CUBE_STEP;
        }

        xpos += CUBE_STEP;
    }

    for(auto& matrix : _color_model_matrices) {
        vkl::Renderer::submit(
            vkl::Renderer::PipelineType::COLOR,
            {
                .vertex_buffer = _color_cube.vertex_buffer().native(),
                .index_buffer  = _color_cube.index_buffer().native(),
                .index_count   = _color_cube.index_count(),
                .push_constants = {{
                    .stage_flags = vk::ShaderStageFlagBits::eVertex,
                    .size        = sizeof(vkl::Mat4),
                    .data        = &matrix,
                }}
            }
        );
    }

    for(uint32_t mat_idx = 0u; mat_idx < _texture_model_matrices.size(); ++mat_idx) {
        auto material = (mat_idx % 2 == 0) ?
                         _bricks_a.image().handle :
                         _bricks_b.image().handle;

        vkl::Renderer::submit(
            vkl::Renderer::PipelineType::TEXTURE,
            {
                .vertex_buffer = _texture_cube.vertex_buffer().native(),
                .index_buffer  = _texture_cube.index_buffer().native(),
                .index_count   = _texture_cube.index_count(),
                .material      = material,
                .push_constants = {{
                    .stage_flags = vk::ShaderStageFlagBits::eVertex,
                    .size        = sizeof(vkl::Mat4),
                    .data        = &_texture_model_matrices[mat_idx],
                }}
            }
        );
    }
}

// =============================================================================
void Demo::init() {
    _persp_camera.orient(
        { 0.0f, 0.0f, 3.0f },
        { 0.0f, 0.0f, 0.0f },
        { 0.0f, 1.0f, 0.0f }
    );
    _persp_camera.set_perspective(0.1f, 1000.0f, 45.0f);

    _view_proj_ubos.resize(vkl::RenderConfig::image_count);
    for(auto &ubo : _view_proj_ubos) {
        ubo.size = sizeof(vkl::Camera::ViewProjMats);
        vkl::BufferTools::create(
            ubo,
            vk::BufferUsageFlagBits::eUniformBuffer,
            (vk::MemoryPropertyFlagBits::eHostVisible |
             vk::MemoryPropertyFlagBits::eHostCoherent)
        );
    }

    vkl::Renderer::set_global_uniforms(_view_proj_ubos);

    _color_cube.init(
        1.0f,
        {{
            { 1.0f, 0.0f, 0.0f, 1.0f }, // Red
            { 0.0f, 1.0f, 0.0f, 1.0f }, // Green
            { 0.0f, 0.0f, 1.0f, 1.0f }, // Blue
            { 1.0f, 1.0f, 1.0f, 1.0f }, // White
            { 1.0f, 1.0f, 0.0f, 1.0f }, // Yellow
            { 0.0f, 1.0f, 1.0f, 1.0f }, // Cyan
            { 1.0f, 0.0f, 1.0f, 1.0f }, // Fuchsia
            { 0.0f, 0.0f, 0.0f, 1.0f }, // Black
        }}
    );
    _color_model_matrices.reserve(CUBES_PER_SIDE * CUBES_PER_SIDE);

    _texture_cube.init(1.0f, 1.0f);
    _texture_model_matrices.reserve(CUBES_PER_SIDE * CUBES_PER_SIDE * 2);

    _bricks_a.init_from_file("textures/brickwall017_d.jpg");
    _bricks_a.init_sampler(
        vk::Filter::eLinear,
        vk::Filter::eLinear,
        vk::SamplerAddressMode::eRepeat,
        vk::SamplerAddressMode::eRepeat
    );

    vkl::Renderer::add_material(_bricks_a.image());

    _bricks_b.init_from_file("textures/bricks082c_d.jpg");
    _bricks_b.init_sampler(
        vk::Filter::eLinear,
        vk::Filter::eLinear,
        vk::SamplerAddressMode::eRepeat,
        vk::SamplerAddressMode::eRepeat
    );

    vkl::Renderer::add_material(_bricks_b.image());
}

// =============================================================================
void Demo::shutdown() {
    _color_cube.shutdown();
    _texture_cube.shutdown();
    _bricks_a.shutdown();
    _bricks_b.shutdown();
}

// =============================================================================
Demo::Demo() :
    _persp_camera           { },
    _color_cube             { },
    _texture_cube           { },
    _color_model_matrices   { },
    _texture_model_matrices { }
{ }

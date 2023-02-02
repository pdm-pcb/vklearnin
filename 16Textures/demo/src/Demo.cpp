#include "Demo.hpp"

#include "vklearnin/rendering/swapchain/Swapchain.hpp"

static uint32_t constexpr CUBES_PER_SIDE = 5u;
static float    constexpr CUBE_STEP      = 0.5f;

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

    _model_matrices.clear();

    float xpos = (CUBES_PER_SIDE / 2) * -CUBE_STEP;
    for(uint32_t x = 0; x < CUBES_PER_SIDE; ++x) {
        float zpos = (CUBES_PER_SIDE / 2) * -CUBE_STEP;

        for(uint32_t z = 0; z < CUBES_PER_SIDE; ++z) {
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

            _model_matrices.push_back(scale * rotate_top * translate_top);
            _model_matrices.push_back(scale * rotate_bottom * translate_bottom);

            zpos += CUBE_STEP;
        }

        xpos += CUBE_STEP;
    }

    for(auto& matrix : _model_matrices) {
        vkl::Renderer::submit({
            .vertex_buffer = _cube.vertex_buffer().native(),
            .index_buffer  = _cube.index_buffer().native(),
            .index_count   = _cube.index_count(),

            .push_constants = {{
                .stage_flags = vk::ShaderStageFlagBits::eVertex,
                .size        = sizeof(vkl::Mat4),
                .data        = &matrix,
            }}
        });
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

    vkl::Renderer::add_ubo(_view_proj_ubos, vk::ShaderStageFlagBits::eVertex);

    _cube.init();
    _model_matrices.reserve(CUBES_PER_SIDE * CUBES_PER_SIDE * 2);

    _bricks.init_from_file("textures/brickwall017_d.jpg");
    _bricks.init_sampler(
        vk::Filter::eLinear,
        vk::Filter::eLinear,
        vk::SamplerAddressMode::eRepeat,
        vk::SamplerAddressMode::eRepeat
    );

    vkl::Renderer::add_texture2D(_bricks.image());
}

// =============================================================================
void Demo::shutdown() {
    _cube.shutdown();
    _bricks.shutdown();
}

// =============================================================================
Demo::Demo() :
    _persp_camera   { },
    _cube           { },
    _model_matrices { }
{ }

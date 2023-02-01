#include "Demo.hpp"

// =============================================================================
void Demo::submit_draws(vkl::Renderer &renderer) {
    _model_matrices.clear();

    float xpos = -1.0f;
    for(uint32_t x = 0; x < 5; ++x) {
        float zpos = -1.0f;

        for(uint32_t z = 0; z < 5; ++z) {
            auto translate_bottom = vkl::math::translated(
                vkl::Mat4::identity,
                { xpos, -0.5f, zpos }
            );

            auto translate_top = vkl::math::translated(
                vkl::Mat4::identity,
                { xpos, 0.5f, zpos }
            );

            auto rotate_top = vkl::math::rotated(
                vkl::Mat4::identity,
                {
                    vkl::Timekeeper::runtime() * 15.0f,
                    vkl::Timekeeper::runtime() * 20.0f,
                    0.0f
                }
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

            zpos += 0.5f;
        }

        xpos += 0.5f;
    }

    for(auto& matrix : _model_matrices) {
        renderer.submit({
            .mesh = _cube,
            .push_constants {
                {
                    .stage_flags = vk::ShaderStageFlagBits::eVertex,
                    .size = static_cast<uint32_t>(sizeof(vkl::Mat4)),
                    .data = &matrix,
                }
            }
        });
    }
}

// =============================================================================
void Demo::init() {
    _cube.init(
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

    _model_matrices.reserve(50);

    vkl::Texture2D loltest;
    loltest.init_from_file("textures/bricks082c_d.jpg");
    loltest.init_sampler(
        vk::Filter::eLinear,
        vk::Filter::eLinear,
        vk::SamplerAddressMode::eRepeat,
        vk::SamplerAddressMode::eRepeat
    );

    loltest.shutdown();
}

// =============================================================================
void Demo::shutdown() {
    _cube.shutdown();
}

// =============================================================================
Demo::Demo() :
    _cube           { },
    _model_matrices { }
{ }

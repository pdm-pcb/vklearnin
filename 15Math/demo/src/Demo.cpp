#include "Demo.hpp"

// =============================================================================
void Demo::submit_draws(vkl::Renderer &renderer) {
    renderer.submit({
        .mesh = _cube,
        .push_constants {
            {
                .stage_flags = vk::ShaderStageFlagBits::eVertex,
                .size = static_cast<uint32_t>(sizeof(vkl::Mat4)),
                .data = &_model_matrix,
            }
        }
    });
}

// =============================================================================
void Demo::init() {
    _xy_plane.init(
        1.0f,
        {{
            { 1.0f, 0.0f, 0.0f, 1.0f }, // Red
            { 0.0f, 1.0f, 0.0f, 1.0f }, // Green
            { 0.0f, 0.0f, 1.0f, 1.0f }, // Blue
            { 1.0f, 1.0f, 1.0f, 1.0f }, // White
        }}
    );

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

    _model_matrix = vkl::math::rotated(
        vkl::Mat4::identity,
        { 0.0f, 45.0f, 0.0f }
    );

    CONSOLE_ERROR("Model:\n{}", _model_matrix);
}

// =============================================================================
void Demo::shutdown() {
    _xy_plane.shutdown();
    _cube.shutdown();
}

// =============================================================================
Demo::Demo() :
    _xy_plane     { },
    _cube         { },
    _model_matrix { }
{ }

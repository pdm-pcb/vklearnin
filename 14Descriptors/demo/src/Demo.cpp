#include "Demo.hpp"

// =============================================================================
void Demo::submit_draws(vkl::Renderer &renderer) {
    // _vert_scale  = {
    //     std::abs(std::sinf(vkl::Timekeeper::runtime() * 0.25f)),
    //     std::abs(std::sinf(vkl::Timekeeper::runtime() * 0.33f))
    // };
    // _color_scale = {
    //     std::abs(std::sinf(vkl::Timekeeper::runtime() * 1.0f)),
    //     std::abs(std::sinf(vkl::Timekeeper::runtime() * 1.25f)),
    //     std::abs(std::sinf(vkl::Timekeeper::runtime() * 1.5f))
    // };

    renderer.submit({
        .mesh = _cube,
        .push_constants {
            {
                .stage_flags = vk::ShaderStageFlagBits::eVertex,
                .size = static_cast<uint32_t>(sizeof(_vert_scale)),
                .data = _vert_scale.data(),
            },
            {
                .stage_flags = vk::ShaderStageFlagBits::eFragment,
                .size = static_cast<uint32_t>(sizeof(_color_scale)),
                .data = _color_scale.data(),
            },
        }
    });
}

// =============================================================================
void Demo::init() {
    _xy_plane.init(
        1.5f,
        {{
            { 1.0f, 0.0f, 0.0f, 1.0f }, // Red
            { 0.0f, 1.0f, 0.0f, 1.0f }, // Green
            { 0.0f, 0.0f, 1.0f, 1.0f }, // Blue
            { 1.0f, 1.0f, 1.0f, 1.0f }, // White
        }}
    );

    _cube.init(
        0.5f,
        {{
            { 1.0f, 0.0f, 0.0f, 1.0f }, // Red
            { 0.0f, 1.0f, 0.0f, 1.0f }, // Green
            { 0.0f, 0.0f, 1.0f, 1.0f }, // Blue
            { 1.0f, 1.0f, 1.0f, 1.0f }, // White
            { 1.0f, 1.0f, 0.0f, 1.0f },
            { 0.0f, 1.0f, 1.0f, 1.0f },
            { 1.0f, 0.0f, 1.0f, 1.0f },
            { 0.0f, 0.0f, 0.0f, 1.0f },
        }}
    );
}

// =============================================================================
void Demo::shutdown() {
    _xy_plane.shutdown();
    _cube.shutdown();
}

// =============================================================================
Demo::Demo() :
    _xy_plane    { },
    _cube        { },
    _vert_scale  { 1.0f, 1.0f, 0.0f, 0.0f },
    _color_scale { 1.0f, 1.0f, 1.0f, 0.0f }
{ }

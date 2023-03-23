#include "Demo.hpp"

#include "vklearnin/rendering/swapchain/Swapchain.hpp"

vkl::Vec4 MATERIAL_COLOR { 0.15f, 0.65f, 0.25f, 256.0f };

vkl::Vec4 DIR_COLOR { 1.0f, 1.0f, 1.0f, 0.25f };
vkl::Vec4 DIR_POS   { 1.0f, 2.0f, 1.0f, 1.0f  };

vkl::Vec4 POINT_COLOR { 1.0f, 1.0f, 1.0f, 0.5f };
vkl::Vec4 POINT_POS   { -2.0f, 0.0f, 2.0f, 1.0f };

// =============================================================================
void Demo::update() {
    _camera.update();

    vkl::Renderer::update_global_buffer({
        .view_matrix = _camera.view_matrix(),
        .proj_matrix = _camera.proj_matrix()
    });

    // _light_props.dir.toward = vkl::math::normalize(DIR_POS);
    // _light_props.dir.color  = DIR_COLOR;

    // _light_props.point.position = POINT_POS;
    // _light_props.point.color    = POINT_COLOR;

    // vkl::BufferTools::update_buffer(
    //     _light_props_ubos[vkl::Swapchain::image_index()],
    //     &_light_props
    // );

    _cube_matrix = vkl::math::rotate(
        vkl::Mat4::identity,
        vkl::Timekeeper::run_time() * 20.0f,
        { 0.0f, 20.0f, 0.0f, 0.0f }
    );
}

// =============================================================================
void Demo::submit_draws() {
    vkl::Renderer::submit_draw(_lamp_mesh, _lamp_matrix);
    vkl::Renderer::submit_draw(_cube_mesh, _cube_texture, _cube_matrix);
    vkl::Renderer::submit_draw(_floor_mesh, _floor_texture, _floor_matrix);
}

// =============================================================================
void Demo::init() {
    _init_camera();
    _init_meshes();
    _init_model_matrices();
    _init_textures();
    _init_lights();
}

// =============================================================================
void Demo::shutdown() {
    _lamp_mesh.shutdown();
    _cube_mesh.shutdown();
    _wall_mesh.shutdown();
    _floor_mesh.shutdown();

    _cube_texture.shutdown();
    _floor_texture.shutdown();
}

// =============================================================================
void Demo::_init_camera() {
    auto const position = 8.0f * vkl::Vec4::unit_z;
    auto const forward  = -1.0f * vkl::Vec4::unit_z;
    _camera.init(position, forward);
    _camera.set_perspective(0.1f, 1000.0f, 45.0f);
}

// =============================================================================
void Demo::_init_meshes() {
    _lamp_mesh.init(0.025f, POINT_COLOR);

    // _cube_mesh.init(
    //     1.0f,
    //     {{
    //         { 1.0f, 0.0f, 0.0f, 1.0f }, // Red
    //         { 0.0f, 1.0f, 0.0f, 1.0f }, // Green
    //         { 0.0f, 0.0f, 1.0f, 1.0f }, // Blue
    //         { 1.0f, 1.0f, 1.0f, 1.0f }, // White

    //         { 1.0f, 1.0f, 0.0f, 1.0f }, // Yellow
    //         { 0.0f, 1.0f, 1.0f, 1.0f }, // Cyan
    //         { 1.0f, 0.0f, 1.0f, 1.0f }, // Fuchsia
    //         { 0.0f, 0.0f, 0.0f, 1.0f }, // Black
    //     }}
    // );

    _cube_mesh.init(1.0f, 1.0f);

    _floor_mesh.init(100.0f, 100.0f);
}

// =============================================================================
void Demo::_init_model_matrices() {
    _lamp_matrix = vkl::math::translate(
        vkl::Mat4::identity,
        POINT_POS
    );

    _cube_matrix = vkl::Mat4::identity;

    _floor_matrix = vkl::math::translate(
        vkl::Mat4::identity,
        { 0.0f, -3.0f, 0.0f, 1.0f }
    );
}

// =============================================================================
void Demo::_init_textures() {
    _cube_texture.texture_from_file("textures/brickwall017_d.jpg");
    _cube_texture.init_sampler(
        vk::Filter::eLinear,
        vk::Filter::eLinear,
        vk::SamplerMipmapMode::eLinear,
        vk::SamplerAddressMode::eRepeat,
        vk::SamplerAddressMode::eRepeat
    );

    _floor_texture.texture_from_file("textures/woodfloor_051_d.jpg");
    _floor_texture.init_sampler(
        vk::Filter::eLinear,
        vk::Filter::eLinear,
        vk::SamplerMipmapMode::eLinear,
        vk::SamplerAddressMode::eRepeat,
        vk::SamplerAddressMode::eRepeat
    );

    vkl::Renderer::set_flat_textures({
        _cube_texture,
        _floor_texture
    });

    vkl::Renderer::set_skybox_texture({{
        "textures/skybox/belfast_sunset/px.png",
        "textures/skybox/belfast_sunset/nx.png",
        "textures/skybox/belfast_sunset/py.png",
        "textures/skybox/belfast_sunset/ny.png",
        "textures/skybox/belfast_sunset/pz.png",
        "textures/skybox/belfast_sunset/nz.png",
    }});
}

// =============================================================================
void Demo::_init_lights() {
    // _light_props_ubos.resize(vkl::RenderConfig::swapchain_image_count);
    // CONSOLE_TRACE("Allocating light UBOs");
    // for(auto &ubo : _light_props_ubos) {
    //     ubo.size = sizeof(vkl::LightProps);
    //     vkl::BufferTools::create(
    //         ubo,
    //         vk::BufferUsageFlagBits::eUniformBuffer,
    //         (vk::MemoryPropertyFlagBits::eHostVisible |
    //          vk::MemoryPropertyFlagBits::eHostCoherent)
    //     );
    // }

    // vkl::Renderer::set_light_ubos(_light_props_ubos);
}

// =============================================================================
Demo::Demo() :
    _camera { },

    _lamp_mesh   { },
    _cube_mesh   { },
    _wall_mesh   { },
    _floor_mesh  { },

    _lamp_matrix  { },
    _cube_matrix  { },
    _wall_matrix  { },
    _floor_matrix { },

    _cube_texture   { },
    _wall_texture   { },
    _floor_texture  { }
{ }

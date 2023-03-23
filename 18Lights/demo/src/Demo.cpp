#include "Demo.hpp"

#include "vklearnin/rendering/swapchain/Swapchain.hpp"

vkl::Vec4 CUBE_COLOR  { 0.15f, 0.65f, 0.25f, 256.0f };
vkl::Vec4 FLOOR_COLOR { 0.15f, 0.25f, 0.65f, 256.0f };
vkl::Vec4 WALL_COLOR  { 0.65f, 0.25f, 0.15f, 256.0f };

vkl::Vec4 DIR_COLOR { 1.0f, 1.0f, 1.0f, 0.25f };
vkl::Vec4 DIR_POS   { 1.0f, 2.0f, 1.0f, 1.0f  };

vkl::Vec4 POINT_COLOR { 1.0f, 1.0f, 1.0f, 1.0f };
vkl::Vec4 POINT_POS   { -2.0f, 0.0f, 2.0f, 1.0f };

// =============================================================================
void Demo::update() {
    _camera.update();

    static vkl::Renderer::GlobalBuffer global_buffer;
    global_buffer.view_matrix = _camera.view_matrix();
    global_buffer.proj_matrix = _camera.proj_matrix();

    vkl::Renderer::update_global_buffer(global_buffer);

    static vkl::LightProps light_props;
    light_props.dir.toward     = vkl::math::normalize(DIR_POS);
    light_props.dir.color      = DIR_COLOR;
    light_props.point.position = POINT_POS;
    light_props.point.color    = POINT_COLOR;

    vkl::Renderer::update_light_props(light_props);

    _cube_matrix = vkl::math::rotate(
        vkl::Mat4::identity,
        vkl::Timekeeper::run_time() * 20.0f,
        vkl::Vec4::unit_y
    );
}

// =============================================================================
void Demo::submit_draws() {
    vkl::Renderer::submit_draw(_lamp_mesh, _lamp_matrix);
    vkl::Renderer::submit_draw(_cube_mesh, _cube_matrix);
    vkl::Renderer::submit_draw(_floor_mesh, _floor_matrix);
    vkl::Renderer::submit_draw(_wall_mesh, _wall_matrix_a);
    vkl::Renderer::submit_draw(_wall_mesh, _wall_matrix_b);
}

// =============================================================================
void Demo::init() {
    _init_camera();
    _init_meshes();
    _init_model_matrices();
    _init_textures();
}

// =============================================================================
void Demo::shutdown() {
    _lamp_mesh.shutdown();
    _cube_mesh.shutdown();
    _floor_mesh.shutdown();
    _wall_mesh.shutdown();

    // TODO: material can be as smart as Texture2D
    _cube_material.diffuse.shutdown();
    _floor_material.diffuse.shutdown();
    _wall_material.diffuse.shutdown();
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
    _cube_mesh.init(1.0f, CUBE_COLOR);
    _floor_mesh.init(10.0f, FLOOR_COLOR);
    _wall_mesh.init(10.0f, WALL_COLOR);
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

    _wall_matrix_a = vkl::math::translate(
        vkl::Mat4::identity,
        { 0.0f, -3.0f, -10.0f, 1.0f }
    );

    _wall_matrix_b = vkl::math::translate(
        vkl::Mat4::identity,
        { 10.0f, -3.0f, 0.0f, 1.0f }
    ) * vkl::math::rotate(
        vkl::Mat4::identity,
        -90.0f,
        vkl::Vec4::unit_y
    );
}

// =============================================================================
void Demo::_init_textures() {
    _cube_material.diffuse.texture_from_file("textures/brickwall017_d.jpg");
    _cube_material.diffuse.init_sampler(
        vk::Filter::eLinear,
        vk::Filter::eLinear,
        vk::SamplerMipmapMode::eLinear,
        vk::SamplerAddressMode::eRepeat,
        vk::SamplerAddressMode::eRepeat
    );

    _floor_material.diffuse.texture_from_file("textures/woodfloor_051_d.jpg");
    _floor_material.diffuse.init_sampler(
        vk::Filter::eLinear,
        vk::Filter::eLinear,
        vk::SamplerMipmapMode::eLinear,
        vk::SamplerAddressMode::eRepeat,
        vk::SamplerAddressMode::eRepeat
    );

    vkl::Renderer::set_skybox_texture({{
        "textures/skybox/belfast_sunset/px.png",
        "textures/skybox/belfast_sunset/nx.png",
        "textures/skybox/belfast_sunset/py.png",
        "textures/skybox/belfast_sunset/ny.png",
        "textures/skybox/belfast_sunset/pz.png",
        "textures/skybox/belfast_sunset/nz.png",
    }});

    vkl::Renderer::set_materials({
        _cube_material,
        _floor_material,
    });
}

// =============================================================================
Demo::Demo()
{ }

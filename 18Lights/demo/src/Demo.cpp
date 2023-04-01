#include "Demo.hpp"

#include "vklearnin/rendering/swapchain/Swapchain.hpp"

vkl::Vec4 CUBE_COLOR  { 0.15f, 0.65f, 0.25f, 256.0f };
vkl::Vec4 FLOOR_COLOR { 0.15f, 0.25f, 0.65f, 256.0f };
vkl::Vec4 WALL_COLOR  { 0.85f, 0.45f, 0.35f, 256.0f };

vkl::Vec4 DIR_POS   { 2.0f, 4.0f, 2.0f, 1.0f  };
vkl::Vec4 DIR_COLOR { vkl::color::sunlight, 1.0f };

vkl::Vec4 POINT_POS   { 0.0f, 0.0f, 0.0f, 1.0f };
vkl::Vec4 POINT_COLOR { vkl::color::sunlight, 2.0f };

vkl::Vec4 SPOT_POS   { -5.0f, 1.0f, 5.0f, 1.0f };
vkl::Vec4 SPOT_FWD = -vkl::math::normalize(SPOT_POS);
vkl::Vec4 SPOT_COLOR { vkl::color::sunlight, 20.0f };

float ROT_FACT = 20.0f;

// =============================================================================
void Demo::update() {
    _camera.update();

    static vkl::Renderer::CameraData camera_data;
    camera_data.view_matrix = _camera.view_matrix();
    camera_data.proj_matrix = _camera.proj_matrix();

    vkl::Renderer::update_camera_data(camera_data);

    _dir_lamp_matrix   = vkl::math::translate(vkl::Mat4::identity, DIR_POS);
    _point_lamp_matrix = vkl::math::translate(vkl::Mat4::identity, POINT_POS);
    _spot_lamp_matrix  = vkl::math::translate(vkl::Mat4::identity, SPOT_POS);

    _cube_matrix_a =
        vkl::math::rotate(
            vkl::math::translate(
                vkl::Mat4::identity,
                -vkl::Vec4::unit_z * 2.0f
            ),
            vkl::Timekeeper::run_time() * ROT_FACT,
            vkl::Vec4::unit_y
        );

    _cube_matrix_b =
        vkl::math::scale(
            vkl::math::rotate(
                vkl::math::translate(
                    vkl::Mat4::identity,
                    vkl::Vec4::unit_z
                ),
                vkl::Timekeeper::run_time() * ROT_FACT,
                vkl::Vec4::unit_x
            ),
            0.25f
        );

    _lights.dir[0].position = DIR_POS;
    _lights.dir[0].color    = DIR_COLOR;

    _lights.point[0].position = POINT_POS;
    _lights.point[0].color    = POINT_COLOR;

    _lights.spot[0].position = SPOT_POS;
    _lights.spot[0].color    = SPOT_COLOR;
    _lights.spot[0].forward  = SPOT_FWD;

    _light_props.dir_count   = _lights.dir.size();
    _light_props.point_count = _lights.dir.size();
    _light_props.spot_count  = _lights.dir.size();

    vkl::Renderer::update_scene_lights(_lights, _light_props);
}

// =============================================================================
void Demo::submit_draws() {
    vkl::Renderer::submit_draw_flat(_dir_lamp_mesh,   _dir_lamp_matrix);
    vkl::Renderer::submit_draw_flat(_point_lamp_mesh, _point_lamp_matrix);
    vkl::Renderer::submit_draw_flat(_spot_lamp_mesh,  _spot_lamp_matrix);

    vkl::Renderer::submit_draw_lit(_cube_mesh, _cube_matrix_a);
    vkl::Renderer::submit_draw_lit(_cube_mesh, _cube_matrix_b);

    vkl::Renderer::submit_draw_lit(_floor_mesh, _floor_matrix);
    vkl::Renderer::submit_draw_lit(_wall_mesh,  _wall_matrix_a);
    vkl::Renderer::submit_draw_lit(_wall_mesh,  _wall_matrix_b);
}

// =============================================================================
void Demo::init() {
    _init_camera();
    _init_meshes();
    _init_model_matrices();
    _init_textures();

    _lights.dir.emplace_back();
    _lights.point.emplace_back();
    _lights.spot.emplace_back();
}

// =============================================================================
void Demo::shutdown() {
    _dir_lamp_mesh.destroy();
    _point_lamp_mesh.destroy();
    _spot_lamp_mesh.destroy();
    _cube_mesh.destroy();
    _floor_mesh.destroy();
    _wall_mesh.destroy();

    // TODO: material can be as smart as Texture2D
    _cube_material.diffuse.destroy();
    _floor_material.diffuse.destroy();
    _wall_material.diffuse.destroy();
}

// =============================================================================
void Demo::_init_camera() {
    auto const position = 8.0f * vkl::Vec4::unit_z;
    auto const forward  = -1.0f * vkl::Vec4::unit_z;
    _camera.init(position, forward);
    // _camera.set_orthographic(0.1f, 1000.0f);
    _camera.set_perspective(0.1f, 45.0f);
}

// =============================================================================
void Demo::_init_meshes() {
    _dir_lamp_mesh.init(0.025f, DIR_COLOR);
    _point_lamp_mesh.init(0.025f, POINT_COLOR);
    _spot_lamp_mesh.init(0.025f, SPOT_COLOR);

    _cube_mesh.init(1.0f, CUBE_COLOR);
    _floor_mesh.init(10.0f, FLOOR_COLOR);
    _wall_mesh.init(10.0f, WALL_COLOR);
}

// =============================================================================
void Demo::_init_model_matrices() {
    _floor_matrix = vkl::math::rotate(
        vkl::math::translate(
            vkl::Mat4::identity,
            { 0.0f, -1.0f, 0.0f, 1.0f }
        ),
        -90.0f,
        vkl::Vec4::unit_x
    );

    _wall_matrix_a = vkl::math::translate(
        vkl::Mat4::identity,
        { 0.0f, -1.0f, -10.0f, 1.0f }
    );

    _wall_matrix_b = vkl::math::translate(
        vkl::Mat4::identity,
        { 10.0f, -1.0f, 0.0f, 1.0f }
    ) * vkl::math::rotate(
        vkl::Mat4::identity,
        -90.0f,
        vkl::Vec4::unit_y
    );
}

// =============================================================================
void Demo::_init_textures() {
    _cube_material.diffuse.texture_from_file("textures/brickwall017_d.jpg");
    _cube_material.diffuse.create_sampler(
        vk::Filter::eLinear,
        vk::Filter::eLinear,
        vk::SamplerMipmapMode::eLinear,
        vk::SamplerAddressMode::eRepeat,
        vk::SamplerAddressMode::eRepeat,
        VK_FALSE,
        vk::CompareOp::eAlways
    );
    _cube_material.diffuse.generate_mipmap(vk::Filter::eLinear);

    _floor_material.diffuse.texture_from_file("textures/woodfloor_051_d.jpg");
    _floor_material.diffuse.create_sampler(
        vk::Filter::eLinear,
        vk::Filter::eLinear,
        vk::SamplerMipmapMode::eLinear,
        vk::SamplerAddressMode::eRepeat,
        vk::SamplerAddressMode::eRepeat,
        VK_FALSE,
        vk::CompareOp::eAlways
    );
    _floor_material.diffuse.generate_mipmap(vk::Filter::eLinear);

    vkl::Renderer::set_skybox_texture({{
        "textures/skybox/belfast_sunset/px.png",
        "textures/skybox/belfast_sunset/nx.png",
        "textures/skybox/belfast_sunset/py.png",
        "textures/skybox/belfast_sunset/ny.png",
        "textures/skybox/belfast_sunset/pz.png",
        "textures/skybox/belfast_sunset/nz.png",
    }});

    // vkl::Renderer::set_materials({
    //     _cube_material,
    //     _floor_material,
    // });
}

// =============================================================================
Demo::Demo()
{ }

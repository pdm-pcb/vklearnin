#include "Demo.hpp"

#include "vklearnin/rendering/swapchain/Swapchain.hpp"

vkl::Vec4 CUBE_COLOR  { vkl::color::american_green, 256.0f };
vkl::Vec4 FLOOR_COLOR { vkl::color::denim_blue, 256.0f };
vkl::Vec4 WALL_COLOR  { vkl::color::terra_cotta, 256.0f };

vkl::Vec4 DIR_POS_A   { 6.0f, 6.0f, 6.0f, 1.0f };
vkl::Vec4 DIR_COLOR_A { vkl::color::sunlight, 1.0f };

vkl::Vec4 DIR_POS_B   { -6.0f, 6.0f, -6.0f, 1.0f };
vkl::Vec4 DIR_COLOR_B { vkl::color::sunlight, 1.0f };

vkl::Vec4 POINT_POS_A { -5.0f, -0.5f, 0.0f, 5.0f };
vkl::Vec4 POINT_COLOR_A { vkl::color::red, 2.0f };

vkl::Vec4 POINT_POS_B { 5.0f, -0.5f, 0.0f, 5.0f };
vkl::Vec4 POINT_COLOR_B { vkl::color::blue, 2.0f };

vkl::Vec4 SPOT_POS_A { -10.0f, 0.0f, 10.0f, 1.0f };
vkl::Vec4 SPOT_COLOR_A { vkl::color::sunlight, 40.0f };
vkl::Vec4 SPOT_FWD_A = -vkl::math::normalize(SPOT_POS_A);

vkl::Vec4 SPOT_POS_B { 5.0f, 0.0f, 5.0f, 1.0f };
vkl::Vec4 SPOT_COLOR_B { vkl::color::sunlight, 10.0f };
vkl::Vec4 SPOT_FWD_B = -vkl::math::normalize(SPOT_POS_B);

float ROT_FACT = 20.0f;

// =============================================================================
void Demo::update() {
    _camera.update();

    static vkl::Renderer::CameraData camera_data;
    camera_data.view_matrix = _camera.view_matrix();
    camera_data.proj_matrix = _camera.proj_matrix();

    vkl::Renderer::update_camera_data(camera_data);

    _dir_lamp_matrix_a   = vkl::math::translate(vkl::Mat4::identity, DIR_POS_A);
    _dir_lamp_matrix_b   = vkl::math::translate(vkl::Mat4::identity, DIR_POS_B);
    _point_lamp_matrix_a = vkl::math::translate(vkl::Mat4::identity, POINT_POS_A);
    _point_lamp_matrix_b = vkl::math::translate(vkl::Mat4::identity, POINT_POS_B);
    _spot_lamp_matrix_a  = vkl::math::translate(vkl::Mat4::identity, SPOT_POS_A);
    _spot_lamp_matrix_b  = vkl::math::translate(vkl::Mat4::identity, SPOT_POS_B);

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

    _lights.dir.clear();
    _lights.dir.emplace_back(vkl::DirectionalLight {
        .position = DIR_POS_A,
        .color    = DIR_COLOR_A,
    });
    // _lights.dir.emplace_back(vkl::DirectionalLight {
    //     .position = DIR_POS_B,
    //     .color    = DIR_COLOR_B,
    // });

    _lights.point.clear();
    _lights.point.emplace_back(vkl::PointLight {
        .position = POINT_POS_A,
        .color    = POINT_COLOR_A,
    });
    _lights.point.emplace_back(vkl::PointLight {
        .position = POINT_POS_B,
        .color    = POINT_COLOR_B,
    });

    _lights.spot.clear();
    _lights.spot.emplace_back(vkl::SpotLight {
        .position = SPOT_POS_A,
        .color    = SPOT_COLOR_A,
        .forward  = SPOT_FWD_A,
    });
    // _lights.spot.emplace_back(vkl::SpotLight {
    //     .position = SPOT_POS_B,
    //     .color    = SPOT_COLOR_B,
    //     .forward  = SPOT_FWD_B,
    // });

    _light_props.dir_count   = _lights.dir.size();
    _light_props.point_count = _lights.point.size();
    _light_props.spot_count  = _lights.spot.size();

    vkl::Renderer::update_scene_lights(_lights, _light_props);
}

// =============================================================================
void Demo::submit_draws() {
    // vkl::Renderer::submit_draw_flat(_dir_lamp_mesh_a, _dir_lamp_matrix_a);
    // vkl::Renderer::submit_draw_flat(_dir_lamp_mesh_b, _dir_lamp_matrix_b);

    // vkl::Renderer::submit_draw_flat(_point_lamp_mesh_a, _point_lamp_matrix_a);

    // vkl::Renderer::submit_draw_flat(_spot_lamp_mesh_a, _spot_lamp_matrix_a);
    // vkl::Renderer::submit_draw_flat(_spot_lamp_mesh_b, _spot_lamp_matrix_b);

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
}

// =============================================================================
void Demo::shutdown() {
    _dir_lamp_mesh_a.destroy();
    _dir_lamp_mesh_b.destroy();
    _point_lamp_mesh_a.destroy();
    _point_lamp_mesh_b.destroy();
    _spot_lamp_mesh_a.destroy();
    _spot_lamp_mesh_b.destroy();
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
    _dir_lamp_mesh_a.init(0.025f, DIR_COLOR_A);
    _dir_lamp_mesh_b.init(0.025f, DIR_COLOR_B);
    _point_lamp_mesh_a.init(0.025f, POINT_COLOR_A);
    _point_lamp_mesh_b.init(0.025f, POINT_COLOR_B);
    _spot_lamp_mesh_a.init(0.025f, SPOT_COLOR_A);
    _spot_lamp_mesh_b.init(0.025f, SPOT_COLOR_B);

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

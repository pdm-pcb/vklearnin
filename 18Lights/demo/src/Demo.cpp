#include "Demo.hpp"

#include "vklearnin/rendering/swapchain/Swapchain.hpp"

vkl::Vec4 MATERIAL_COLOR { 0.15f, 0.65f, 0.25f, 256.0f };

vkl::Vec4 DIR_COLOR { 1.0f, 1.0f, 1.0f, 0.25f };
vkl::Vec4 DIR_POS   { 1.0f, 2.0f, 1.0f, 1.0f  };

vkl::Vec4 POINT_COLOR { 1.0f, 1.0f, 1.0f, 0.5f };
vkl::Vec4 POINT_POS   { -2.0f, 0.0f, 2.0f, 1.0f };

// =============================================================================
void Demo::update() {
    auto const cos_yaw   = std::cos(vkl::math::radians(_cam_data.yaw));
    auto const sin_yaw   = std::sin(vkl::math::radians(_cam_data.yaw));
    auto const cos_pitch = std::cos(vkl::math::radians(_cam_data.pitch));
    auto const sin_pitch = std::sin(vkl::math::radians(_cam_data.pitch));

    _cam_data.forward.x = cos_yaw * cos_pitch;
    _cam_data.forward.y = sin_pitch;
    _cam_data.forward.z = sin_yaw * cos_pitch;

    _cam_data.forward = vkl::math::normalize(_cam_data.forward);
    _cam_data.side = vkl::math::normalize(
        vkl::math::cross(_cam_data.forward, vkl::Vec4::unit_y)
    );
    _cam_data.up = vkl::math::cross(_cam_data.side, _cam_data.forward);

    auto kb_speed = _cam_data.kb_speed * vkl::Timekeeper::frame_time();
    if(_kb.lshift) {
        kb_speed *= 2.0f;
    }

    if(_kb.w)          { _cam_data.pos += _cam_data.forward * kb_speed; }
    else if(_kb.s)     { _cam_data.pos -= _cam_data.forward * kb_speed; }
    if(_kb.a)          { _cam_data.pos -= _cam_data.side    * kb_speed; }
    else if(_kb.d)     { _cam_data.pos += _cam_data.side    * kb_speed; }
    if(_kb.space)      { _cam_data.pos += _cam_data.up      * kb_speed; }
    else if(_kb.lctrl) { _cam_data.pos -= _cam_data.up      * kb_speed; }

    _persp_camera.orient(
        _cam_data.pos,
        _cam_data.forward,
        _cam_data.side,
        _cam_data.up
    );

    vkl::Renderer::update_global_buffer({
        .view_matrix = _persp_camera.view_matrix(),
        .proj_matrix = _persp_camera.proj_matrix()
    });

    // _light_props.dir.toward = vkl::math::normalize(DIR_POS);
    // _light_props.dir.color  = DIR_COLOR;

    // _light_props.point.position = POINT_POS;
    // _light_props.point.color    = POINT_COLOR;

    // vkl::BufferTools::update_buffer(
    //     _light_props_ubos[vkl::Swapchain::image_index()],
    //     &_light_props
    // );
}

// =============================================================================
void Demo::submit_draws() {
    // vkl::Renderer::submit(
    //     vkl::DrawSubmission<vkl::VertexFlatColor> {
    //         .mesh = &_lamp_mesh,
    //         .push_constants = {{
    //                 .stage_flags = vk::ShaderStageFlagBits::eAll,
    //                 .size        = sizeof(vkl::Mat4),
    //                 .data        = &_lamp_matrix,
    //             }}
    //     }
    // );

    // _cube_matrix = vkl::math::rotate(
    //     vkl::Mat4::identity,
    //     vkl::Timekeeper::run_time() * 20.0f,
    //     { 0.0f, 20.0f, 0.0f, 0.0f }
    // );

    // vkl::Renderer::submit(
    //     vkl::DrawSubmission<vkl::VertexLitColor> {
    //         .mesh = &_cube_mesh,
    //         .push_constants = {{
    //                 .stage_flags = vk::ShaderStageFlagBits::eAll,
    //                 .size        = sizeof(vkl::Mat4),
    //                 .data        = &_cube_matrix,
    //             }}
    //     }
    // );

    // vkl::Renderer::submit(
    //     vkl::DrawSubmission<vkl::VertexFlatTexture> {
    //         .mesh     = &_floor_mesh,
    //         .material = &_floor_texture,
    //         .push_constants = {{
    //                 .stage_flags = vk::ShaderStageFlagBits::eAll,
    //                 .size        = sizeof(vkl::Mat4),
    //                 .data        = &_floor_matrix,
    //             }}
    //     }
    // );
}

// =============================================================================
void Demo::init() {
    _subscribe_to_events();
    _init_camera();
    _init_meshes();
    _init_trs_matrices();
    _init_textures();
    _init_lights();
}

// =============================================================================
void Demo::shutdown() {
    _lamp_mesh.shutdown();
    _cube_mesh.shutdown();
    _floor_mesh.shutdown();

    _cube_texture.shutdown();
    _floor_texture.shutdown();
}

// =============================================================================
void Demo::on_key_press(const vkl::KeyPressEvent &event) {
    switch(event.code) {
        case vkl::KB_W :
            _kb.w = true;
            _kb.s = false;
            break;
        case vkl::KB_A :
            _kb.a = true;
            _kb.d = false;
            break;
        case vkl::KB_S :
            _kb.s = true;
            _kb.w = false;
            break;
        case vkl::KB_D :
            _kb.d = true;
            _kb.a = false;
            break;
        case vkl::KB_LEFT_CTRL:
            _kb.lctrl = true;
            _kb.space = false;
            break;
        case vkl::KB_SPACE:
            _kb.space = true;
            _kb.lctrl = false;
            break;

        case vkl::KB_LEFT_SHIFT: _kb.lshift = true; break;

        default: break;
    }
}

// =============================================================================
void Demo::on_key_release(const vkl::KeyReleaseEvent &event) {
    switch(event.code) {
        case vkl::KB_W          : _kb.w      = false; break;
        case vkl::KB_A          : _kb.a      = false; break;
        case vkl::KB_S          : _kb.s      = false; break;
        case vkl::KB_D          : _kb.d      = false; break;
        case vkl::KB_LEFT_CTRL  : _kb.lctrl  = false; break;
        case vkl::KB_SPACE      : _kb.space  = false; break;
        case vkl::KB_LEFT_SHIFT : _kb.lshift = false; break;

        default: break;
    }
}

// =============================================================================
void Demo::on_mouse_move(const vkl::MouseMoveEvent &event) {
    _cam_data.pitch += -event.y_offset * _cam_data.mouse_speed;
    _cam_data.yaw   += event.x_offset * _cam_data.mouse_speed;

    if(_cam_data.pitch > 89.9f)       { _cam_data.pitch = 89.9f;  }
    else if(_cam_data.pitch < -89.9f) { _cam_data.pitch = -89.9f; }
}

// =============================================================================
void Demo::on_mouse_button_press(const vkl::MouseButtonPressEvent &event) {
    switch(event.code) {
        case vkl::MOUSE_BUTTON_LEFT    : CONSOLE_INFO("LMB down");      break;
        case vkl::MOUSE_BUTTON_RIGHT   : CONSOLE_INFO("RMB down");      break;
        case vkl::MOUSE_BUTTON_MIDDLE  : CONSOLE_INFO("MMB down");      break;
        case vkl::MOUSE_BUTTON_BACK    : CONSOLE_INFO("MBack down");    break;
        case vkl::MOUSE_BUTTON_FORWARD : CONSOLE_INFO("MForward down"); break;
    }
}

// =============================================================================
void Demo::on_mouse_button_release(const vkl::MouseButtonReleaseEvent &event) {
    switch(event.code) {
        case vkl::MOUSE_BUTTON_LEFT    : CONSOLE_INFO("LMB up");      break;
        case vkl::MOUSE_BUTTON_RIGHT   : CONSOLE_INFO("RMB up");      break;
        case vkl::MOUSE_BUTTON_MIDDLE  : CONSOLE_INFO("MMB up");      break;
        case vkl::MOUSE_BUTTON_BACK    : CONSOLE_INFO("MBack up");    break;
        case vkl::MOUSE_BUTTON_FORWARD : CONSOLE_INFO("MForward up"); break;
    }
}

// =============================================================================
void Demo::on_mouse_scroll(const vkl::MouseScrollEvent &event) {
    CONSOLE_INFO(
        "Mouse scroll vert: {}, horiz: {}",
        event.vert_offset,
        event.horiz_offset
    );
}

// =============================================================================
void Demo::_subscribe_to_events() {
vkl::EventBroker::subscribe<vkl::KeyPressEvent>(
        this,
        &Demo::on_key_press
    );

    vkl::EventBroker::subscribe<vkl::KeyReleaseEvent>(
        this,
        &Demo::on_key_release
    );

    vkl::EventBroker::subscribe<vkl::MouseMoveEvent>(
        this,
        &Demo::on_mouse_move
    );

    vkl::EventBroker::subscribe<vkl::MouseButtonPressEvent>(
        this,
        &Demo::on_mouse_button_press
    );

    vkl::EventBroker::subscribe<vkl::MouseButtonReleaseEvent>(
        this,
        &Demo::on_mouse_button_release
    );

    vkl::EventBroker::subscribe<vkl::MouseScrollEvent>(
        this,
        &Demo::on_mouse_scroll
    );
}

// =============================================================================
void Demo::_init_camera() {
    _persp_camera.set_perspective(0.1f, 1000.0f, 45.0f);

    _cam_data.pos     =  8.0f * vkl::Vec4::unit_z;
    _cam_data.forward = -1.0f * vkl::Vec4::unit_z;
}

// =============================================================================
void Demo::_init_meshes() {
    _lamp_mesh.init(
        0.025f,
        {{
            POINT_COLOR, POINT_COLOR, POINT_COLOR, POINT_COLOR,
            POINT_COLOR, POINT_COLOR, POINT_COLOR, POINT_COLOR,
        }}
    );
    _cube_mesh.init(1.0f, MATERIAL_COLOR);
    _floor_mesh.init(100.0f, 100.0f);
}

// =============================================================================
void Demo::_init_trs_matrices() {
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
    // _cube_texture.texture_from_file("textures/brickwall017_d.jpg");
    // _cube_texture.init_sampler(
    //     vk::Filter::eLinear,
    //     vk::Filter::eLinear,
    //     vk::SamplerMipmapMode::eLinear,
    //     vk::SamplerAddressMode::eRepeat,
    //     vk::SamplerAddressMode::eRepeat
    // );
    // vkl::Renderer::add_flat_texture(_cube_texture);

    // _floor_texture.texture_from_file("textures/woodfloor_051_d.jpg");
    // _floor_texture.init_sampler(
    //     vk::Filter::eLinear,
    //     vk::Filter::eLinear,
    //     vk::SamplerMipmapMode::eLinear,
    //     vk::SamplerAddressMode::eRepeat,
    //     vk::SamplerAddressMode::eRepeat
    // );
    // vkl::Renderer::add_flat_texture(_floor_texture);
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
    _kb { },

    _persp_camera { },
    _cam_data     { },

    _lamp_mesh { },
    _cube_mesh   { },
    _floor_mesh  { },

    _lamp_matrix { },
    _cube_matrix   { },
    _floor_matrix  { },

    _cube_texture   { },
    _floor_texture  { }
{ }

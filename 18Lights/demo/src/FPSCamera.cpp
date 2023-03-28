#include "vklearnin/vklearnin.hpp"
#include "FPSCamera.hpp"

// =============================================================================
void FPSCamera::update() {
    auto const cos_yaw   = std::cos(vkl::math::radians(_state.yaw));
    auto const sin_yaw   = std::sin(vkl::math::radians(_state.yaw));
    auto const cos_pitch = std::cos(vkl::math::radians(_state.pitch));
    auto const sin_pitch = std::sin(vkl::math::radians(_state.pitch));

    _state.forward.x = cos_yaw * cos_pitch;
    _state.forward.y = sin_pitch;
    _state.forward.z = sin_yaw * cos_pitch;

    _state.forward = vkl::math::normalize(_state.forward);
    _state.side = vkl::math::normalize(
        vkl::math::cross(_state.forward, vkl::Vec4::unit_y)
    );
    _state.up = vkl::math::cross(_state.side, _state.forward);

    auto move_speed = _config.move_speed * vkl::Timekeeper::tick_delta();
    if(_kb.lshift) {
        move_speed *= 2.0f;
    }

    if(_kb.w)     { _state.pos += _state.forward * move_speed; }
    if(_kb.s)     { _state.pos -= _state.forward * move_speed; }
    if(_kb.a)     { _state.pos -= _state.side    * move_speed; }
    if(_kb.d)     { _state.pos += _state.side    * move_speed; }
    if(_kb.space) { _state.pos += _state.up      * move_speed; }
    if(_kb.lctrl) { _state.pos -= _state.up      * move_speed; }

    _orient();
}

// =============================================================================
void FPSCamera::init(vkl::Vec4 const &position, vkl::Vec4 const &forward) {
    _state.pos = position;
    _state.forward = forward;

    _subscribe_to_events();
}

// =============================================================================
void FPSCamera::set_perspective(float const near, float const far,
                                float const vertical_fov_degrees)
{
    _proj_mat = vkl::math::perspective_projection(
        near,
        far,
        vertical_fov_degrees,
        vkl::RenderConfig::window_aspect
    );
}

// =============================================================================
void FPSCamera::on_key_press(const vkl::KeyPressEvent &event) {
    switch(event.code) {
        case vkl::KB_W          : _kb.w = true;      break;
        case vkl::KB_A          : _kb.a = true;      break;
        case vkl::KB_S          : _kb.s = true;      break;
        case vkl::KB_D          : _kb.d = true;      break;
        case vkl::KB_LEFT_CTRL  : _kb.lctrl = true;  break;
        case vkl::KB_SPACE      : _kb.space = true;  break;
        case vkl::KB_LEFT_SHIFT : _kb.lshift = true; break;
    }
}

// =============================================================================
void FPSCamera::on_key_release(const vkl::KeyReleaseEvent &event) {
    switch(event.code) {
        case vkl::KB_W          : _kb.w = false;      break;
        case vkl::KB_A          : _kb.a = false;      break;
        case vkl::KB_S          : _kb.s = false;      break;
        case vkl::KB_D          : _kb.d = false;      break;
        case vkl::KB_LEFT_CTRL  : _kb.lctrl = false;  break;
        case vkl::KB_SPACE      : _kb.space = false;  break;
        case vkl::KB_LEFT_SHIFT : _kb.lshift = false; break;
    }
}

// =============================================================================
void FPSCamera::on_mouse_move(const vkl::MouseMoveEvent &event) {
    float new_pitch = static_cast<float>(-event.y_offset) * _config.look_speed;
    float new_yaw   = static_cast<float>(event.x_offset) * _config.look_speed;

    new_pitch += _state.pitch;
    new_yaw += _state.yaw;

    if(new_pitch > 89.9f)       { new_pitch = 89.9f;  }
    else if(new_pitch < -89.9f) { new_pitch = -89.9f; }

    if(new_yaw > 360.0f)    { new_yaw -= 360.0f; }
    else if(new_yaw < 0.0f) { new_yaw += 360.0f; }

    _state.pitch = new_pitch;
    _state.yaw = new_yaw;
}

// =============================================================================
void FPSCamera::_subscribe_to_events() {
    vkl::EventBroker::subscribe<vkl::KeyPressEvent>(
        this,
        &FPSCamera::on_key_press
    );

    vkl::EventBroker::subscribe<vkl::KeyReleaseEvent>(
        this,
        &FPSCamera::on_key_release
    );

    vkl::EventBroker::subscribe<vkl::MouseMoveEvent>(
        this,
        &FPSCamera::on_mouse_move
    );
}

// =============================================================================
void FPSCamera::_orient() {
    if(vkl::math::length2(_state.forward) <= 0.0f) {
        CONSOLE_CRITICAL("Cannot orient camera with zero forward vector.");
        return;
    }

    _view_mat = vkl::math::orient_view_matrix(
        _state.pos,
        _state.forward,
        _state.side,
        _state.up
    );
}

// =============================================================================
FPSCamera::FPSCamera() :
    _view_mat { vkl::Mat4::identity },
    _proj_mat { vkl::Mat4::zero },
    _config   { },
    _kb       { },
    _state    { }
{
    _config.move_speed = 3.0f;
    _config.look_speed = 0.075f;

    _state.pitch = 0.0f;
    _state.yaw = -90.0f;
}
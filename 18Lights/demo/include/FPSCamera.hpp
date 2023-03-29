#ifndef FPSCAMERA_HPP
#define FPSCAMERA_HPP

#include "vklearnin/system/pch.hpp"

class FPSCamera final {
public:
    void update();

    void init(vkl::Vec4 const &position, vkl::Vec4 const &forward);

    void set_orthographic(float const near, float const far);
    void set_perspective(float const near, float const far,
                         float const vertical_fov_degrees);

    void on_key_press(const vkl::KeyPressEvent &event);
    void on_key_release(const vkl::KeyReleaseEvent &event);
    void on_mouse_move(const vkl::MouseMoveEvent &event);

    inline auto const& view_matrix() const { return _view_mat; }
    inline auto const& proj_matrix() const { return _proj_mat; }

    FPSCamera();
    ~FPSCamera() = default;

    FPSCamera(FPSCamera &&) = delete;
    FPSCamera(const FPSCamera &) = delete;

    FPSCamera& operator=(FPSCamera &&) = delete;
    FPSCamera& operator=(const FPSCamera &) = delete;

private:
    vkl::Mat4 _view_mat;
    vkl::Mat4 _proj_mat;

    struct CameraConfig {
        float move_speed;
        float look_speed;
    } _config;

    struct KeyboardState {
        bool w = false;
        bool a = false;
        bool s = false;
        bool d = false;
        bool lctrl  = false;
        bool space  = false;
        bool lshift = false;
    } _kb;

    struct CameraState {
        vkl::Vec4 pos;
        vkl::Vec4 forward;
        vkl::Vec4 side;
        vkl::Vec4 up;

        float pitch;
        float yaw;
    } _state;

    void _subscribe_to_events();
    void _orient();
};

#endif // FPSCAMERA_HPP
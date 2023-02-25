#ifndef DEMO_HPP
#define DEMO_HPP

#include "vklearnin/vklearnin.hpp"
#include "vklearnin/meshes/Cube.hpp"

class Demo final : public vkl::Application {
public:
    void update() override;
    void submit_draws() override;

    void init() override;
    void shutdown() override;

    void on_key_press(const vkl::KeyPressEvent &event);
    void on_key_release(const vkl::KeyReleaseEvent &event);
    void on_mouse_move(const vkl::MouseMoveEvent &event);
    void on_mouse_button_press(const vkl::MouseButtonPressEvent &event);
    void on_mouse_button_release(const vkl::MouseButtonReleaseEvent &event);
    void on_mouse_scroll(const vkl::MouseScrollEvent &event);

    Demo();
    ~Demo() override = default;

    Demo(Demo &&) = delete;
    Demo(Demo const&) = delete;

    Demo& operator=(Demo &&) = delete;
    Demo& operator=(Demo const&) = delete;

private:
    struct KeyboardState {
        bool w;
        bool a;
        bool s;
        bool d;
    } _kb;

    struct MouseState {
        int32_t x_offset = 0;
        int32_t y_offset = 0;
    } _mouse;

    struct CamData {
        vkl::Vec4 pos;
        vkl::Vec4 forward;
        vkl::Vec4 side;
        vkl::Vec4 up;

        float pitch = 0.0f;
        float yaw   = -90.0f;

        float kb_speed    = 3.0f;
        float mouse_speed = 0.1f;
    } _cam_data;

    struct VPMatrices {
        vkl::Mat4 view;
        vkl::Mat4 proj;
    } _vp_matrices;

    vkl::Camera _persp_camera;

    std::vector<vkl::BufferObject> _vp_ubos;

    vkl::Cube<vkl::VertexColor>   _color_cube;
    vkl::Cube<vkl::VertexTexture> _texture_cube;

    std::vector<vkl::Mat4> _color_model_matrices;
    std::vector<vkl::Mat4> _texture_model_matrices;

    vkl::Texture2D _bricks_a;
    vkl::Texture2D _bricks_b;
};

#endif // DEMO_HPP
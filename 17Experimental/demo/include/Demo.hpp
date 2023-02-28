#ifndef DEMO_HPP
#define DEMO_HPP

#include "vklearnin/vklearnin.hpp"
#include "vklearnin/meshes/Cube.hpp"
#include "vklearnin/meshes/XYPlane.hpp"
#include "vklearnin/meshes/XZPlane.hpp"

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
        bool w = false;
        bool a = false;
        bool s = false;
        bool d = false;
        bool lctrl = false;
        bool space = false;
    } _kb;

    struct CamData {
        vkl::Vec4 pos;
        vkl::Vec4 forward;
        vkl::Vec4 side;
        vkl::Vec4 up;

        float pitch = 0.0f;
        float yaw   = -90.0f;

        float kb_speed    = 3.0f;
        float mouse_speed = 0.075f;
    } _cam_data;

    struct VPMatrices {
        vkl::Mat4 view;
        vkl::Mat4 proj;
    } _vp_matrices;

    vkl::Camera _persp_camera;

    std::vector<vkl::BufferObject> _vp_ubos;

    vkl::Cube<vkl::VertexFlatColor>      _color_cube;
    vkl::Cube<vkl::VertexFlatTexture>    _texture_cube;
    vkl::XYPlane<vkl::VertexFlatTexture> _wall_mesh;
    vkl::XZPlane<vkl::VertexFlatTexture> _floor_mesh;

    std::vector<vkl::Mat4> _color_model_matrices;
    std::vector<vkl::Mat4> _texture_model_matrices;
    vkl::Mat4 _wall_matrix;
    vkl::Mat4 _floor_matrix;

    vkl::Texture2D _bricks_a;
    vkl::Texture2D _bricks_b;
    vkl::Texture2D _wall_texture;
    vkl::Texture2D _floor_texture;
};

#endif // DEMO_HPP
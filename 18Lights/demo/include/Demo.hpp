#ifndef DEMO_HPP
#define DEMO_HPP

#include "vklearnin/vklearnin.hpp"
#include "vklearnin/meshes/Cube.hpp"
#include "vklearnin/meshes/XYPlane.hpp"
#include "vklearnin/meshes/XZPlane.hpp"
#include "vklearnin/meshes/Skybox.hpp"

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
        bool lctrl  = false;
        bool space  = false;
        bool lshift = false;
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

    vkl::PointLight   _point_light;
    vkl::BufferObject _point_light_ubo;

    vkl::Cube<vkl::VertexFlatColor>      _caster_mesh;
    vkl::Cube<vkl::VertexLitColor>       _cube_mesh;
    vkl::XZPlane<vkl::VertexFlatTexture> _floor_mesh;

    vkl::Mat4 _caster_matrix;
    vkl::Mat4 _cube_matrix;
    vkl::Mat4 _floor_matrix;

    vkl::Texture2D _cube_texture;
    vkl::Texture2D _floor_texture;

    void _subscribe_to_events();
    void _init_camera();
    void _init_meshes();
    void _init_trs_matrices();
    void _init_textures();
};

#endif // DEMO_HPP
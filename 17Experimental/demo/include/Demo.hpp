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

        bool up;
        bool down;
        bool left;
        bool right;
    } _kb;

    struct CamData {
        glm::vec3 pos;
        glm::vec3 forward;

        float speed = 3.0f;
        float pitch = 0.0f;
        float yaw = -90.0f;
    } _cam_data;

    struct VPMatrices {
        glm::mat4 view;
        glm::mat4 proj;
    } _vp_matrices;

    vkl::Camera _persp_camera;

    std::vector<vkl::BufferObject> _view_proj_ubos;

    vkl::Cube<vkl::VertexColor>   _color_cube;
    vkl::Cube<vkl::VertexTexture> _texture_cube;

    std::vector<glm::mat4> _color_model_matrices;
    std::vector<glm::mat4> _texture_model_matrices;

    vkl::Texture2D _bricks_a;
    vkl::Texture2D _bricks_b;
};

#endif // DEMO_HPP
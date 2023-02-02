#ifndef DEMO_HPP
#define DEMO_HPP

#include "vklearnin/vklearnin.hpp"
#include "vklearnin/meshes/Cube.hpp"

class Demo final : public vkl::Application {
public:
    void submit_draws() override;

    void init() override;
    void shutdown() override;

    Demo() = default;
    ~Demo() override = default;

    Demo(Demo &&) = delete;
    Demo(Demo const&) = delete;
    
    Demo& operator=(Demo &&) = delete;
    Demo& operator=(Demo const&) = delete;

private:
    vkl::Camera _persp_camera;
    std::vector<vkl::BufferObject> _view_proj_ubos;

    vkl::Cube<vkl::VertexColor>   _color_cube;
    vkl::Cube<vkl::VertexTexture> _texture_cube;

    std::vector<vkl::Mat4> _color_model_matrices;
    std::vector<vkl::Mat4> _texture_model_matrices;

    vkl::Texture2D _bricks;
};

#endif // DEMO_HPP
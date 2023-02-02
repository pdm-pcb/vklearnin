#ifndef DEMO_HPP
#define DEMO_HPP

#include "vklearnin/vklearnin.hpp"
#include "vklearnin/meshes/Cube.hpp"

class Demo final : public vkl::Application {
public:
    void submit_draws() override;

    void init() override;
    void shutdown() override;

    Demo();
    ~Demo() override = default;

    Demo(Demo &&) = delete;
    Demo(const Demo &) = delete;
    
    Demo& operator=(Demo &&) = delete;
    Demo& operator=(const Demo &) = delete;

private:
    vkl::Camera _persp_camera;

    std::vector<vkl::BufferObject> _view_proj_ubos;

    vkl::Cube _cube;
    std::vector<vkl::Mat4> _model_matrices;

    vkl::Texture2D _bricks;
};

#endif // DEMO_HPP
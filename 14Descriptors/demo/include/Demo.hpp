#ifndef DEMO_HPP
#define DEMO_HPP

#include "vklearnin/vklearnin.hpp"
#include "vklearnin/meshes/XYPlane.hpp"
#include "vklearnin/meshes/Cube.hpp"

class Demo final : public vkl::Application {
public:
    void submit_draws(vkl::Renderer &renderer) override;

    void init() override;
    void shutdown() override;

    Demo();
    ~Demo() override = default;

    Demo(Demo &&) = delete;
    Demo(const Demo &) = delete;
    
    Demo& operator=(Demo &&) = delete;
    Demo& operator=(const Demo &) = delete;

private:
    vkl::XYPlane _xy_plane;
    vkl::Cube    _cube;
    std::array<float, 4> _vert_scale;
    std::array<float, 4> _color_scale;
};

#endif // DEMO_HPP
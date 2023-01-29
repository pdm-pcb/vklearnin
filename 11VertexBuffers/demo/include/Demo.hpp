#ifndef DEMO_HPP
#define DEMO_HPP

#include "vklearnin/vklearnin.hpp"
#include "vklearnin/meshes/XYPlane.hpp"

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
};

#endif // DEMO_HPP
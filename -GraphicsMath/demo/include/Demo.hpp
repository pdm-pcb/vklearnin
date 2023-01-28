#ifndef DEMO_HPP
#define DEMO_HPP

#include "vklearnin/vklearnin.hpp"
#include "vklearnin/meshes/XYPlane.hpp"

class Demo final : public vkl::Application {
public:
    void run_renderpasses(const vkl::CmdBuffer &command_buffer,
                          const uint32_t frame_index) override;

    void init() override;
    void shutdown() override;

    Demo();
    ~Demo() override = default;

    Demo(Demo &&) = delete;
    Demo(const Demo &) = delete;
    
    Demo & operator=(Demo &&) = delete;
    Demo & operator=(const Demo &) = delete;

private:
    std::vector<vkl::Framebuffer> _framebuffers;

    vkl::RenderPass _render_pass;
    vkl::Pipeline   _pipeline;

    vkl::XYPlane _xy_plane;
};

#endif // DEMO_HPP
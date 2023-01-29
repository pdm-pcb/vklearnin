#ifndef VKLEARNIN_RENDERING_RENDERER_HPP
#define VKLEARNIN_RENDERING_RENDERER_HPP

#include "vklearnin/system/pch.hpp"
#include "vklearnin/rendering/DrawSubmission.hpp"
#include "vklearnin/rendering/renderpass/RenderPass.hpp"
#include "vklearnin/rendering/renderpass/Framebuffer.hpp"
#include "vklearnin/rendering/pipeline/Pipeline.hpp"

namespace vkl {

class Renderer {
public:
    void submit(const DrawSubmission &draw);
    void render_pass(const vk::CommandBuffer &cmd_buffer);

    void init();
    void shutdown();

    Renderer();
    ~Renderer() = default;

    Renderer(Renderer &&) = delete;
    Renderer(const Renderer &) = delete;

    Renderer& operator=(Renderer &&) = delete;
    Renderer& operator=(const Renderer &) = delete;

private:
    std::vector<Framebuffer> _framebuffers;
    vkl::RenderPass _render_pass;
    vkl::Pipeline   _pipeline;

    std::vector<DrawSubmission> _draws;

    void _init_framebuffers();
    void _init_pipeline();
};

} // namespace vkl

#endif // VKLEARNIN_RENDERING_RENDERER_HPP
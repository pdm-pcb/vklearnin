#ifndef VKLEARNIN_RENDERING_RENDERER_HPP
#define VKLEARNIN_RENDERING_RENDERER_HPP

#include "vklearnin/system/pch.hpp"
#include "vklearnin/rendering/DrawSubmission.hpp"
#include "vklearnin/rendering/renderpass/RenderPass.hpp"
#include "vklearnin/rendering/renderpass/Framebuffer.hpp"
#include "vklearnin/rendering/pipeline/Pipeline.hpp"
#include "vklearnin/rendering/descriptors/DescriptorPool.hpp"
#include "vklearnin/rendering/descriptors/DescriptorSetLayout.hpp"
#include "vklearnin/rendering/descriptors/DescriptorSet.hpp"

namespace vkl {

struct BufferObject;
struct ImageObject;

class Renderer {
public:
    static void submit(const DrawSubmission &draw);
    static void render_pass(const vk::CommandBuffer &cmd_buffer);

    static void init();
    static void shutdown();

    static void add_ubo(const std::vector<BufferObject> &ubos,
                        const vk::ShaderStageFlags stage_flags);
    static void add_texture2D(const ImageObject &texture);
    static void create_pipelines();

    Renderer() = delete;

private:
    static std::vector<Framebuffer> _framebuffers;

    static DescriptorPool             _desc_pool;
    static DescriptorSetLayout        _desc_layout;
    static std::vector<DescriptorSet> _desc_sets;

    static RenderPass _render_pass;
    static Pipeline   _pipeline;

    static std::vector<DrawSubmission> _draws;

    static void _init_framebuffers();
    static void _init_descriptors();
};

} // namespace vkl

#endif // VKLEARNIN_RENDERING_RENDERER_HPP
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
    enum DescBindFreq {
        GLOBAL_UNIFORM,
        PER_MATERIAL,
        PER_DRAW,

        MAX
    };

    static void submit(const DrawSubmission &draw);
    static void render_pass(const vk::CommandBuffer &cmd_buffer);

    static void init();
    static void shutdown();

    static void set_global_uniforms(const std::vector<BufferObject> &ubos);
    static void add_material(const ImageObject &texture);
    static void create_pipelines();

    Renderer() = delete;

private:
    static DescriptorPool _desc_pool;

    static DescriptorSetLayout _global_uniform_set_layout;
    static DescriptorSetLayout _material_set_layout;
    static DescriptorSetLayout _draw_set_layout;

    using DescriptorSets = std::vector<DescriptorSet>;
    static DescriptorSets _global_uniform_sets;
    static DescriptorSets _material_sets;
    static DescriptorSets _draw_sets;

    static RenderPass _render_pass;
    static Pipeline   _pipeline;

    static std::vector<Framebuffer> _framebuffers;

    struct MaterialDrawQueue {
        size_t const set_index;
        std::vector<DrawSubmission> queue;
    };

    using Draws = std::unordered_map<uint64_t, MaterialDrawQueue>;
    static Draws _draws;

    static void _init_framebuffers();
    static void _init_descriptors();
};

} // namespace vkl

#endif // VKLEARNIN_RENDERING_RENDERER_HPP
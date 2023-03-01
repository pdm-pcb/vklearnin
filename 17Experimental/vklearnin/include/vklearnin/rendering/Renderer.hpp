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
    enum PipelineType {
        FLAT_COLOR,
        FLAT_TEXTURE,

        MAX
    };

    static void submit(PipelineType const pipeline, DrawSubmission const &draw);
    static void render_pass(vk::CommandBuffer const &cmd_buffer);

    static void init();
    static void shutdown();

    static void set_global_uniforms(std::vector<BufferObject> const &ubos);
    static void add_material(ImageObject const &texture);
    static void create_pipelines();

    Renderer() = delete;

private:
    enum DescBindFreq {
        GLOBAL_UNIFORM,
        PER_MATERIAL,
        PER_DRAW
    };

    static DescriptorPool _desc_pool;

    static DescriptorSetLayout _global_uniform_set_layout;
    static DescriptorSetLayout _material_set_layout;
    static DescriptorSetLayout _draw_set_layout;

    using DescriptorSets = std::vector<DescriptorSet>;
    static DescriptorSets _global_uniform_sets;
    static DescriptorSets _material_sets;
    static DescriptorSets _draw_sets;

    static RenderPass _render_pass;
    static std::array<Pipeline, PipelineType::MAX> _pipelines;

    static std::vector<Framebuffer> _framebuffers;

    static std::vector<DrawSubmission> _color_draws;

    struct MaterialDrawQueue {
        size_t const set_index;
        std::vector<DrawSubmission> queue;
    };
    using TextureDraws = std::unordered_map<uint64_t, MaterialDrawQueue>;
    static TextureDraws _texture_draws;

    static DescriptorSet  _skybox_desc_set;
    static DrawSubmission const _skybox_draw;

    static void _init_framebuffers();
    static void _init_descriptors();

    static void _init_color_pipeline();
    static void _init_texture_pipeline();
    static void _init_skybox_pipeline();

    static void _execute_color_pipeline(vk::CommandBuffer const &cmd_buffer);
    static void _execute_texture_pipeline(vk::CommandBuffer const &cmd_buffer);
    static void _execute_skybox_pipeline(vk::CommandBuffer const &cmd_buffer);

    static void _bind_globals(Pipeline const &pipeline,
                              vk::CommandBuffer const &cmd_buffer);
    static void _send_push_constants(Pipeline const &pipeline,
                                     DrawSubmission const &draw,
                                     vk::CommandBuffer const &cmd_buffer);
};

} // namespace vkl

#endif // VKLEARNIN_RENDERING_RENDERER_HPP
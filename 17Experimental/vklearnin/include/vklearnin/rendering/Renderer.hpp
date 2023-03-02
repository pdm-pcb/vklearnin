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
#include "vklearnin/meshes/VertexTypes.hpp"

namespace vkl {

struct BufferObject;
struct ImageObject;

class Renderer {
public:
    template <typename VertexType>
    static void submit(DrawSubmission<VertexType> const &draw);

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

    static std::vector<Framebuffer> _framebuffers;

    static Pipeline _flat_color_pipeline;
    static Pipeline _flat_texture_pipeline;

    using ColorDraws = std::vector<DrawSubmission<VertexFlatColor>>;
    static ColorDraws _flat_color_draws;

    struct MaterialDrawQueue {
        size_t const set_index;
        std::vector<DrawSubmission<VertexFlatTexture>> queue;
    };
    using TextureDraws = std::unordered_map<uint64_t, MaterialDrawQueue>;
    static TextureDraws _flat_texture_draws;

    static void _init_framebuffers();
    static void _init_descriptors();

    static void _init_flat_color_pipeline();
    static void _init_flat_texture_pipeline();
    static void _init_skybox_pipeline();

    static void
    _execute_flat_color_pipeline(vk::CommandBuffer const &cmd_buffer);

    static void
    _execute_flat_texture_pipeline(vk::CommandBuffer const &cmd_buffer);

    static void _execute_skybox_pipeline(vk::CommandBuffer const &cmd_buffer);

    static void _bind_global_uniforms(Pipeline const &pipeline,
                                      vk::CommandBuffer const &cmd_buffer);

    template <typename VertexType>
    static void _send_push_constants(Pipeline const &pipeline,
                                     DrawSubmission<VertexType> const &draw,
                                     vk::CommandBuffer const &cmd_buffer);
};

// =============================================================================
template <typename VertexType>
inline void Renderer::submit(DrawSubmission<VertexType> const &draw) {
    if constexpr(std::is_same_v<VertexType, VertexFlatColor>) {
        _flat_color_draws.push_back(draw);
    }

    if constexpr(std::is_same_v<VertexType, VertexFlatTexture>) {
        auto mat_index = reinterpret_cast<uint64_t>(
            VkImage(draw.material->image().handle)
        );
        _flat_texture_draws.at(mat_index).queue.push_back(draw);
    }
}

// =============================================================================
template <typename VertexType>
inline void
Renderer::_send_push_constants(Pipeline const &pipeline,
                               DrawSubmission<VertexType> const &draw,
                               vk::CommandBuffer const &cmd_buffer)
{
    size_t running_offset = 0u;
    for(auto const& push_constant : draw.push_constants) {
        cmd_buffer.pushConstants(
            pipeline.layout(),
            push_constant.stage_flags,
            static_cast<uint32_t>(running_offset),
            static_cast<uint32_t>(push_constant.size),
            push_constant.data
        );

        running_offset += push_constant.size;
    }
}

} // namespace vkl

#endif // VKLEARNIN_RENDERING_RENDERER_HPP
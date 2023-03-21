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
#include "vklearnin/lighting/LightProps.hpp"
#include "vklearnin/lighting/MaterialProps.hpp"

namespace vkl {

class FrameData;
struct BufferObject;
struct ImageObject;

class Renderer {
public:
    struct GlobalBuffer {
        Mat4 view_matrix;
        Mat4 proj_matrix;
    };

    static void update_global_buffer(GlobalBuffer const &buffer);

    template <typename VertexType>
    static void submit(DrawSubmission<VertexType> const &draw);

    static void render_pass();

    static void init();
    static void shutdown();

    // ...??
    // static void set_flat_textures(std::vector<ImageObject> const &images);

    static void create_pipelines();

    Renderer() = delete;

private:
    static RenderPass               _render_pass;
    static std::vector<Framebuffer> _framebuffers;
    static std::vector<FrameData>   _frame_data;
    static uint32_t                 _frame_index;
    static uint64_t                 _frame_count;

    // Convenience using delcarations ------------------------------------------
    using PerFrameSets    = std::vector<DescriptorSet>;
    using PerFrameBuffers = std::vector<BufferObject>;
    using FlatColorDraw   = DrawSubmission<VertexFlatColor>;
    using FlatColorDraws  = std::vector<FlatColorDraw>;

    // Descriptors -------------------------------------------------------------
    static DescriptorPool _desc_pool;

    static DescriptorSetLayout _global_buffer_layout;
    static PerFrameSets        _global_buffer_sets;

    // Shader Resources --------------------------------------------------------
    static PerFrameBuffers _global_buffers;

    // Pipelines ---------------------------------------------------------------
    static Pipeline _flat_color_pipeline;

    // Draw Queues -------------------------------------------------------------
    static FlatColorDraws _flat_color_draws;

    static void _init_framebuffers();
    static void _init_frame_data();

    static void _init_descriptor_pool();
    static void _init_descriptor_sets();

    static void _init_shader_resources();
    static void _shutdown_shader_resources();

    static void _init_flat_color_pipeline();

    static void
    _execute_flat_color_pipeline(vk::CommandBuffer const &cmd_buffer);

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
    // else if constexpr(std::is_same_v<VertexType, VertexLitColor>) {
    //     _lit_color_draws.push_back(draw);
    // }
    // else if constexpr(std::is_same_v<VertexType, VertexFlatTexture>) {
    //     auto mat_index = reinterpret_cast<uint64_t>(
    //         VkImage(draw.material->image().handle)
    //     );
    //     _flat_texture_draws.at(mat_index).queue.push_back(draw);
    // }
    // else if constexpr(std::is_same_v<VertexType, VertexSkybox>) {
    //     _skybox_draw = draw;
    // }
}

// =============================================================================
template <typename VertexType>
inline void
Renderer::_send_push_constants(Pipeline const &pipeline,
                               DrawSubmission<VertexType> const &draw,
                               vk::CommandBuffer const &cmd_buffer)
{
    size_t offset = 0u;
    for(auto const& push_constant : draw.push_constants) {
        cmd_buffer.pushConstants(
            pipeline.layout(),
            push_constant.stage_flags,
            static_cast<uint32_t>(offset),
            static_cast<uint32_t>(push_constant.size),
            push_constant.data
        );

        offset += push_constant.size;
    }
}

} // namespace vkl

#endif // VKLEARNIN_RENDERING_RENDERER_HPP
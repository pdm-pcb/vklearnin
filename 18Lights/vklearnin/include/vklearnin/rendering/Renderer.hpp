#ifndef VKLEARNIN_RENDERING_RENDERER_HPP
#define VKLEARNIN_RENDERING_RENDERER_HPP

#include "vklearnin/system/pch.hpp"
#include "vklearnin/rendering/FrameData.hpp"
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
    struct GlobalBuffer {
        Mat4 view_matrix;
        Mat4 proj_matrix;
    };

    static void update_global_buffer(GlobalBuffer const &buffer);

    template <typename VertexType>
    static void submit(DrawSubmission<VertexType> const &draw);

    static void record_commands();
    static void submit_and_present();

    static void init();
    static void shutdown();

    static void set_flat_textures(std::vector<Texture2D> const &textures);

    static void create_pipelines();

    Renderer() = delete;

private:
    static RenderPass               _render_pass;
    static std::vector<Framebuffer> _framebuffers;
    static std::vector<FrameData>   _frame_data;
    static uint32_t                 _frame_index;
    static uint64_t                 _frame_count;

    // Convenience using delcarations ------------------------------------------
    using DescSetList = std::vector<DescriptorSet>;
    using BufferList  = std::vector<BufferObject>;
    using ImageList   = std::vector<ImageObject>;

    using FlatColorDraw      = DrawSubmission<VertexFlatColor>;
    using FlatColorDrawQueue = std::vector<FlatColorDraw>;

    using FlatTextureDraw  = DrawSubmission<VertexFlatTexture>;
    using FlatTextureDraws = std::vector<FlatTextureDraw>;

    struct PerFlatTextureDraws {
        size_t set_index;
        FlatTextureDraws queue;
    };

    using FlatTextureDrawQueue =
        std::unordered_map<uint64_t, PerFlatTextureDraws>;

    // Descriptors -------------------------------------------------------------
    static DescriptorPool _desc_pool;

    static DescriptorSetLayout _global_buffer_layout;
    static DescSetList         _global_buffer_sets;
    static DescriptorSetLayout _flat_texture_layout;
    static DescSetList         _flat_texture_sets;

    // Shader Resources --------------------------------------------------------
    static BufferList _global_buffers;

    // Pipelines ---------------------------------------------------------------
    static Pipeline _flat_color_pipeline;
    static Pipeline _flat_texture_pipeline;

    // Draw Queues -------------------------------------------------------------
    static FlatColorDrawQueue   _flat_color_draws;
    static FlatTextureDrawQueue _flat_texture_draws;

    static void _init_framebuffers();
    static void _init_frame_data();

    static void _init_descriptor_pool();

    static void _init_global_buffers();
    static void _init_flat_textures();

    static void _init_flat_color_pipeline();
    static void _init_flat_texture_pipeline();

    static void _execute_flat_color_pipeline(const FrameData &frame_data);
    static void _execute_flat_texture_pipeline(const FrameData &frame_data);

    template <typename VertexType>
    static void _send_push_constants(Pipeline const &pipeline,
                                     DrawSubmission<VertexType> const &draw,
                                     FrameData const &frame_data);
};

// =============================================================================
template <typename VertexType>
inline void Renderer::submit(DrawSubmission<VertexType> const &draw) {
    if constexpr(std::is_same_v<VertexType, VertexFlatColor>) {
        _flat_color_draws.push_back(draw);
    }
    else if constexpr(std::is_same_v<VertexType, VertexFlatTexture>) {
        auto texture_id = reinterpret_cast<uint64_t>(
            VkImage(draw.material->image().handle)
        );
        _flat_texture_draws[texture_id].queue.push_back(draw);
    }
    // else if constexpr(std::is_same_v<VertexType, VertexSkybox>) {
    //     _skybox_draw = draw;
    // }
    // else if constexpr(std::is_same_v<VertexType, VertexLitColor>) {
    //     _lit_color_draws.push_back(draw);
    // }
}

// =============================================================================
template <typename VertexType>
inline void
Renderer::_send_push_constants(Pipeline const &pipeline,
                               DrawSubmission<VertexType> const &draw,
                               FrameData const &frame_data)
{
    size_t offset = 0u;
    for(auto const& push_constant : draw.push_constants) {
        frame_data.cmd_buffer().native().pushConstants(
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
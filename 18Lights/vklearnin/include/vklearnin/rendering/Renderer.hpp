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
#include "vklearnin/meshes/Skybox.hpp"
#include "vklearnin/lighting/LightProps.hpp"
#include "vklearnin/lighting/ShadowMapTransforms.hpp"

namespace vkl {

struct BufferObject;
struct ImageObject;
struct Material;

class Renderer {
public:
    struct CameraData {
        Mat4 view_matrix;
        Mat4 proj_matrix;
    };

    static void update_camera_data(CameraData const &data);
    static void update_light_props(LightProps const &data);

    static void submit_draw(Mesh<VertexFlatColor> const &mesh,
                            Mat4 const &model_matrix);

    static void submit_draw(Mesh<VertexTexture> const &mesh,
                            Texture2D const &texture,
                            Mat4 const &model_matrix);

    static void submit_draw(Mesh<VertexLitColor> const &mesh,
                            Mat4 const &model_matrix);

    static void submit_draw(Mesh<VertexMaterial> const &mesh,
                            Material const &material,
                            Mat4 const &model_matrix);

    static void record_commands();
    static void submit_commands_and_present();

    static void init();
    static void shutdown();

    static void set_textures(std::vector<Texture2D> const &textures);
    static void set_skybox_texture(Texture2D::CubeFilepaths const &filepaths);
    static void set_materials(std::vector<Material> const &materials);

    static void create_pipelines();

    Renderer() = delete;

private:
    static RenderPass _color_pass;
    static RenderPass _shadow_map_pass;

    static std::vector<Framebuffer> _color_framebuffers;
    static std::vector<Framebuffer> _shadow_map_framebuffers;

    static uint32_t _shadow_map_resolution;

    static std::vector<FrameData> _frame_data;
    static uint32_t               _frame_index;
    static uint64_t               _frame_count;

    // Convenience using delcarations ------------------------------------------
    using DescSetList = std::vector<DescriptorSet>;
    using BufferList  = std::vector<BufferObject>;
    using ImageList   = std::vector<ImageObject>;

    using FlatColorDraw      = DrawSubmission<VertexFlatColor>;
    using FlatColorDrawQueue = std::vector<FlatColorDraw>;

    using TextureDraw  = DrawSubmission<VertexTexture>;
    using TextureDraws = std::vector<TextureDraw>;

    struct PerTextureDraws {
        size_t const set_index = std::numeric_limits<size_t>::max();
        TextureDraws queue;
    };

    using TextureDrawQueue =
        std::unordered_map<uint64_t, PerTextureDraws>;

    using LitColorDraw = DrawSubmission<VertexLitColor>;
    using LitColorDrawQueue = std::vector<LitColorDraw>;

    using MaterialDraw  = DrawSubmission<VertexMaterial>;
    using MaterialDraws = std::vector<MaterialDraw>;

    struct PerMaterialDraws {
        size_t const set_index = std::numeric_limits<size_t>::max();
        MaterialDraws queue;
    };

    using MaterialDrawQueue =
        std::unordered_map<uint64_t, PerMaterialDraws>;

    // Descriptors -------------------------------------------------------------
    static DescriptorPool _desc_pool;

    static DescriptorSetLayout _global_data_layout;
    static DescSetList         _global_data_sets;

    static DescriptorSetLayout _texture_layout;
    static DescSetList         _texture_sets;

    static DescriptorSet _skybox_texture_set;

    static DescriptorSetLayout _light_props_layout;
    static DescSetList         _light_props_sets;

    static DescriptorSetLayout _material_layout;
    static DescSetList         _material_sets;

    static DescriptorSetLayout _shadow_map_transform_layout;
    static DescSetList         _shadow_map_transform_sets;
    static DescriptorSet       _shadow_map_set;

    // Shader Resources --------------------------------------------------------
    static BufferList           _camera_buffers;
    static Skybox<VertexSkybox> _skybox_mesh;
    static Texture2D            _skybox_texture;
    static BufferList           _light_props_buffers;
    static BufferList           _shadow_map_transform_buffers;

    // Pipelines ---------------------------------------------------------------
    static Pipeline _flat_color_pipeline;
    static Pipeline _texture_pipeline;
    static Pipeline _skybox_pipeline;
    static Pipeline _lit_color_pipeline;
    static Pipeline _material_pipeline;
    static Pipeline _shadow_map_pipeline;

    // Draw Queues -------------------------------------------------------------
    static FlatColorDrawQueue _flat_color_draws;
    static TextureDrawQueue   _texture_draws;
    static LitColorDrawQueue  _lit_color_draws;
    static MaterialDrawQueue  _material_draws;

    static void _init_color_pass();
    static void _init_color_framebuffers();

    static void _init_shadow_map_pass();
    static void _init_shadow_map_framebuffers();

    static void _init_frame_data();

    static void _init_descriptor_pool();

    static void _init_camera_buffers();
    static void _init_global_data_sets();
    static void _init_texture_sets();
    static void _init_skybox_resources();
    static void _init_light_props_buffers();
    static void _init_light_props_sets();
    static void _init_material_sets();
    static void _init_shadow_map_resources();
    static void _init_shadow_map_sets();

    static void _init_flat_color_pipeline();
    static void _init_texture_pipeline();
    static void _init_skybox_pipeline();
    static void _init_lit_color_pipeline();
    static void _init_material_pipeline();
    static void _init_shadow_map_pipeline();

    static void _execute_flat_color_pipeline();
    static void _execute_texture_pipeline();
    static void _execute_skybox_pipeline();
    static void _execute_lit_color_pipeline();
    static void _execute_material_pipeline();
    static void _execute_shadow_map_pipeline();

    template <typename VertexType>
    static void _send_push_constants(Pipeline const &pipeline,
                                     DrawSubmission<VertexType> const &draw,
                                     FrameData const &frame_data);
};

// =============================================================================
template <typename VertexType>
void Renderer::_send_push_constants(Pipeline const &pipeline,
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
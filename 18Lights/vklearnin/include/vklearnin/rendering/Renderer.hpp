#ifndef VKLEARNIN_RENDERING_RENDERER_HPP
#define VKLEARNIN_RENDERING_RENDERER_HPP

#include "vklearnin/system/pch.hpp"
#include "vklearnin/rendering/FrameData.hpp"
#include "vklearnin/rendering/DrawSubmission.hpp"
#include "vklearnin/rendering/pipeline/Pipeline.hpp"
#include "vklearnin/rendering/descriptors/DescriptorPool.hpp"
#include "vklearnin/rendering/descriptors/DescriptorSetLayout.hpp"
#include "vklearnin/rendering/descriptors/DescriptorSet.hpp"
#include "vklearnin/rendering/targets/DepthBuffer.hpp"
#include "vklearnin/rendering/targets/ColorBuffer.hpp"
#include "vklearnin/meshes/Skybox.hpp"
#include "vklearnin/lighting/SceneLights.hpp"

namespace vkl {

class GeneratedMesh;

struct BufferObject;
struct ImageObject;

class Renderer {
public:
    struct CameraData {
        Mat4 view_matrix;
        Mat4 proj_matrix;
    };

    static void update_camera_data(CameraData const &data);
    static void update_scene_lights(LightProps const &props,
                                    SceneLights const &lights);

    static void submit_draw(GeneratedMesh const &mesh,
                            Mat4 const &model_matrix);

    static void submit_draw(GeneratedMesh const &mesh,
                            Texture2D const &texture,
                            Mat4 const &model_matrix);

    static void submit_draw_lit(GeneratedMesh const &mesh,
                                Mat4 const &model_matrix);

    static void record_commands();
    static void submit_commands_and_present();

    static void init();
    static void shutdown();

    static void set_textures(std::vector<Texture2D> const &textures);
    static void set_skybox_texture(Texture2D::CubeFilepaths const &filepaths);

    static void create_pipelines();

    Renderer() = delete;

private:
    // Convenience using delcarations ------------------------------------------
    using DepthBufferList = std::vector<DepthBuffer>;
    using ColorBufferList = std::vector<ColorBuffer>;

    using PushList = std::vector<PushConstant>;

    using DescSetList = std::vector<DescriptorSet>;
    using BufferList  = std::vector<BufferObject>;
    using ImageList   = std::vector<ImageObject>;

    using DrawQueue = std::vector<DrawSubmission>;

    struct PerTextureDraws {
        size_t const set_index = std::numeric_limits<size_t>::max();
        DrawQueue queue;
    };
    using TextureDrawQueue = std::unordered_map<uint64_t, PerTextureDraws>;

    // Renderer specific values ------------------------------------------------
    static DepthBufferList _depth_buffers;
    static ColorBufferList _color_buffers;

    static uint32_t _shadow_map_resolution;

    static uint32_t _frame_index;
    static uint64_t _frame_count;

    static std::vector<FrameData> _frame_data;

    // Descriptors -------------------------------------------------------------
    static DescriptorPool _desc_pool;

    static DescriptorSetLayout _global_data_layout;
    static DescSetList         _global_data_sets;

    static DescriptorSetLayout _texture_layout;
    static DescSetList         _texture_sets;

    static DescriptorSet _skybox_texture_set;

    static DescriptorSetLayout _scene_lights_layout;
    static DescSetList         _scene_lights_sets;

    // Shader Resources --------------------------------------------------------
    static BufferList _camera_buffers;
    static Skybox     _skybox_mesh;
    static Texture2D  _skybox_texture;

    static BufferList _light_props_buffers;

    static char *_dir_ssbo_data;
    static char *_point_ssbo_data;
    static char *_spot_ssbo_data;

    static BufferList _dir_ssbo_buffers;
    static BufferList _point_ssbo_buffers;
    static BufferList _spot_ssbo_buffers;

    // Pipelines ---------------------------------------------------------------
    static Pipeline _flat_color_pipeline;
    static Pipeline _texture_pipeline;
    static Pipeline _skybox_pipeline;
    static Pipeline _lit_color_pipeline;

    // Draw Queues -------------------------------------------------------------
    static DrawQueue        _flat_color_draws;
    static TextureDrawQueue _texture_draws;
    static DrawQueue        _lit_color_draws;

    static void _init_depth_buffers();
    static void _init_color_buffers();

    static void _init_frame_data();

    static void _init_descriptor_pool();

    static void _init_camera_buffers();
    static void _init_global_data_sets();
    static void _init_skybox_resources();
    static void _init_lights_buffers();
    static void _init_lights_sets();

    static void _init_flat_color_pipeline();
    static void _init_texture_pipeline();
    static void _init_skybox_pipeline();
    static void _init_lit_color_pipeline();

    static void _execute_flat_color_pipeline();
    static void _execute_texture_pipeline();
    static void _execute_skybox_pipeline();
    static void _execute_lit_color_pipeline();

    static void _send_push_constants(Pipeline const &pipeline,
                                     PushList const &push_constants);
};

} // namespace vkl

#endif // VKLEARNIN_RENDERING_RENDERER_HPP
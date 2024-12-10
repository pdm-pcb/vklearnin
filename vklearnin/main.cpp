#include "vklearnin/vulkan/vkInstance.hpp"
#include "vklearnin/platform/TargetWindow.hpp"
#include "vklearnin/vulkan/swapchain/vkSurface.hpp"
#include "vklearnin/vulkan/devices/vkPhysicalDevice.hpp"
#include "vklearnin/vulkan/devices/vkDevice.hpp"
#include "vklearnin/vulkan/swapchain/vkSwapchain.hpp"
#include "vklearnin/vulkan/swapchain/vkFrameBuffer.hpp"
#include "vklearnin/vulkan/swapchain/vkFrameSync.hpp"
#include "vklearnin/vulkan/pipelines/ShaderProgram.hpp"
#include "vklearnin/vulkan/pipelines/vkGraphicsPipeline.hpp"
#include "vklearnin/vulkan/pipelines/vkComputePipeline.hpp"

#include "vklearnin/rendering/passes/ColorPass.hpp"
#include "vklearnin/rendering/passes/DepthPass.hpp"
#include "vklearnin/rendering/passes/MSAAPass.hpp"

#include "vklearnin/rendering/dynamic/ColorDynamic.hpp"

#include "vklearnin/vulkan/descriptors/vkDescriptorPool.hpp"
#include "vklearnin/vulkan/descriptors/vkDescriptorSetLayout.hpp"
#include "vklearnin/vulkan/descriptors/vkDescriptorSet.hpp"

#include "vklearnin/meshes/primatives/Plane.hpp"
#include "vklearnin/textures/Texture2D.hpp"

// #define RENDER_PASS
// #define COLOR_PASS
// #define DEPTH_PASS
// #define MSAA_PASS

#define DYNAMIC_RENDERING
#define COLOR_DYNAMIC
// #define DEPTH_DYNAMIC
// #define MSAA_DYNAMIC

using namespace vkl;

// app stuff -------------------------------------------------------------------
static std::string const app_name = "Demo";
static uint32_t const app_version = 010u;

// system stuff ----------------------------------------------------------------
static vkInstance instance;
static TargetWindow target_window;
static vkSurface surface;
static vkDevice device;

static vkSwapchain swapchain;

#ifdef RENDER_PASS
static std::vector<vkFrameBuffer> frame_buffers;
#endif // RENDER_PASS

static std::vector<vkFrameSync> graphics_syncs;
static uint32_t frame_index = 0u;

static auto const instance_config = vkInstance::Config {
    .extensions = {
        VK_KHR_SURFACE_EXTENSION_NAME,
        #ifdef VKL_LINUX
            VK_KHR_XLIB_SURFACE_EXTENSION_NAME,
        #elif VKL_WINDOWS
            VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
        #endif // VKL platform
    },
    .enable_validation = true,
};

static auto features = vkPhysicalDevice::Features {
    .sampler_anisotropy = true,

#ifdef DYNAMIC_RENDERING
    .dynamic_rendering  = true,
#endif // DYNAMIC_RENDERING
};

static std::vector<char const *> const physical_device_extensions {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME,
#ifdef DYNAMIC_RENDERING
    VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME,
#endif // DYNAMIC_RENDERING
};

// pipeline stuff --------------------------------------------------------------
static vkShaderModule vert_stage;
static vkShaderModule frag_stage;
static vkGraphicsPipeline graphics_pipeline;

// render pass stuff -----------------------------------------------------------
static std::vector<vk::ClearValue> const clear_values {{
    { .color { std::array<float, 4> {{ 0.08f, 0.08f, 0.16f, 1.0f }} }},
#if defined(DEPTH_PASS) or defined(MSAA_PASS)
    { .depthStencil { .depth = 1.0f, .stencil = 1u } }
#endif // DEPTH or MSAA
}};

static vk::SampleCountFlagBits msaa_samples = vk::SampleCountFlagBits::e1;

#ifdef RENDER_PASS

#ifdef COLOR_PASS
static ColorPass color_pass;
#endif // COLOR_PASS

#ifdef DEPTH_PASS
static DepthPass depth_pass;
#endif // DEPTH_PASS

#ifdef MSAA_PASS
static MSAAPass msaa_pass;
#endif // MSAA_PASS

#endif // RENDER_PASS

#ifdef DYNAMIC_RENDERING

#ifdef COLOR_DYNAMIC
static ColorDynamic color_dynamic;
#endif // COLOR_DYNAMIC

#endif // DYNAMIC_RENDERING

// drawing stuff ---------------------------------------------------------------
static struct CameraMatrices {
    glm::mat4 view { };
    glm::mat4 proj { };
} camera_mats;

static std::vector<vkBuffer> camera_ubos;

static vkDescriptorPool descriptor_pool;
static vkDescriptorSetLayout camera_descriptor_set_layout;
static std::vector<vkDescriptorSet> camera_descriptor_sets;

static Plane plane_a;
static glm::mat4 model_mat_a;

static Plane plane_b;
static glm::mat4 model_mat_b;

static Texture2D brick_texture;
static Texture2D wood_texture;
static vkDescriptorSetLayout texture_descriptor_set_layout;
static vkDescriptorSet brick_descriptor_set;
static vkDescriptorSet wood_descriptor_set;

// =============================================================================

void vulkan_setup();
void init_rendering();
void create_swapchain();
void create_draw_data();
void create_descriptor_data();
void create_graphics_pipeline();

void destroy_graphics_pipeline();
void destroy_descriptor_data();
void destroy_draw_data();
void destroy_swapchain();
void destroy_render_pass();
void vulkan_shutdown();

void draw(vkCmdBuffer const &cmd_buffer, float run_time_s);

void recreate_swapchain();

// =============================================================================
int main() {
    vulkan_setup();
    init_rendering();
    create_swapchain();

    descriptor_pool
        .set_type_count(vk::DescriptorType::eUniformBuffer, 10u)
        .set_type_count(vk::DescriptorType::eCombinedImageSampler, 10u)
        .set_type_count(vk::DescriptorType::eStorageBuffer, 10u)
        .create(100u, device);

    create_draw_data();
    create_descriptor_data();
    create_graphics_pipeline();

    // -------------------------------------------------------------------------
    // main loop ---------------------------------------------------------------
    target_window.show();

    auto const start_time = std::chrono::steady_clock::now();
    auto loop_start = start_time;
    auto loop_end = start_time;

    uint64_t frame_count = 0u;

    while(!target_window.poll_events()) {
        auto const frame_time = loop_end - loop_start;
        auto const frame_time_s =
            static_cast<float>(frame_time.count()) / 1'000'000'000.0f;

        auto const run_time = loop_end - start_time;
        auto const run_time_s =
            static_cast<float>(run_time.count()) / 1'000'000'000.0f;

        loop_start = loop_end;

        Log::trace("{}: {:.06f}", ++frame_count, frame_time_s);

        // ---------------------------------------------------------------------
        // begin graphics commands
#ifdef RENDER_PASS
        auto const &frame_buffer   = frame_buffers[frame_index];
#endif // RENDER_PASS

#ifdef DYNAMIC_RENDERING
        auto const &swapchain_image_view = swapchain.image_views()[frame_index];
        auto const &swapchain_image_layout = swapchain.images()[frame_index].layout();
#endif // DYNAMIC_RENDERING

        auto const &gfx_sync       = graphics_syncs[frame_index];
        auto const &graphics_cmd_buffer = gfx_sync.cmd_buffer();

        // wait on queue fence, reset fence and command pool
        gfx_sync.wait_and_reset();

        // get next image index
        auto const image_index = swapchain.acquire_next_image(
            gfx_sync.wait_semaphore()
        );

        if(image_index == std::numeric_limits<uint32_t>::max()) {
            device.wait_idle();
            recreate_swapchain();
            frame_index = 0u;
            continue;
        }

        if(image_index != frame_index) {
            Log::critical(
                "Got image index {} with frame index {}.",
                image_index,
                frame_index
            );
            break;
        }

        graphics_cmd_buffer.begin_one_time_submit();

#ifdef RENDER_PASS
        // ---------------------------------------------------------------------
        // begin render pass

#ifdef COLOR_PASS
        auto &begin_info = color_pass.begin_info();
#endif // COLOR_PASS

#ifdef DEPTH_PASS
        auto &begin_info = depth_pass.begin_info();
#endif // DEPTH_PASS

#ifdef MSAA_PASS
        auto &begin_info = msaa_pass.begin_info();
#endif // MSAA_PASS

        begin_info.framebuffer = frame_buffer.native();

        graphics_cmd_buffer.begin_render_pass(begin_info);
            graphics_pipeline.bind(graphics_cmd_buffer);
            draw(graphics_cmd_buffer, run_time_s);
        graphics_cmd_buffer.end_render_pass();

#endif // RENDER_PASS

#ifdef DYNAMIC_RENDERING

#ifdef COLOR_DYNAMIC
        auto &rendering_info =
            color_dynamic.rendering_info(swapchain_image_view.native(),
                                         swapchain_image_layout);
#endif // COLOR_DYNAMIC

        graphics_cmd_buffer.begin_rendering(rendering_info);
            graphics_pipeline.bind(graphics_cmd_buffer);
            draw(graphics_cmd_buffer, run_time_s);
        graphics_cmd_buffer.end_rendering();

#endif // DYNAMIC_RENDERING

        graphics_cmd_buffer.end_recording();

        // ---------------------------------------------------------------------
        // submit graphics commands
        device.cmd_queue().submit(
            {{ graphics_cmd_buffer.native() }},
            {{
                gfx_sync.wait_semaphore()
            }},
            {{
                vk::PipelineStageFlagBits::eColorAttachmentOutput
            }},
            {{ gfx_sync.complete_semaphore() }},
            gfx_sync.in_flight_fence()
        );

        // ---------------------------------------------------------------------
        // present
        if(!device.cmd_queue().present(
            swapchain,
            gfx_sync.complete_semaphore(),
            frame_index))
        {
            device.wait_idle();
            recreate_swapchain();
            frame_index = 0u;
            continue;
        }

        frame_index = (frame_index + 1) % swapchain.image_count();
        loop_end = std::chrono::steady_clock::now();
    }

    target_window.hide();

    device.wait_idle();

    // destroy in reverse create order -----------------------------------------
    destroy_graphics_pipeline();
    destroy_descriptor_data();
    destroy_draw_data();

    descriptor_pool.destroy();

    destroy_swapchain();
    destroy_render_pass();
    vulkan_shutdown();

    return 0;
}

// =============================================================================
void vulkan_setup() {
    // instance ----------------------------------------------------------------
    instance.create(instance_config, app_name, app_version);

    // target window, surface, and physical device -----------------------------
    TargetWindow::init();
    target_window.create(app_name);

    surface.create(target_window, instance, { .enable_vsync = true });

    vkPhysicalDevice::populate_device_list(
        instance,
        surface,
        features,
        physical_device_extensions
    );

    vkPhysicalDevice::select_device(
        vk::PhysicalDeviceType::eDiscreteGpu
        // vk::PhysicalDeviceType::eIntegratedGpu
    );

    surface.get_details(vkPhysicalDevice::current_device());

    // logical device ----------------------------------------------------------
    device.create(vkPhysicalDevice::current_device());
}

void init_rendering() {
#ifdef RENDER_PASS

#ifdef COLOR_PASS
    color_pass.create(surface, clear_values, device);
#endif // COLOR_PASS

#ifdef DEPTH_PASS
    depth_pass.create(
        surface,
        clear_values,
        vkPhysicalDevice::current_device(),
        device
    );
#endif // DEPTH_PASS

#ifdef MSAA_PASS
    msaa_samples = vkPhysicalDevice::current_device().max_samples();

    msaa_pass.create(
        surface,
        clear_values,
        msaa_samples,
        vkPhysicalDevice::current_device(),
        device
    );
#endif // MSAA_PASS

#endif // RENDER_PASS


#ifdef DYNAMIC_RENDERING

#ifdef COLOR_DYNAMIC
    color_dynamic.init(surface, clear_values);
#endif // COLOR_DYNAMIC

#endif // DNYMAIC_RENDERING
}

void create_swapchain() {
    // swapchain  --------------------------------------------------------------
    swapchain.create(device, surface);

#ifdef RENDER_PASS

    // frame buffers -----------------------------------------------------------
    frame_buffers.resize(swapchain.image_count());
    for(uint32_t i = 0u; i < swapchain.image_count(); ++i) {
        frame_buffers[i].create(

#ifdef COLOR_PASS
            color_pass.render_pass(), // for color pass only (no depth)
            {{ swapchain.image_views()[i].native() }},
#endif // COLOR_PASS

#ifdef DEPTH_PASS
            depth_pass.render_pass(), // for color and depth
            {{
                swapchain.image_views()[i].native(),
                depth_pass.depth_view().native()
            }},
#endif // DEPTH_PASS

#ifdef MSAA_PASS
            msaa_pass.render_pass(), // for color, depth, and resolve
            {{
                msaa_pass.multisample_view().native(),
                msaa_pass.depth_view().native(),
                swapchain.image_views()[i].native(),
            }},
#endif // MSAA_PASS

            surface.extent(),
            device
        );
    }

#endif // RENDER_PASS

    graphics_syncs.resize(swapchain.image_count());
    for(auto &sync : graphics_syncs) {
        sync.create(device);
    }
}

void create_draw_data() {
    // models ------------------------------------------------------------------
    plane_a.create(
        vkPhysicalDevice::current_device(),
        device
    );

    plane_b.create(
        vkPhysicalDevice::current_device(),
        device
    );

    // camera matrices and buffers ---------------------------------------------
    camera_ubos.resize(swapchain.image_count());

    for(auto &ubo : camera_ubos) {
        ubo.create(
            sizeof(CameraMatrices),
            vk::BufferUsageFlagBits::eUniformBuffer,
            vkPhysicalDevice::current_device(),
            device
        );

        ubo.allocate(vk::MemoryPropertyFlagBits::eHostVisible
                     | vk::MemoryPropertyFlagBits::eHostCoherent);
    }

    // textures ----------------------------------------------------------------
    brick_texture.create(
        "textures/brick_050d.jpg",
        vkImage::Details {
            .type = vk::ImageType::e2D,
            .samples = vk::SampleCountFlagBits::e1,
            .usage_flags = (
                vk::ImageUsageFlagBits::eSampled
                | vk::ImageUsageFlagBits::eTransferDst
                | vk::ImageUsageFlagBits::eTransferSrc
            ),
            .memory_flags = vk::MemoryPropertyFlagBits::eDeviceLocal,
            .aspect_flags = vk::ImageAspectFlagBits::eColor,
            .array_layers = 1u,
        },
        vk::ImageViewType::e2D,
        vkSampler::Filters {
            .mag = vk::Filter::eLinear,
            .min = vk::Filter::eLinear,
            .mip = vk::SamplerMipmapMode::eLinear,
        },
        vkSampler::AddressMode {
            .u = vk::SamplerAddressMode::eRepeat,
            .v = vk::SamplerAddressMode::eRepeat
        },
        vkPhysicalDevice::current_device(),
        device
    );

    wood_texture.create(
        "textures/woodfloor_051_d.jpg",
        vkImage::Details {
            .type = vk::ImageType::e2D,
            .samples = vk::SampleCountFlagBits::e1,
            .usage_flags = (
                vk::ImageUsageFlagBits::eSampled
                | vk::ImageUsageFlagBits::eTransferDst
                | vk::ImageUsageFlagBits::eTransferSrc
            ),
            .memory_flags = vk::MemoryPropertyFlagBits::eDeviceLocal,
            .aspect_flags = vk::ImageAspectFlagBits::eColor,
            .array_layers = 1u,
        },
        vk::ImageViewType::e2D,
        vkSampler::Filters {
            .mag = vk::Filter::eLinear,
            .min = vk::Filter::eLinear,
            .mip = vk::SamplerMipmapMode::eLinear,
        },
        vkSampler::AddressMode {
            .u = vk::SamplerAddressMode::eRepeat,
            .v = vk::SamplerAddressMode::eRepeat
        },
        vkPhysicalDevice::current_device(),
        device
    );

}

void create_descriptor_data() {
    // camera descriptor sets --------------------------------------------------
    camera_descriptor_set_layout
        .add_binding(
            0u,
            vk::DescriptorType::eUniformBuffer,
            1u,
            vk::ShaderStageFlagBits::eVertex)
        .create(device);

    camera_descriptor_sets.resize(swapchain.image_count());
    for(uint32_t i = 0u; i < camera_descriptor_sets.size(); ++i) {
        camera_descriptor_sets[i].allocate(
            camera_descriptor_set_layout,
            descriptor_pool,
            device
        );

        camera_descriptor_sets[i]
            .add_update(
                vk::DescriptorBufferInfo {
                    .buffer = camera_ubos[i].native(),
                    .offset = 0u,
                    .range = VK_WHOLE_SIZE,
                },
                0u,
                vk::DescriptorType::eUniformBuffer
            )
            .update();
    }

    // texture descriptor sets -------------------------------------------------
    texture_descriptor_set_layout
        .add_binding(
            0u,
            vk::DescriptorType::eCombinedImageSampler,
            1u,
            vk::ShaderStageFlagBits::eFragment)
        .create(device);

    brick_descriptor_set.allocate(
        texture_descriptor_set_layout,
        descriptor_pool,
        device
    );

    brick_descriptor_set
        .add_update(
            vk::DescriptorImageInfo {
                .sampler = brick_texture.sampler().native(),
                .imageView = brick_texture.view().native(),
                .imageLayout = brick_texture.image().layout()
            },
            0u,
            vk::DescriptorType::eCombinedImageSampler
        )
        .update();

    wood_descriptor_set.allocate(
        texture_descriptor_set_layout,
        descriptor_pool,
        device
    );

    wood_descriptor_set
        .add_update(
            vk::DescriptorImageInfo {
                .sampler = wood_texture.sampler().native(),
                .imageView = wood_texture.view().native(),
                .imageLayout = wood_texture.image().layout()
            },
            0u,
            vk::DescriptorType::eCombinedImageSampler
        )
        .update();
}

void create_graphics_pipeline() {
    // shaders and pipeline after descriptor sets ------------------------------
    vert_stage.create("shaders/04texture.vert", device);
    frag_stage.create("shaders/02texture.frag", device);

    graphics_pipeline
        .add_shader(vert_stage)
        .add_shader(frag_stage)
        .add_push_constant(
            vk::ShaderStageFlagBits::eVertex,
            sizeof(glm::mat4)
        )
        .add_descriptor_set_layout(camera_descriptor_set_layout.native())
        .add_descriptor_set_layout(texture_descriptor_set_layout.native())

#ifdef RENDER_PASS

#ifdef COLOR_PASS
        .add_render_pass(color_pass.render_pass())
#endif // COLOR_PASS

#ifdef DEPTH_PASS
        .add_render_pass(depth_pass.render_pass())
#endif // DEPTH_PASS

#ifdef MSAA_PASS
        .add_render_pass(msaa_pass.render_pass())
#endif // MSAA_PASS

#endif // RENDER_PASS

        .create(vkGraphicsPipeline::Config {
                .viewport_extent = surface.extent(),
                .topology = vk::PrimitiveTopology::eTriangleList,
                .sample_flags = msaa_samples,

#if defined(DEPTH_PASS) || defined(MSAA_PASS)
                .enable_depth_test = vk::True,
#endif // render passes that use depth testing

#ifdef DYNAMIC_RENDERING
                .rendering_create_info = &color_dynamic.pipeline_create_info(),
#endif // DYNAMIC_RENDERING

            },
            device
        );

}

void destroy_graphics_pipeline() {
    frag_stage.destroy();
    vert_stage.destroy();

    graphics_pipeline.destroy();
}

void destroy_descriptor_data() {
    texture_descriptor_set_layout.destroy();
    camera_descriptor_set_layout.destroy();
}

void destroy_draw_data() {
    wood_texture.destroy();
    brick_texture.destroy();

    for(auto &ubo : camera_ubos) {
        ubo.destroy();
    }

    plane_b.destroy();
    plane_a.destroy();
}

void destroy_swapchain() {
    for(auto &sync : graphics_syncs) {
        sync.destroy();
    }

#ifdef RENDER_PASS
    for(auto &fb : frame_buffers) {
        fb.destroy();
    }
#endif // RENDER_PASS

    swapchain.destroy();
}

void destroy_render_pass() {
#ifdef COLOR_PASS
    color_pass.destroy();
#endif // COLOR_PASS

#ifdef DEPTH_PASS
    depth_pass.destroy();
#endif // DEPTH_PASS

#ifdef MSAA_PASS
    msaa_pass.destroy();
#endif // MSAA_PASS
}

void vulkan_shutdown() {
    device.destroy();

    vkPhysicalDevice::clear_device_list();

    surface.destroy();

    target_window.destroy();
    TargetWindow::shutdown();

    instance.destroy();
}

void draw(vkCmdBuffer const &cmd_buffer, float run_time_s) {
    camera_descriptor_sets[frame_index].bind(
        graphics_pipeline,
        0u,
        cmd_buffer
    );

    camera_mats.view = glm::lookAtRH(
        glm::vec3{ 0.0f,  0.0f,  2.0f }, // camera position
        glm::vec3{ 0.0f,  0.0f,  0.0f }, // camera target
        glm::vec3{ 0.0f,  1.0f,  0.0f }  // camera "up"
    );

    camera_mats.proj = glm::infinitePerspectiveRH_ZO(
        glm::radians(45.0f),
        surface.aspect_ratio(),
        0.1f
    );

    camera_ubos[frame_index].fill_buffer(&camera_mats);

    // ---------- bind brick texture  ----------
    brick_descriptor_set.bind(graphics_pipeline, 1u, cmd_buffer);

    // ---------- draw object 1 ----------
    model_mat_a = glm::rotate(
        glm::mat4(1.0f),
        run_time_s * glm::radians(22.5f),
        glm::vec3(0.0f, 0.0f, 1.0f)
    );

    graphics_pipeline.send_push_constants(
        vk::ShaderStageFlagBits::eVertex,
        0u,
        sizeof(glm::mat4),
        &model_mat_a,
        cmd_buffer
    );

    plane_a.bind(cmd_buffer);
    plane_a.draw(cmd_buffer);

    // ---------- bind wood texture  ----------
    wood_descriptor_set.bind(graphics_pipeline, 1u, cmd_buffer);

    // ---------- draw object 2 ----------
    model_mat_b = glm::rotate(
        glm::translate(glm::vec3(0.5f, 0.5f, -1.0f)),
        run_time_s * glm::radians(-11.25f),
        glm::vec3(0.0f, 0.0f, 1.0f)
    );

    graphics_pipeline.send_push_constants(
        vk::ShaderStageFlagBits::eVertex,
        0u,
        sizeof(glm::mat4),
        &model_mat_b,
        cmd_buffer
    );

    plane_b.bind(cmd_buffer);
    plane_b.draw(cmd_buffer);
}

void recreate_swapchain() {
    for(auto &sync : graphics_syncs) {
        sync.destroy();
    }

#ifdef RENDER_PASS
    for(auto &fb : frame_buffers) {
        fb.destroy();
    }
#endif // RENDER_PASS

#ifdef DEPTH_PASS
    depth_pass.destroy_swapchain_resources();
#endif // DEPTH_PASS

#ifdef MSAA_PASS
    msaa_pass.destroy_swapchain_resources();
#endif // MSAA_PASS

    swapchain.destroy();

    // ...

    surface.get_details(vkPhysicalDevice::current_device());

    graphics_pipeline.update_dimensions(surface.extent(), { });

    swapchain.create(device, surface);

#ifdef RENDER_PASS

#ifdef COLOR_PASS
    color_pass.update_render_area(surface);
#endif // COLOR_PASS

#ifdef DEPTH_PASS
    depth_pass.create_swapchain_resources(
        surface,
        vkPhysicalDevice::current_device(),
        device
    );
#endif // DEPTH_PASS

#ifdef MSAA_PASS
    msaa_pass.create_swapchain_resources(
        surface,
        vkPhysicalDevice::current_device(),
        device
    );
#endif // MSAA_PASS

    frame_buffers.resize(swapchain.image_count());
    for(uint32_t i = 0u; i < swapchain.image_count(); ++i) {
        frame_buffers[i].create(

#ifdef COLOR_PASS
            color_pass.render_pass(),
            {{ swapchain.image_views()[i].native() }},
#endif // COLOR_PASS

#ifdef DEPTH_PASS
            depth_pass.render_pass(),
            {{
                swapchain.image_views()[i].native(),
                depth_pass.depth_view().native()
            }},
#endif // DEPTH_PASS

#ifdef MSAA_PASS
            msaa_pass.render_pass(),
            {{
                msaa_pass.multisample_view().native(),
                msaa_pass.depth_view().native(),
                swapchain.image_views()[i].native(),
            }},
#endif // MSAA_PASS

            surface.extent(),
            device
        );
    }

#endif // RENDER_PASS

    graphics_syncs.resize(swapchain.image_count());
    for(auto &sync : graphics_syncs) {
        sync.create(device);
    }

    camera_mats.proj = glm::infinitePerspectiveRH_ZO(
        glm::radians(45.0f),
        surface.aspect_ratio(),
        0.1f
    );
}


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

#include "vklearnin/render_passes/ColorPass.hpp"
#include "vklearnin/render_passes/ColorDepthPass.hpp"
#include "vklearnin/render_passes/ColorDepthResolvePass.hpp"

#include "vklearnin/vulkan/descriptors/vkDescriptorPool.hpp"
#include "vklearnin/vulkan/descriptors/vkDescriptorSetLayout.hpp"
#include "vklearnin/vulkan/descriptors/vkDescriptorSet.hpp"

#include "vklearnin/meshes/primatives/Plane.hpp"
#include "vklearnin/textures/Texture2D.hpp"

#define DRAW_TEXTURES
// #define DRAW_PARTICLES
#define MSAA

using namespace vkl;

// system stuff ----------------------------------------------------------------
static vkInstance instance;
static TargetWindow target_window;
static vkSurface surface;
static vkDevice device;

static vkSwapchain swapchain;
static std::vector<vkFrameBuffer> frame_buffers;
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

static auto features = vk::PhysicalDeviceFeatures2 {
    .pNext = nullptr,
    .features {
        .samplerAnisotropy = VK_TRUE,
    }
};

static std::vector<char const *> const physical_device_extensions {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME,
};

// pipeline stuff --------------------------------------------------------------
static vkShaderModule vert_stage;
static vkShaderModule frag_stage;
static vkGraphicsPipeline graphics_pipeline;

// render pass stuff -----------------------------------------------------------
static std::vector<vk::ClearValue> const clear_values {{
    { .color { std::array<float, 4> {{ 0.08f, 0.08f, 0.16f, 1.0f }} }},
    { .depthStencil { .depth = 1.0f, .stencil = 1u } }
}};

// static ColorPass color_pass;
#ifdef MSAA
static vk::SampleCountFlagBits msaa_samples = vk::SampleCountFlagBits::e1;
static ColorDepthResolvePass color_depth_resolve_pass;
#else
static ColorDepthPass color_depth_pass;
#endif // MSAA

// drawing stuff ---------------------------------------------------------------
static struct CameraMatrices {
    glm::mat4 view { };
    glm::mat4 proj { };
} camera_mats;

static std::vector<vkBuffer> camera_ubos;

static Plane plane_a;
static glm::mat4 model_mat_a;

static Plane plane_b;
static glm::mat4 model_mat_b;

static vkDescriptorPool descriptor_pool;
static vkDescriptorSetLayout camera_descriptor_set_layout;
static std::vector<vkDescriptorSet> camera_descriptor_sets;

static Texture2D brick_texture;
static Texture2D wood_texture;
static vkDescriptorSetLayout texture_descriptor_set_layout;
static vkDescriptorSet brick_descriptor_set;
static vkDescriptorSet wood_descriptor_set;

// particles and compute -------------------------------------------------------
struct Particle {
    glm::vec2 position;
    glm::vec2 velocity;
    glm::vec4 color;

    static std::vector<vk::VertexInputBindingDescription> const bindings;
    static std::vector<vk::VertexInputAttributeDescription> const attributes;
};

static uint32_t const PARTICLE_COUNT = 8196u;

std::vector<vk::VertexInputBindingDescription> const Particle::bindings {{
    .binding = 0u,
    .stride = sizeof(Particle),
    .inputRate = vk::VertexInputRate::eVertex,
}};

std::vector<vk::VertexInputAttributeDescription> const Particle::attributes {
    vk::VertexInputAttributeDescription {
        .location = 0,
        .binding = 0,
        .format = vk::Format::eR32G32Sfloat,
        .offset = offsetof(Particle, position),
    },
    vk::VertexInputAttributeDescription {
        .location = 1,
        .binding = 0,
        .format = vk::Format::eR32G32B32A32Sfloat,
        .offset = offsetof(Particle, color),
    }
};

static std::vector<vkBuffer> particle_ubos;
static std::vector<vkBuffer> particle_ssbos;

static vkDescriptorSetLayout particle_descriptor_set_layout;
static std::vector<vkDescriptorSet> particle_descriptor_sets;

static vkShaderModule compute_stage;
static vkComputePipeline compute_pipeline;

static std::vector<vkFrameSync> compute_syncs;

// =============================================================================

void vulkan_setup();
void create_render_pass();
void create_swapchain();
void create_draw_data();
void create_descriptor_data();
void create_graphics_pipeline();

void create_particle_descriptors();
void create_particle_pipeline();

void destroy_particle_pipeline();
void destroy_particle_descriptors();

void destroy_graphics_pipeline();
void destroy_descriptor_data();
void destroy_draw_data();
void destroy_swapchain();
void destroy_render_pass();
void vulkan_shutdown();

void run_particle_compute(vkFrameSync const &compute_sync,
                          vkCmdBuffer const &cmd_buffer,
                          float const frame_time_s);

void draw(vkCmdBuffer const &cmd_buffer, float run_time_s);

void recreate_swapchain();

// =============================================================================
int main() {
    vulkan_setup();
    create_render_pass();
    create_swapchain();

    descriptor_pool
        .set_type_count(vk::DescriptorType::eUniformBuffer, 10u)
        .set_type_count(vk::DescriptorType::eCombinedImageSampler, 10u)
        .set_type_count(vk::DescriptorType::eStorageBuffer, 10u)
        .create(100u, device);

#ifdef DRAW_TEXTURES
    create_draw_data();
    create_descriptor_data();
#endif // DRAW_TEXTURES

    create_graphics_pipeline();

#ifdef DRAW_PARTICLES
    create_particle_descriptors();
    create_particle_pipeline();
#endif // DRAW_PARTICLES

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

#ifdef DRAW_PARTICLES
        // ---------------------------------------------------------------------
        // begin compute commands
        auto const &compute_sync       = compute_syncs[frame_index];
        auto const &compute_cmd_buffer = compute_sync.cmd_buffer();

        run_particle_compute(compute_sync, compute_cmd_buffer, frame_time_s);
#endif // DRAW_PARTICLES

        // ---------------------------------------------------------------------
        // begin graphics commands
        auto const &frame_buffer   = frame_buffers[frame_index];
        auto const &gfx_sync       = graphics_syncs[frame_index];
        auto const &gfx_cmd_buffer = gfx_sync.cmd_buffer();

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

        gfx_cmd_buffer.begin_one_time_submit();

        // ---------------------------------------------------------------------
        // begin render pass
        // color_pass.begin(frame_buffer, clear_values, gfx_cmd_buffer);

#ifdef MSAA
        color_depth_resolve_pass.begin(frame_buffer, clear_values, gfx_cmd_buffer);
#else
        color_depth_pass.begin(frame_buffer, clear_values, gfx_cmd_buffer);
#endif // MSAA

        graphics_pipeline.bind(gfx_cmd_buffer);

#ifdef DRAW_TEXTURES
        draw(gfx_cmd_buffer, run_time_s);
#endif // DRAW_TEXTURES

#ifdef DRAW_PARTICLES
        std::array<vk::DeviceSize, 1u> offsets { 0u };
        gfx_cmd_buffer.native().bindVertexBuffers(
            0u,
            1u,
            &particle_ssbos[frame_index].native(),
            offsets.data()
        );

        gfx_cmd_buffer.native().draw(
            PARTICLE_COUNT,
            1u,
            0u,
            0u
        );
#endif // DRAW_PARTICLES

        gfx_cmd_buffer.end_render_pass();
        gfx_cmd_buffer.end_recording();

        // ---------------------------------------------------------------------
        // submit graphics commands
        device.cmd_queue().submit(
            {{ gfx_cmd_buffer.native() }},
            {{
#ifdef DRAW_PARTICLES
                compute_sync.complete_semaphore(),
#endif // DRAW_PARTICLES
                gfx_sync.wait_semaphore()
            }},
            {{
#ifdef DRAW_PARTICLES
                vk::PipelineStageFlagBits::eVertexInput,
#endif // DRAW_PARTICLES
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
#ifdef DRAW_PARTICLES
    destroy_particle_pipeline();
    destroy_particle_descriptors();
#endif // DRAW_PARTICLES

    destroy_graphics_pipeline();

#ifdef DRAW_TEXTURES
    destroy_descriptor_data();
    destroy_draw_data();
#endif // DRAW_TEXTURES

    descriptor_pool.destroy();

    destroy_swapchain();
    destroy_render_pass();
    vulkan_shutdown();

    return 0;
}

// =============================================================================
void vulkan_setup() {
    // instance ----------------------------------------------------------------
    instance.create(instance_config, VKL_NAME, 010u);

    // target window, surface, and physical device -----------------------------
    TargetWindow::init();
    target_window.create(std::format("{} v{}", VKL_NAME, VKL_VERSION_STRING));

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

void create_render_pass() {
    // color_pass.create(surface, device);

#ifdef MSAA
    msaa_samples = vkPhysicalDevice::current_device().max_samples();

    color_depth_resolve_pass.create(
        surface,
        vkPhysicalDevice::current_device(),
        device,
        msaa_samples
    );
#else
    color_depth_pass.create(
        surface,
        vkPhysicalDevice::current_device(),
        device
    );
#endif // MSAA
}

void create_swapchain() {
    // swapchain  --------------------------------------------------------------
    swapchain.create(device, surface);

    // frame buffers -----------------------------------------------------------
    frame_buffers.resize(swapchain.image_count());
    for(uint32_t i = 0u; i < swapchain.image_count(); ++i) {
        frame_buffers[i].create(

            // color_pass.render_pass(), // for color pass only (no depth)
            // {{ swapchain.image_views()[i].native() }},

#ifdef MSAA
            color_depth_resolve_pass.render_pass(), // for color, depth, and resolve
            {{
                color_depth_resolve_pass.multisample_view().native(),
                color_depth_resolve_pass.depth_view().native(),
                swapchain.image_views()[i].native(),
            }},
#else
            color_depth_pass.render_pass(), // for color and depth
            {{
                swapchain.image_views()[i].native(),
                color_depth_pass.depth_view().native()
            }},
#endif // MSAA

            surface.extent(),
            device
        );
    }

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

#ifdef DRAW_TEXTURES
    vert_stage.create("shaders/04texture.vert", device);
    frag_stage.create("shaders/02texture.frag", device);
#endif // DRAW_TEXTURES

#ifdef DRAW_PARTICLES
    vert_stage.create("shaders/05particles.vert", device);
    frag_stage.create("shaders/05particles.frag", device);
#endif // DRAW_PARTICLES

    graphics_pipeline
        .add_shader(vert_stage)
        .add_shader(frag_stage)

#ifdef DRAW_TEXTURES
        .add_push_constant(
            vk::ShaderStageFlagBits::eVertex,
            sizeof(glm::mat4)
        )
        .add_descriptor_set_layout(camera_descriptor_set_layout.native())
        .add_descriptor_set_layout(texture_descriptor_set_layout.native())
#endif // DRAW_TEXTURES

        // .add_render_pass(color_pass.render_pass())

#ifdef MSAA
        .add_render_pass(color_depth_resolve_pass.render_pass())
#else
        .add_render_pass(color_depth_pass.render_pass())
#endif // MSAA

        .create(vkGraphicsPipeline::Config {
                .viewport_extent = surface.extent(),

#ifdef DRAW_TEXTURES
                .topology = vk::PrimitiveTopology::eTriangleList,
#endif // DRAW_TEXTUREs

#ifdef DRAW_PARTICLES
                .topology = vk::PrimitiveTopology::ePointList,
#endif // DRAW_PARTICLES

#ifdef MSAA
                .sample_flags = msaa_samples,
#else
                .sample_flags = vk::SampleCountFlagBits::e1,
#endif // MSAA

#ifdef DRAW_TEXTURES
                .enable_depth_test = VK_TRUE,
#endif // DRAW_TEXTURES
            },
            device
        );
}

void create_particle_descriptors() {
    // particle buffers --------------------------------------------------------
    std::vector<Particle> particles;
    particles.resize(PARTICLE_COUNT);

    std::default_random_engine rengine((unsigned)time(nullptr));
    std::uniform_real_distribution<float> rdist(0.0f, 1.0f);

    for(auto &particle : particles) {
        float r = 0.25f * ::sqrt(rdist(rengine));
        float theta = rdist(rengine) * 2 * 3.14159265358979323846;
        float x = r * ::cos(theta) * surface.aspect_ratio();
        float y = r * ::sin(theta);
        particle.position = glm::vec2(x, y);
        particle.velocity = glm::normalize(glm::vec2(x,y)) * 0.5f;
        particle.color = glm::vec4(rdist(rengine), rdist(rengine), rdist(rengine), 1.0f);
    }

    particle_ubos.resize(swapchain.image_count());
    for(auto &ubo : particle_ubos) {
        ubo.create(
            sizeof(float),
            vk::BufferUsageFlagBits::eUniformBuffer,
            vkPhysicalDevice::current_device(),
            device
        );

        ubo.allocate(vk::MemoryPropertyFlagBits::eHostVisible
                     | vk::MemoryPropertyFlagBits::eHostCoherent);
    }

    particle_ssbos.resize(swapchain.image_count());
    for(auto &ssbo : particle_ssbos) {
        ssbo.create(
            sizeof(Particle) * PARTICLE_COUNT,
            vk::BufferUsageFlagBits::eStorageBuffer
            | vk::BufferUsageFlagBits::eVertexBuffer
            | vk::BufferUsageFlagBits::eTransferDst,
            vkPhysicalDevice::current_device(),
            device
        );

        ssbo.allocate(vk::MemoryPropertyFlagBits::eHostVisible
                      | vk::MemoryPropertyFlagBits::eHostCoherent);

        ssbo.fill_buffer(particles.data());
    }

    // particle descriptors ----------------------------------------------------
    particle_descriptor_set_layout
        .add_binding(
            0u,
            vk::DescriptorType::eUniformBuffer,
            1u,
            vk::ShaderStageFlagBits::eCompute)
        .add_binding(
            1u,
            vk::DescriptorType::eStorageBuffer,
            1u,
            vk::ShaderStageFlagBits::eCompute)
        .add_binding(
            2u,
            vk::DescriptorType::eStorageBuffer,
            1u,
            vk::ShaderStageFlagBits::eCompute)
        .create(device);

    particle_descriptor_sets.resize(swapchain.image_count());
    for(uint32_t i = 0u; i < particle_descriptor_sets.size(); ++i) {
        particle_descriptor_sets[i].allocate(
            particle_descriptor_set_layout,
            descriptor_pool,
            device
        );

        auto const last_frame_index = (i - 1u) % swapchain.image_count();

        particle_descriptor_sets[i]
            .add_update(
                vk::DescriptorBufferInfo {
                    .buffer = particle_ubos[i].native(),
                    .offset = 0u,
                    .range = VK_WHOLE_SIZE,
                },
                0u,
                vk::DescriptorType::eUniformBuffer
            )
            .add_update(
                vk::DescriptorBufferInfo {
                    .buffer = particle_ssbos[last_frame_index].native(),
                    .offset = 0u,
                    .range = VK_WHOLE_SIZE,
                },
                1u,
                vk::DescriptorType::eStorageBuffer
            )
            .add_update(
                vk::DescriptorBufferInfo {
                    .buffer = particle_ssbos[i].native(),
                    .offset = 0u,
                    .range = VK_WHOLE_SIZE,
                },
                2u,
                vk::DescriptorType::eStorageBuffer
            )
            .update();
    }
}

void create_particle_pipeline() {
    // particle pipeline -------------------------------------------------------
    compute_stage.create("shaders/05particles.comp", device);
    compute_pipeline
        .add_shader(compute_stage)
        .add_descriptor_set_layout(particle_descriptor_set_layout.native())
        .create(device);

    // particle sync -----------------------------------------------------------
    compute_syncs.resize(swapchain.image_count());
    for(auto &sync : compute_syncs) {
        sync.create(device);
    }
}

void destroy_particle_pipeline() {
    for(auto &sync : compute_syncs) {
        sync.destroy();
    }
    compute_pipeline.destroy();
    compute_stage.destroy();
}

void destroy_particle_descriptors() {
    particle_descriptor_set_layout.destroy();
    for(auto &buffer : particle_ssbos) {
        buffer.destroy();
    }
    for(auto &buffer : particle_ubos) {
        buffer.destroy();
    }
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

    for(auto &fb : frame_buffers) {
        fb.destroy();
    }

    swapchain.destroy();
}

void destroy_render_pass() {
#ifdef MSAA
    color_depth_resolve_pass.destroy();
#else
    color_depth_pass.destroy();
#endif // MSAA
    // color_pass.destroy();
}

void vulkan_shutdown() {
    device.destroy();

    vkPhysicalDevice::clear_device_list();

    surface.destroy();

    target_window.destroy();
    TargetWindow::shutdown();

    instance.destroy();
}

void run_particle_compute(vkFrameSync const &compute_sync, vkCmdBuffer const &cmd_buffer, float const frame_time_s) {
    compute_sync.wait_and_reset();

    cmd_buffer.begin_one_time_submit();
    compute_pipeline.bind(cmd_buffer);

    particle_ubos[frame_index].fill_buffer(&frame_time_s);

    particle_descriptor_sets[frame_index].bind(
        compute_pipeline,
        0u,
        cmd_buffer
    );

    cmd_buffer.dispatch({
        .x = PARTICLE_COUNT / 256u,
        .y = 1u,
        .z = 1u
    });

    cmd_buffer.end_recording();

    device.cmd_queue().submit(
        {{ cmd_buffer.native() }},
        { },
        { },
        {{ compute_sync.complete_semaphore() }},
        compute_sync.in_flight_fence()
    );
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

    for(auto &fb : frame_buffers) {
        fb.destroy();
    }

#ifdef MSAA
    color_depth_resolve_pass.destroy_swapchain_resources();
#else
    color_depth_pass.destroy_swapchain_resources();
#endif // MSAA

    swapchain.destroy();

    // ...

    surface.get_details(vkPhysicalDevice::current_device());

    graphics_pipeline.update_dimensions(surface.extent(), { });

    swapchain.create(device, surface);

    // color_pass.update_render_area(surface);

#ifdef MSAA
    color_depth_resolve_pass.create_swapchain_resources(
        surface,
        vkPhysicalDevice::current_device(),
        device
    );
#else
    color_depth_pass.create_swapchain_resources(
        surface,
        vkPhysicalDevice::current_device(),
        device
    );
#endif // MSAA

    frame_buffers.resize(swapchain.image_count());
    for(uint32_t i = 0u; i < swapchain.image_count(); ++i) {
        frame_buffers[i].create(

            // color_pass.render_pass(),
            // {{ swapchain.image_views()[i].native() }},
#ifdef MSAA
            color_depth_resolve_pass.render_pass(),
            {{
                color_depth_resolve_pass.multisample_view().native(),
                color_depth_resolve_pass.depth_view().native(),
                swapchain.image_views()[i].native(),
            }},
#else
            color_depth_pass.render_pass(),
            {{
                swapchain.image_views()[i].native(),
                color_depth_pass.depth_view().native()
            }},
#endif // MSAA

            surface.extent(),
            device
        );
    }

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


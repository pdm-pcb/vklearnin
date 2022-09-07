#include "vklearnin/common.hpp"
#include "vklearnin/Instance.hpp"
#include "vklearnin/CommandStructures/CommandQueues.hpp"
#include "vklearnin/Swapchain.hpp"
#include "vklearnin/Pipeline.hpp"
#include "vklearnin/Buffers/Framebuffers.hpp"
#include "vklearnin/RenderLoop.hpp"
#include "vklearnin/Shaders/Buffers/BufferObject.hpp"
#include "vklearnin/Shaders/Buffers/UBOList.hpp"
#include "vklearnin/DescriptorSets/DescriptorSets.hpp"
#include "vklearnin/Textures/Texture2D.hpp"
#include "vklearnin/Buffers/DepthBuffer.hpp"
#include "vklearnin/Models/Model.hpp"

#if defined(__linux__)
    #include "vklearnin/Platform/X11/X11Window.hpp"
    using Window = X11Window;
#elif defined(_WIN32)
    #include "vklearnin/Platform/Win32/Win32Window.hpp"
    using Window = Win32Window;
#endif

int main() {
    ConsoleLog::init();
    EventBroker::init();

    Instance instance(true);
    instance.init_instance();
    instance.init_physical_device();

    // =========================================================================
    // Window, command pool, and command queues
    Window window(instance.vulkan_instance());

    // both the window itself and the window's surface are required for
    // establishing a command queue family for the Vulkan instance
    window.init_window();
    window.init_surface();
    
    // track down the desired queue families
    CommandQueues command_queues(window.surface(), instance);
    command_queues.init_families();
    command_queues.init_queue_info();

    // with the physical device set up and queue family chosen, the logical
    // device can be created
    instance.init_logical_device(command_queues);

    // now that there's a logical device in place, go a head and initialize a
    // command pool, queue, and command buffer
    command_queues.init_pools();
    command_queues.init_queues();
    command_queues.init_buffers();


    // =========================================================================
    // Model vertex/texture data -----------------------------------------------
    Model asteroid01(
        "../../assets/meshes/asteroids/asteroid01.gltf",
        glm::vec3{ -6.0f, 0.0f, 0.0f},
        "../../assets/textures/asteroids/asteroid01_diffuse.png",
        command_queues,
        instance
    );

    asteroid01.populate_buffers(),
    asteroid01.init_texture_image_view();
    asteroid01.init_texture_sampler();

    Model asteroid02(
        "../../assets/meshes/asteroids/asteroid02.gltf",
        glm::vec3{ -2.0f, 0.0f, 0.0f},
        "../../assets/textures/asteroids/asteroid02_diffuse.png",
        command_queues,
        instance
    );

    asteroid02.populate_buffers(),
    asteroid02.init_texture_image_view();
    asteroid02.init_texture_sampler();
    
    Model asteroid03(
        "../../assets/meshes/asteroids/asteroid03.gltf",
        glm::vec3{ 2.0f, 0.0f, 0.0f},
        "../../assets/textures/asteroids/asteroid03_diffuse.png",
        command_queues,
        instance
    );

    asteroid03.populate_buffers(),
    asteroid03.init_texture_image_view();
    asteroid03.init_texture_sampler();
    
    Model asteroid04(
        "../../assets/meshes/asteroids/asteroid04.gltf",
        glm::vec3{ 6.0f, 0.0f, 0.0f},
        "../../assets/textures/asteroids/asteroid04_diffuse.png",
        command_queues,
        instance
    );

    asteroid04.populate_buffers(),
    asteroid04.init_texture_image_view();
    asteroid04.init_texture_sampler();

    std::vector<Model *> models {
        &asteroid01,
        &asteroid02,
        &asteroid03,
        &asteroid04
    };

    std::vector<Texture2D const *> textures {
        asteroid01.texture(),
        asteroid02.texture(),
        asteroid03.texture(),
        asteroid04.texture()
    };
    
    // =========================================================================
    // the swapchain will use the function pointers gathered by the instance,
    // as well as details of the window's surface
    Swapchain swapchain(instance, window.surface());

    swapchain.init_color_format();  // presumably 32-bit SRGB
    swapchain.init_present_modes(); // presumably FIFO/v-sync
    swapchain.init_extent({         // just the window size for now
        window.width(),
        window.height()
    });
    swapchain.init_swapchain(command_queues); // and, go!
    swapchain.init_swapchain_images(); // should give us two images for writing
    swapchain.init_image_views();      // views to interface with the images

    // =========================================================================
    // Uniform Buffer Object(s)
    UniformBufferObject per_frame_ubo(sizeof(VPMatrices), 0u, instance);
    per_frame_ubo.init_buffers("per_frame_ubo");

    // calculate what we need for the dynamic UBO to work
	auto min_ubo_alignment = instance.min_ubo_alignment();
	auto offset = static_cast<uint32_t>(sizeof(glm::mat4));
	if(min_ubo_alignment > 0u) {
		offset =
            (offset + min_ubo_alignment - 1) &
            ~(min_ubo_alignment - 1);
	}
	auto buffer_size = static_cast<uint32_t>(models.size() * offset);

    UniformBufferObject per_object_ubo(buffer_size, offset, instance);
    per_object_ubo.init_buffers("per_object_ubo");

    UBOList ubo_list {
        per_frame_ubo,
        per_object_ubo
    };

    // =========================================================================
    // Descriptor Sets
    PerFrameDescriptors per_frame_descs(instance.logical_device());
    per_frame_descs.init_layout();
    per_frame_descs.init_pool();
    per_frame_descs.init_sets(per_frame_ubo);
    
    PerPassDescriptors per_pass_descs;

    PerMaterialDescriptors per_material_descs(
        static_cast<uint32_t>(textures.size()),
        instance.logical_device()
    );
    per_material_descs.init_layout();
    per_material_descs.init_pool();
    per_material_descs.init_sets(textures);
    
    PerObjectDescriptors per_object_descs(
        static_cast<uint32_t>(models.size()),
        instance.logical_device()
    );
    per_object_descs.init_layout();
    per_object_descs.init_pool();
    per_object_descs.init_sets(per_object_ubo);

    DescriptorSets desc_sets {
        per_frame_descs,
        per_pass_descs,
        per_material_descs,
        per_object_descs
    };

    // =========================================================================
    // shaderc's Compiler::Compiler() appears to have an 80 byte memory leak,
    // so no online compiling for now.
    Pipeline pipeline(instance);
    pipeline.vertex_from_binary("../../assets/shaders/shader.vert.spv");
    pipeline.fragment_from_binary("../../assets/shaders/shader.frag.spv");

    pipeline.init_render_passes(swapchain, MSAA_SAMPLES);
    pipeline.init_layout({
        desc_sets.per_frame.layout(),
        desc_sets.per_material.layout(),
        desc_sets.per_object.layout(),
    });
    pipeline.init_pipeline(swapchain);

    // =========================================================================
    // Only need two render targets for now
    Framebuffers framebuffers(instance.logical_device());
    framebuffers.init_buffers(swapchain, pipeline);

    // =========================================================================
    // The business end
    RenderLoop render_loop(instance.logical_device(), window, command_queues);
    render_loop.init_synchronization();

    bool carry_on = true;
    while(carry_on) {
        carry_on = render_loop.run(
            instance,
            swapchain,
            ubo_list,
            pipeline,
            framebuffers,
            desc_sets,
            models
        );

        // ...!
    }

    EventBroker::shutdown();

    return 0;
}
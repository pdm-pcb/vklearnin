#include "vklearnin/common.hpp"
#include "vklearnin/Instance.hpp"
#include "vklearnin/CommandStructures/CommandQueues.hpp"
#include "vklearnin/Swapchain.hpp"
#include "vklearnin/Pipeline.hpp"
#include "vklearnin/Buffers/Framebuffers.hpp"
#include "vklearnin/RenderLoop.hpp"
#include "vklearnin/Shaders/Buffers/BufferObject.hpp"
#include "vklearnin/Shaders/Buffers/UniformBufferObject.hpp"
#include "vklearnin/DescriptorSets/PerFrameDescriptors.hpp"
#include "vklearnin/DescriptorSets/PerMaterialDescriptors.hpp"
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
    Model gunship(
        "../../assets/meshes/spaceships/gunship.gltf",
        glm::vec3{ 3.0f, 2.0f, 0.0f},
        "../../assets/textures/spaceships/gunship_diffuse.png",
        command_queues,
        instance
    );

    gunship.populate_buffers(),
    gunship.init_texture_image_view();
    gunship.init_texture_sampler();

    Model carrier(
        "../../assets/meshes/spaceships/carrier.gltf",
        glm::vec3{ -2.0f, -2.0f, 0.0f },
        "../../assets/textures/spaceships/carrier_diffuse.png",
        command_queues,
        instance
    );

    carrier.populate_buffers(),
    carrier.init_texture_image_view();
    carrier.init_texture_sampler();

    Model floor = Model(Mesh::Primitive::XZPlane, glm::vec3(0.0f, -5.0f, 0.0f),
                        "../../assets/textures/Asphalt_001s.jpg",
                        command_queues, instance,
                        300.0f, 25.0f, 25.0f);

    floor.populate_buffers(),
    floor.init_texture_image_view();
    floor.init_texture_sampler();

    std::vector<Model *> models {
        &gunship,
        &carrier,
        &floor
    };

    std::vector<Texture2D const *> textures {
        gunship.texture(),
        carrier.texture(),
        floor.texture()
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
    UniformBufferObject ubo(sizeof(VPMatrices), FRAME_OVERLAP, instance);
    ubo.init_buffers("ubo");

    // =========================================================================
    // Descriptor Sets
    PerFrameDescriptors per_frame_descriptors(instance.logical_device());
    per_frame_descriptors.init_layout();
    per_frame_descriptors.init_pool();
    per_frame_descriptors.init_sets(ubo);

    PerMaterialDescriptors per_material_descriptors(
        static_cast<uint32_t>(textures.size()),
        instance.logical_device()
    );
    per_material_descriptors.init_layout();
    per_material_descriptors.init_pool();
    per_material_descriptors.init_sets(textures);

    // =========================================================================
    // shaderc's Compiler::Compiler() appears to have an 80 byte memory leak,
    // so no online compiling for now.
    Pipeline pipeline(instance);
    pipeline.vertex_from_binary("../../assets/shaders/shader.vert.spv");
    pipeline.fragment_from_binary("../../assets/shaders/shader.frag.spv");

    pipeline.init_render_passes(swapchain, MSAA_SAMPLES);
    pipeline.init_layout({
        per_frame_descriptors.layout(),
        per_material_descriptors.layout()
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
            ubo,
            pipeline,
            framebuffers,
            per_frame_descriptors,
            per_material_descriptors,
            models
        );

        // ...!
    }

    return 0;
}
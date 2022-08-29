#include "vklearnin/common.hpp"
#include "vklearnin/Instance.hpp"
#include "vklearnin/CommandStructures/CommandQueues.hpp"
#include "vklearnin/Swapchain.hpp"
#include "vklearnin/Pipeline.hpp"
#include "vklearnin/Buffers/Framebuffers.hpp"
#include "vklearnin/RenderLoop.hpp"
#include "vklearnin/Shaders/Buffers/BufferObject.hpp"
#include "vklearnin/Shaders/Buffers/UniformBufferObject.hpp"
#include "vklearnin/DescriptorSet.hpp"
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

    Instance instance;
    instance.init_instance();
    instance.init_instance_procs();
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
    instance.init_logical_device(command_queues.queues(),
                                 command_queues.queue_count());
    instance.init_logical_device_procs();

    // now that there's a logical device in place, go a head and initialize a
    // command pool, queue, and command buffer
    command_queues.init_pools();
    command_queues.init_queues();
    command_queues.init_buffers();


    // =========================================================================
    // Vertex data -------------------------------------------------------------
    Model viking_room("../../assets/meshes/viking_room.gltf");
    BufferObject<Vertex> vertex_buffer(viking_room.vertices(), instance);
    vertex_buffer.populate_buffer(command_queues.command_pool(),
                                  command_queues.graphics_queue());

    std::vector<::VkBuffer> vertex_buffers {
        vertex_buffer.handle()
    };

    BufferObject<Index> index_buffer(viking_room.indices(), instance);
    index_buffer.populate_buffer(command_queues.command_pool(),
                                 command_queues.graphics_queue());
    
/*
    std::vector<Vertex> vertices;
    std::vector<Index>  indices;

    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string error;

    bool model_loaded = tinyobj::LoadObj(
        &attrib,
        &shapes,
        &materials,
        &error,
        "../../assets/meshes/viking_room.obj"
    );

    if(!model_loaded)
    {
        CONSOLE_CRITICAL("\nTinyOBJ: {}", error);
    }
    else if(!error.empty()) {
        CONSOLE_WARN("\nTinyOBJ: {}", error);
    }

    for(const auto &shape : shapes) {
        for(const auto &index : shape.mesh.indices) {
            Vertex vert(
                glm::vec3(
                    attrib.vertices[3 * index.vertex_index + 0],
                    attrib.vertices[3 * index.vertex_index + 1],
                    attrib.vertices[3 * index.vertex_index + 2]
                ),
                glm::vec3(1.0f, 1.0f, 1.0f),
                glm::vec2(
                    attrib.texcoords[2 * index.texcoord_index + 0],
                    attrib.texcoords[2 * index.texcoord_index + 1]
                )
            );

            vertices.push_back(vert);
            indices.push_back(indices.size());
        }
    }

    CONSOLE_TRACE("Loaded mesh with {} vertices", vertices.size());
*/
/*
    const std::vector<Vertex> vertices {
        {{ -0.5f, -0.5f, 0.0f }, { 1.0f, 0.0f, 1.0f }, { 1.0f, 0.0f }},
        {{  0.5f, -0.5f, 0.0f }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f }},
        {{  0.5f,  0.5f, 0.0f }, { 0.0f, 1.0f, 0.0f }, { 0.0f, 1.0f }},
        {{ -0.5f,  0.5f, 0.0f }, { 1.0f, 0.0f, 0.0f }, { 1.0f, 1.0f }},

        {{ -0.5f, -0.5f, -1.0f }, { 1.0f, 0.0f, 1.0f }, { 1.0f, 0.0f }},
        {{  0.5f, -0.5f, -1.0f }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f }},
        {{  0.5f,  0.5f, -1.0f }, { 0.0f, 1.0f, 0.0f }, { 0.0f, 1.0f }},
        {{ -0.5f,  0.5f, -1.0f }, { 1.0f, 0.0f, 0.0f }, { 1.0f, 1.0f }},
    };

    // Index Buffer-------------------------------------------------------------
    const std::vector<Index> indices {
        0u, 1u, 2u,
        2u, 3u, 0u,

        4u, 5u, 6u,
        6u, 7u, 4u,
    };


    BufferObject<Vertex> vertex_buffer(vertices, instance);
    vertex_buffer.populate_buffer(command_queues.command_pool(),
                                  command_queues.graphics_queue());

    std::vector<::VkBuffer> vertex_buffers {
        vertex_buffer.handle()
    };

    BufferObject<Index> index_buffer(indices, instance);
    index_buffer.populate_buffer(command_queues.command_pool(),
                                 command_queues.graphics_queue());
//*/

    // =========================================================================
    Texture2D texture(command_queues.command_pool(),
                      command_queues.graphics_queue(),
                      instance);
    // texture.load_file("../../assets/textures/stone_wall01d.png");
    texture.load_file("../../assets/textures/viking_room.png");
    texture.init_image_view();
    texture.init_sampler(
        ::VK_FILTER_LINEAR,
        ::VK_FILTER_LINEAR,
        ::VK_SAMPLER_MIPMAP_MODE_LINEAR,
        ::VK_SAMPLER_ADDRESS_MODE_REPEAT,
        ::VK_SAMPLER_ADDRESS_MODE_REPEAT,
        true, instance.max_anisotropy()
    );
    
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
    UniformBufferObject ubo(sizeof(MVPMatrices), MAX_IMAGES, instance);
    ubo.init_buffers();

    // =========================================================================
    // Descriptor Sets
    DescriptorSet descriptor_set(MAX_IMAGES, instance.logical_device());
    descriptor_set.init_layout();
    descriptor_set.init_pool();
    descriptor_set.init_sets(ubo, texture);

    // =========================================================================
    // shaderc's Compiler::Compiler() appears to have an 80 byte memory leak,
    // so no online compiling for now.
    Pipeline pipeline(instance);
    pipeline.vertex_from_binary("../../assets/shaders/shader.vert.spv");
    pipeline.fragment_from_binary("../../assets/shaders/shader.frag.spv");

    pipeline.init_render_passes(swapchain);
    pipeline.init_layout(descriptor_set.layout());
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
            descriptor_set,
            framebuffers,
            index_buffer,
            vertex_buffers,
            { 0u }
        );

        // ...!
    }

    return 0;
}
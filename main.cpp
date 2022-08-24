#include "common.hpp"
#include "Instance.hpp"
#include "CommandQueues.hpp"
#include "Swapchain.hpp"
#include "Pipeline.hpp"
#include "Framebuffers.hpp"
#include "RenderLoop.hpp"

#if defined(__linux__)
    #include "X11Window.hpp"
    using Window = X11Window;
#elif defined(_WIN32)
    #include "Win32Window.hpp"
    using Window = Win32Window;
#endif

int main() {
    ConsoleLog::init();

    Instance instance;
    instance.init_instance();
    instance.init_instance_procs();
    instance.init_physical_device();

    Window window(instance.vulkan_instance());

    // both the window itself and the window's surface are required for
    // establishing a command queue family for the Vulkan instance
    window.init_window();
    window.init_surface();
    
    // track down the desired queue families
    CommandQueues command_queues(instance.physical_device(),
                                 instance.logical_device(),
                                 window.surface());
    command_queues.init_families(instance);
    command_queues.init_queue_info();

    // with the physical device set up and queue family chosen, the logical
    // devuce can be created
    instance.init_logical_device(command_queues);
    instance.init_logical_device_procs();

    // now that there's a logical device in place, go a head and initialize a
    // command pool, queue, and command buffer
    command_queues.init_pools();
    command_queues.init_queues();
    command_queues.init_buffers();
    
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

    Pipeline pipeline(instance.logical_device());
    // pipeline.vertex_from_source("../../shaders/shader.vert");
    // pipeline.fragment_from_source("../../shaders/shader.frag");

    // shaderc's Compiler::Compiler() appears to have an 80 byte memory leak,
    // so no online compiling for now.
    pipeline.vertex_from_binary("../../shaders/shader.vert.spv");
    pipeline.fragment_from_binary("../../shaders/shader.frag.spv");

    pipeline.init_render_passes(swapchain); // only one render pass for now
    pipeline.init_layout();            // the bare minimum for now
    pipeline.init_pipeline(swapchain); // set it all up with the right values

    Framebuffers framebuffers(instance.logical_device());
    framebuffers.init_buffers(swapchain, pipeline);

    RenderLoop render_loop(instance.logical_device(), window, command_queues);
    render_loop.init_synchronization();

    bool carry_on = true;
    while(carry_on) {
        carry_on = render_loop.run(
            instance,
            command_queues,
            swapchain,
            pipeline,
            framebuffers
        );

        // ...!
    }

    return 0;
}
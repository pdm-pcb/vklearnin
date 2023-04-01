// #include "vklearnin/vklearnin.hpp"
// #include "vklearnin/system/Engine.hpp"

// #include "vklearnin/system/Application.hpp"
// #include "vklearnin/rendering/swapchain/Swapchain.hpp"

// namespace vkl {

// // =============================================================================
// void Engine::render_loop() {
//     Timekeeper::frame_start();
//         Swapchain::next_image();
//         Swapchain::reset_fence();

//         _cmd_pools[Swapchain::image_index()].reset();
//         auto &cmd_buffer = _cmd_buffers[Swapchain::image_index()];

//         const vk::CommandBufferBeginInfo begin_info { };
//         cmd_buffer.native().begin(begin_info);

//             _application.submit_draws();
//             Renderer::render_pass(cmd_buffer.native());

//         cmd_buffer.native().end();

//         Swapchain::submit({ cmd_buffer.native() });
//         Swapchain::present();
//     Timekeeper::frame_end();
// }

// // =============================================================================
// void Engine::init() {
//     for(uint32_t frame = 0; frame < _cmd_pools.size(); ++frame) {
//         _cmd_pools[frame].create();
//         _cmd_buffers[frame].allocate(_cmd_pools[frame].native());
//     }
// }

// // =============================================================================
// void Engine::shutdown() {
//     for(uint32_t frame = 0; frame < _cmd_pools.size(); ++frame) {
//         _cmd_buffers[frame].free();
//         _cmd_pools[frame].destroy();
//     }
// }

// // =============================================================================
// Engine::Engine(Application &app) :
//     _application { app }
// {
//     _cmd_pools.resize(RenderConfig::concurrent_frames);
//     _cmd_buffers.resize(RenderConfig::concurrent_frames);
// }

// } // namespace vkl
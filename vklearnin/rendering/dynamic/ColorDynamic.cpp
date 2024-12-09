// #include "vklearnin/vklearnin.hpp"
// #include "vklearnin/rendering/dynamic/ColorDynamic.hpp"

// #include "vklearnin/vulkan/swapchain/vkSurface.hpp"
// #include "vklearnin/vulkan/devices/vkDevice.hpp"
// #include "vklearnin/vulkan/devices/vkCmdBuffer.hpp"

// namespace vkl {

// // =============================================================================
// bool ColorDynamic::create(vkSurface const &surface, vkDevice const &device) {
//     if(_render_pass.native()) {
//         Log::error(
//             "Color pass {} already exists.",
//             _render_pass.native()
//         );
//         return false;
//     }

//     if(!surface.native()) {
//         Log::error("Cannot create color pass with invalid surface.");
//         return false;
//     }

//     if(!device.native()) {
//         Log::error("Cannot create color pass with invalid device.");
//         return false;
//     }

//     _color_attachment = vk::RenderingAttachmentInfoKHR {
//         .pNext = nullptr,
//         .imageView = { },
//         .imageLayout = { },
//         .resolveMode = { },
//         .resolveImageView = { },
//         .resolveImageLayout = { },
//         .loadOp = { },
//         .storeOp = { },
//         .clearValue = { },
//     };

//     _render_area = vk::Rect2D {
//         .offset = { },
//         .extent = surface.extent()
//     };

//     _rendering_info = vk::RenderingInfoKHR {
//         .pNext = nullptr,
//         .flags = { },
//         .renderArea = { },
//         .layerCount = { },
//         .viewMask = { },
//         .colorAttachmentCount = { },
//         .pColorAttachments = { },
//         .pDepthAttachment = { },
//         .pStencilAttachment = { },
//     };

// auto render_area               = VkRect2D{VkOffset2D{}, VkExtent2D{width, height}};
// auto render_info               = vkb::initializers::rendering_info(render_area, 1, &color_attachment_info);
// render_info.layerCount         = 1;
// render_info.pDepthAttachment   = &depth_attachment_info;
// render_info.pStencilAttachment = &depth_attachment_info;

//     return true;
// }

// // =============================================================================
// bool ColorDynamic::destroy() {

//     return true;
// }

// // =============================================================================
// void ColorDynamic::update_render_area(vkSurface const &surface) {

// }

// // =============================================================================
// void ColorDynamic::begin(vkFrameBuffer const &frame_buffer,
//                          std::span<vk::ClearValue const> const clear_values,
//                          vkCmdBuffer const &cmd_buffer)
// {

// }

// } // namespace vkl
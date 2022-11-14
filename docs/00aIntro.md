## Reasoning
etc etc

## Tools
CMake, VSCode, etc

## Overview
- Create Vulkan instance
	- Initialize [Debug Report Callback](https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/vkCreateDebugReportCallbackEXT.html) object
- Select [Physical Device](https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VkPhysicalDevice.html)
	- Check for device [Queue Family Properties](https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VkQueueFamilyProperties.html) which support graphics operations and presentation
- Create a platform window and [Surface](https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VkSurfaceKHR.html)
- Create a [Logical Device](https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VkDevice.html) and associated [Device Command Queues](https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VkQueue.html)
- Build a [Swapchain](https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VK_KHR_swapchain.html)
	- Adjust logical device and device queues to account for double buffering
	- Query for surface [Capabilities](https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VkSurfaceCapabilitiesKHR.html) and [Format](https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VkSurfaceFormatKHR.html)
		- RGB linear vs sRGB non-linear
	- Address VSync and [Present Modes](https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VkPresentModeKHR.html)
	- Request swapchain [Images](https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VkImage.html) and create [Image Views](https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VkImageView.html)
		- Triple (or more) buffering
- Load a [Shader Module](https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VkShaderModule.html) from disk
	- Offline compilation, portability, optimizations, `glslang`, `glslc`
- Design a [Pipeline](https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VkPipeline.html)
	- Establish shader stages
	- Create empty [Pipeline Layout](https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VkPipelineLayout.html)
	- Specify default [Render Pass](https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VkRenderPass.html)
		- Single [Subpass](https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VkSubpassDescription.html) with single [Input Attachment](https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VkAttachmentDescription.html) for the color buffer
		- [Subpass Dependency](https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VkSubpassDependency.html) setup such that the subpass waits for an image to write
		- Set the number of [Viewports](https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VkViewport.html) and scissors
		- Match the viewport and scissor dimensions with the surface [Extent](https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VkExtent2D.html)
			- Using negative viewport height and "reverse" [Triangle Winding](https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VkFrontFace.html) to [flip the Vulkan viewport](https://www.saschawillems.de/blog/2019/03/29/flipping-the-vulkan-viewport/)
		- Establish the viewport and scissor as [Dynamic States](https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VkDynamicState.html)
		- Create an empty [Vertex Input State](https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VkPipelineVertexInputStateCreateInfo.html)
		- Set the [Input Assembly State](https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VkPrimitiveTopology.html) to triangle list
		- Disable alpha blending
		- Configure rasterizer to [Fill](https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VkPolygonMode.html) triangles and [Cull](https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VkCullModeFlagBits.html) back faces
- Create a Renderer, for containing:
	- [Synchronization](https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VkSemaphore.html) [Primitives](https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VkFence.html)
		- Semaphores are for ordering queue/swapchain operations (GPU side)
			- A pair of semaphores to indicate when a swapchain image is available for writing
			- A pair of semaphores to indicate when the back buffer has been drawn to completely
		- Fences are used to send signals from the GPU to the host regarding synchronization
			- A pair of fences to halt CPU submission of GPU commands when there's a frame drawn to both buffers already
	- and [Framebuffers](https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VkFramebuffer.html)
		- 
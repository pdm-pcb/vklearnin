## Swapchain
The [`vk::SwapchainKHR`](https://docs.vulkan.org/refpages/latest/refpages/source/VkSwapchainKHR.html) object represents Vulkan's ideas of a "chain" of images used for drawing and presentation. While one image is being displayed by our target window/surface, another image (often more than one image) is being worked on by the host and device. Once the new image is ready and the swapchain is ready to display it, the new image is drawn on screen and the old image is sent back to begin being drawn to.

In order to create a swapchain, we need a populated [`vk::SwapchainCreateInfoKHR`](https://docs.vulkan.org/refpages/latest/refpages/source/VkSwapchainCreateInfoKHR.html) structure. It'll need the surface we intend to display images via, as well as details about the number, format, size, and more of swapchain images it needs to provide. After creating the swapchain, we have to request the images it's created for us so we can interface with them later.


## Images and Views

Vulkan represents memory in a myriad of ways. The [`vk::Image`](https://docs.vulkan.org/refpages/latest/refpages/source/VkImage.html) handle represents memory which can be accessed in a multidimensional way. In the case of our swapchain images, they're two dimensional, just like your display. There is an additional tool required for accessing the memory represented by an image, though: the [`vk::ImageView`](https://docs.vulkan.org/refpages/latest/refpages/source/VkImageView.html). 

For the time being, we aren't strictly creating any images, as the swapchain has done that for us. We will have to create views for the image handles the swapchain provides, however.
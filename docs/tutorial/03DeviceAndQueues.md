## Logical Device
With all of the previous infrastructure established, we can finally create the [`vk::Device`](https://docs.vulkan.org/refpages/latest/refpages/source/VkDevice.html), which is how Vulkan wraps up all of these concepts into a single handle. The logical device will want to know what features and extensions you want enabled, which will be the same features and extensions we queried the physical device for.

## Device Queue
Since the device owns its queues, I've chosen to build `vkQueue` with a set/clear interface. The host can neither create or destroy a queue, only request it from the device. After we've got the handle, the queue will have two jobs: receiving command submissions and presenting images.

For unsynchronized submission, we can simply provide a [`vk::SubmitInfo`](https://docs.vulkan.org/refpages/latest/refpages/source/VkSubmitInfo.html) structure to `vkQueue::submit()`. An overload of this same function takes a submit info struct as well as a [`vk::Fence`](https://docs.vulkan.org/refpages/latest/refpages/source/VkFence.html) to coordinate the device queue's execution order relative to the host. We'll talk more on synchronization before too long.

Presentation also requires synchronization, but as far as `vkQueue` is concerned, the user just furnishes a [`vk::PresentInfoKHR`](https://docs.vulkan.org/refpages/latest/refpages/source/VkPresentInfoKHR.html) structure.
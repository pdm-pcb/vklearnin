---
issueTracker: "[[vklearnin/dashboard.md|Issues]]"
issueNo: 1
status: open
labels:
  - enhancement
---
```dataviewjs
dv.view("Issue Tracker/IssueTracker/Issue", { obsidian: obsidian });
```

- [ ] Remove request for `VK_KHR_dynamic_rendering` extension, but still check the physical device capabilities
- [ ] Convert to Sync2
	- [ ] `vk::ImageMemoryBarrier` -> `vk::ImageMemoryBarrier2`
	- [ ] `vk::CommandBuffer::pipelineBarrier()` -> `vk::CommandBuffer::pipelineBarrier2()`
	- [ ] `vk::Queue::submit()` -> `vk::Queue::submit2()`

https://www.khronos.org/blog/vulkan-sdk-offers-developers-a-smooth-transition-path-to-synchronization2

https://docs.vulkan.org/guide/latest/synchronization.html

https://www.lunarg.com/wp-content/uploads/2024/02/Guide-to-Vulkan-Synchronization-Validation-LunarG-John-Zulauf-02-01-2024.pdf

https://www.sctheblog.com/blog/vulkan-synchronization/

https://themaister.net/blog/2019/08/14/yet-another-blog-explaining-vulkan-synchronization/
---
issueTracker: "[[vklearnin/dashboard.md|Issues]]"
issueNo: 3
status: open
labels:
  - feature
---
```dataviewjs
dv.view("Issue Tracker/IssueTracker/Issue", { obsidian: obsidian });
```

**2025.03.30**
I finally got the implementation working generally. Matt came to the rescue via his own MSAA Dynamic Rendering code. What I was missing before was that, with dynamic rendering, you only need two attachments: color and depth. The color attachment "contains" both the multisample buffer and the resolve image (the swapchain image).

Specifically, when setting up the attachments initially, the multisample buffer is used as the color attachment.

```cpp
void MSAADynamic::_init_attachments(
    std::span<vk::ClearValue const> const clear_values)
{
    _color_attachments = {{ vk::RenderingAttachmentInfoKHR {
        .pNext = nullptr,
        .imageView = _multisample_view.native(),
        .imageLayout = { },
        .resolveMode = vk::ResolveModeFlagBitsKHR::eAverage,
        .resolveImageView = { },
        .resolveImageLayout = { },
        .loadOp = vk::AttachmentLoadOp::eClear,
        .storeOp = vk::AttachmentStoreOp::eStore,
        .clearValue = {
            .color = clear_values[0].color,
        },
    }}};

    _depth_attachment = vk::RenderingAttachmentInfoKHR {
        .pNext = nullptr,
        .imageView = _depth_view.native(),
        .imageLayout = _depth_buffer.layout(),
        .resolveMode = { },
        .resolveImageView = { },
        .resolveImageLayout = { },
        .loadOp = vk::AttachmentLoadOp::eClear,
        .storeOp = vk::AttachmentStoreOp::eStore,
        .clearValue = {
            .depthStencil = clear_values[1].depthStencil,
        },
    };
}
```

And when modifying the rendering info struct, the swapchain image gets slotted in just like depth and color dynamic rendering objects:

```cpp
vk::RenderingInfoKHR const &
MSAADynamic::rendering_info(vk::ImageView const &view,
                            vk::ImageLayout const &layout)
{
    _color_attachments[0].imageLayout = _multisample_buffer.layout();
    _color_attachments[0].resolveImageView = view;
    _color_attachments[0].resolveImageLayout = layout;

    _depth_attachment.imageLayout = _depth_buffer.layout();

    return _rendering_info;
}
```

The only gotcha I had after I sorted out which images go where was forgetting to update the image layout. The multisample buffer will have been transitioned from undefined to color attachment optimal before draw commands are recorded, so it needs to get updated every frame just like the swapchain stuff.

What I'm left with now, though, is unsuitable image transition code. Presumably the multisample buffer requires different sources and destinations than does the swapchain image when going from undefined to color attachment optimal. Perhaps I can pull the sources and destinations out of the image transition function all together and make them parameters instead of having a bunch of branching logic.
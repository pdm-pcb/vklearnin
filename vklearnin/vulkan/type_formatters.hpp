#ifndef VKLEARNIN_TOOLS_TYPE_FORMATTERS_HPP
#define VKLEARNIN_TOOLS_TYPE_FORMATTERS_HPP

#include <fmt/format.h>

template <> struct fmt::formatter<vk::Instance> {
    constexpr auto parse(fmt::format_parse_context &ctx) {
        return ctx.begin();
    }

    auto format(vk::Instance const &instance, fmt::format_context &ctx) const {
        return fmt::format_to(
            ctx.out(), "{:#x}",
            reinterpret_cast<uint64_t>(VkInstance(instance))
        );
    }
};

template <> struct fmt::formatter<vk::DebugUtilsMessengerEXT> {
    constexpr auto parse(fmt::format_parse_context &ctx) {
        return ctx.begin();
    }

    auto format(vk::DebugUtilsMessengerEXT const &debug,
                fmt::format_context &ctx) const
    {
        return fmt::format_to(
            ctx.out(), "{:#x}",
            reinterpret_cast<uint64_t>(VkDebugUtilsMessengerEXT(debug))
        );
    }
};

template <> struct fmt::formatter<vk::PhysicalDevice> {
    constexpr auto parse(fmt::format_parse_context &ctx) {
        return ctx.begin();
    }

    auto
    format(vk::PhysicalDevice const &device, fmt::format_context &ctx) const {
        return fmt::format_to(
            ctx.out(), "{:#x}",
            reinterpret_cast<uint64_t>(VkPhysicalDevice(device))
        );
    }
};

template <> struct fmt::formatter<vk::Device> {
    constexpr auto parse(fmt::format_parse_context &ctx) {
        return ctx.begin();
    }

    auto format(vk::Device const &device, fmt::format_context &ctx) const {
        return fmt::format_to(
            ctx.out(), "{:#x}",
            reinterpret_cast<uint64_t>(VkDevice(device))
        );
    }
};

template <> struct fmt::formatter<vk::DeviceMemory> {
    constexpr auto parse(fmt::format_parse_context &ctx) {
        return ctx.begin();
    }

    auto format(vk::DeviceMemory const &memory, fmt::format_context &ctx) const
    {
        return fmt::format_to(
            ctx.out(), "{:#x}",
            reinterpret_cast<uint64_t>(VkDeviceMemory(memory))
        );
    }
};

template <> struct fmt::formatter<vk::Queue> {
    constexpr auto parse(fmt::format_parse_context &ctx) {
        return ctx.begin();
    }

    auto format(vk::Queue const &queue, fmt::format_context &ctx) const {
        return fmt::format_to(
            ctx.out(), "{:#x}",
            reinterpret_cast<uint64_t>(VkQueue(queue))
        );
    }
};

template <> struct fmt::formatter<vk::CommandPool> {
    constexpr auto parse(fmt::format_parse_context &ctx) {
        return ctx.begin();
    }

    auto format(vk::CommandPool const &pool, fmt::format_context &ctx) const {
        return fmt::format_to(
            ctx.out(), "{:#x}",
            reinterpret_cast<uint64_t>(VkCommandPool(pool))
        );
    }
};

template <> struct fmt::formatter<vk::CommandBuffer> {
    constexpr auto parse(fmt::format_parse_context &ctx) {
        return ctx.begin();
    }

    auto format(vk::CommandBuffer const &cmd_buffer,
                fmt::format_context &ctx) const
    {
        return fmt::format_to(
            ctx.out(), "{:#x}",
            reinterpret_cast<uint64_t>(VkCommandBuffer(cmd_buffer))
        );
    }
};

template <> struct fmt::formatter<vk::SurfaceKHR> {
    constexpr auto parse(fmt::format_parse_context &ctx) {
        return ctx.begin();
    }

    auto format(vk::SurfaceKHR const &surface, fmt::format_context &ctx) const {
        return fmt::format_to(
            ctx.out(), "{:#x}",
            reinterpret_cast<uint64_t>(VkSurfaceKHR(surface))
        );
    }
};

template <> struct fmt::formatter<vk::SwapchainKHR> {
    constexpr auto parse(fmt::format_parse_context &ctx) {
        return ctx.begin();
    }

    auto format(vk::SwapchainKHR const &swapchain,
                fmt::format_context &ctx) const
    {
        return fmt::format_to(
            ctx.out(), "{:#x}",
            reinterpret_cast<uint64_t>(VkSwapchainKHR(swapchain))
        );
    }
};

template <> struct fmt::formatter<vk::Fence> {
    constexpr auto parse(fmt::format_parse_context &ctx) {
        return ctx.begin();
    }

    auto format(vk::Fence const &fence, fmt::format_context &ctx) const
    {
        return fmt::format_to(
            ctx.out(), "{:#x}",
            reinterpret_cast<uint64_t>(VkFence(fence))
        );
    }
};

template <> struct fmt::formatter<vk::Semaphore> {
    constexpr auto parse(fmt::format_parse_context &ctx) {
        return ctx.begin();
    }

    auto format(vk::Semaphore const &sem, fmt::format_context &ctx) const
    {
        return fmt::format_to(
            ctx.out(), "{:#x}",
            reinterpret_cast<uint64_t>(VkSemaphore(sem))
        );
    }
};

template <> struct fmt::formatter<vk::Buffer> {
    constexpr auto parse(fmt::format_parse_context &ctx) {
        return ctx.begin();
    }

    auto format(vk::Buffer const &buffer, fmt::format_context &ctx) const {
        return fmt::format_to(
            ctx.out(), "{:#x}",
            reinterpret_cast<uint64_t>(VkBuffer(buffer))
        );
    }
};

template <> struct fmt::formatter<vk::Image> {
    constexpr auto parse(fmt::format_parse_context &ctx) {
        return ctx.begin();
    }

    auto format(vk::Image const &image, fmt::format_context &ctx) const {
        return fmt::format_to(
            ctx.out(), "{:#x}",
            reinterpret_cast<uint64_t>(VkImage(image))
        );
    }
};

template <> struct fmt::formatter<vk::ImageView> {
    constexpr auto parse(fmt::format_parse_context &ctx) {
        return ctx.begin();
    }

    auto format(vk::ImageView const &view, fmt::format_context &ctx) const {
        return fmt::format_to(
            ctx.out(), "{:#x}",
            reinterpret_cast<uint64_t>(VkImageView(view))
        );
    }
};

template <> struct fmt::formatter<vk::Sampler> {
    constexpr auto parse(fmt::format_parse_context &ctx) {
        return ctx.begin();
    }

    auto format(vk::Sampler const &sampler, fmt::format_context &ctx) const {
        return fmt::format_to(
            ctx.out(), "{:#x}",
            reinterpret_cast<uint64_t>(VkSampler(sampler))
        );
    }
};

template <> struct fmt::formatter<vk::DescriptorPool> {
    constexpr auto parse(fmt::format_parse_context &ctx) {
        return ctx.begin();
    }

    auto format(vk::DescriptorPool const &pool, fmt::format_context &ctx) const
    {
        return fmt::format_to(
            ctx.out(), "{:#x}",
            reinterpret_cast<uint64_t>(VkDescriptorPool(pool))
        );
    }
};

template <> struct fmt::formatter<vk::DescriptorSetLayout> {
    constexpr auto parse(fmt::format_parse_context &ctx) {
        return ctx.begin();
    }

    auto format(vk::DescriptorSetLayout const &layout,
                fmt::format_context &ctx) const
    {
        return fmt::format_to(
            ctx.out(), "{:#x}",
            reinterpret_cast<uint64_t>(VkDescriptorSetLayout(layout))
        );
    }
};

template <> struct fmt::formatter<vk::DescriptorSet> {
    constexpr auto parse(fmt::format_parse_context &ctx) {
        return ctx.begin();
    }

    auto format(vk::DescriptorSet const &set, fmt::format_context &ctx) const {
        return fmt::format_to(
            ctx.out(), "{:#x}",
            reinterpret_cast<uint64_t>(VkDescriptorSet(set))
        );
    }
};

template <> struct fmt::formatter<vk::RenderPass> {
    constexpr auto parse(fmt::format_parse_context &ctx) {
        return ctx.begin();
    }

    auto format(vk::RenderPass const &render_pass, fmt::format_context &ctx)
    const {
        return fmt::format_to(
            ctx.out(), "{:#x}",
            reinterpret_cast<uint64_t>(VkRenderPass(render_pass))
        );
    }
};

template <> struct fmt::formatter<vk::Pipeline> {
    constexpr auto parse(fmt::format_parse_context &ctx) {
        return ctx.begin();
    }

    auto format(vk::Pipeline const &pipeline, fmt::format_context &ctx) const {
        return fmt::format_to(
            ctx.out(), "{:#x}",
            reinterpret_cast<uint64_t>(VkPipeline(pipeline))
        );
    }
};

template <> struct fmt::formatter<vk::PipelineLayout> {
    constexpr auto parse(fmt::format_parse_context &ctx) {
        return ctx.begin();
    }

    auto format(vk::PipelineLayout const &layout,
                fmt::format_context &ctx) const
    {
        return fmt::format_to(
            ctx.out(), "{:#x}",
            reinterpret_cast<uint64_t>(VkPipelineLayout(layout))
        );
    }
};

template <> struct fmt::formatter<vk::ShaderModule> {
    constexpr auto parse(fmt::format_parse_context &ctx) {
        return ctx.begin();
    }

    auto format(vk::ShaderModule const &shader, fmt::format_context &ctx) const
    {
        return fmt::format_to(
            ctx.out(), "{:#x}",
            reinterpret_cast<uint64_t>(VkShaderModule(shader))
        );
    }
};

template <> struct fmt::formatter<vk::Framebuffer> {
    constexpr auto parse(fmt::format_parse_context &ctx) {
        return ctx.begin();
    }

    auto format(vk::Framebuffer const &framebuffer,
                fmt::format_context &ctx) const
    {
        return fmt::format_to(
            ctx.out(), "{:#x}",
            reinterpret_cast<uint64_t>(VkFramebuffer(framebuffer))
        );
    }
};

#endif // VKLEARNIN_TOOLS_TYPE_FORMATTERS_HPP
#ifndef VKLEARNIN_COMMANDSTRUCTURES_SINGLEUSECOMMANDBUFFER_HPP
#define VKLEARNIN_COMMANDSTRUCTURES_SINGLEUSECOMMANDBUFFER_HPP

#include <vulkan/vulkan.hpp>

class Instance;

class SingleUseCommandBuffer {
public:
    vk::CommandBuffer & init();
    void begin();
    void end();
    void submit(const vk::Queue &queue);

    SingleUseCommandBuffer(const vk::CommandPool &pool,
                           const Instance &instance);

    SingleUseCommandBuffer() = delete;

private:
    const vk::CommandPool &_pool;
    vk::CommandBuffer _buffer;

    const Instance &_instance;
};

#endif // VKLEARNIN_COMMANDSTRUCTURES_SINGLEUSECOMMANDBUFFER_HPP
#ifndef VKLEARNIN_COMMANDSTRUCTURES_SINGLEUSECOMMANDBUFFER_HPP
#define VKLEARNIN_COMMANDSTRUCTURES_SINGLEUSECOMMANDBUFFER_HPP

#include <vulkan/vulkan.h>

class Instance;

class SingleUseCommandBuffer {
public:
    ::VkCommandBuffer & init();
    void begin();
    void end();
    void submit(const ::VkQueue &queue);

    SingleUseCommandBuffer(const ::VkCommandPool &pool,
                           const Instance &instance);

private:
    const ::VkCommandPool &_pool;
    ::VkCommandBuffer _buffer;

    const Instance &_instance;
};

#endif // VKLEARNIN_COMMANDSTRUCTURES_SINGLEUSECOMMANDBUFFER_HPP
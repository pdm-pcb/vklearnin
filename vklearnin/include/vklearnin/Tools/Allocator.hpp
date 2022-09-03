#ifndef VKLEARNIN_TOOLS_ALLOCATOR_HPP
#define VKLEARNIN_TOOLS_ALLOCATOR_HPP

#include <vulkan/vulkan.hpp>

class Instance;

struct VmaAllocator_T;
VK_DEFINE_HANDLE(VmaAllocator);
struct VmaAllocation_T;
VK_DEFINE_HANDLE(VmaAllocation);

class Allocator {
public:
    static void init(const Instance &instance);
    static ::VmaAllocator & allocator();
    static void shutdown();

private:
    static ::VmaAllocator _allocator;
};

#endif // VKLEARNIN_TOOLS_ALLOCATOR_HPP
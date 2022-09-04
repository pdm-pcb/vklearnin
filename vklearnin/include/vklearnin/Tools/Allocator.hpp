#ifndef VKLEARNIN_TOOLS_ALLOCATOR_HPP
#define VKLEARNIN_TOOLS_ALLOCATOR_HPP

#include "vklearnin/pch.hpp"

class Instance;

class Allocator {
public:
    static void init(const Instance &instance);
    static ::VmaAllocator & allocator();
    static void snapshot();
    static void shutdown();

private:
    static ::VmaAllocator _allocator;
};

#endif // VKLEARNIN_TOOLS_ALLOCATOR_HPP
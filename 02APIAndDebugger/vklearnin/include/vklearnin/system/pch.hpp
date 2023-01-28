#ifndef VKLEARNIN_SYSTEM_PCH_HPP
#define VKLEARNIN_SYSTEM_PCH_HPP

// Designated initializers
// https://github.com/KhronosGroup/Vulkan-Hpp#designated-initializers
#define VULKAN_HPP_NO_CONSTRUCTORS

#define VULKAN_HPP_NO_EXCEPTIONS    // I perfer to check the return values
#define VULKAN_HPP_ASSERT_ON_RESULT // Assert on result can trip things up when
                                    // exceptions are disabled

// Prevent needing to load every function by hand
#define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC 1

#include <vulkan/vulkan.hpp>

#endif // VKLEARNIN_SYSTEM_PCH_HPP
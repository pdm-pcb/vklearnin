#ifndef VKLEARNAIN_TOOLS_SHADERTOOLS_HPP
#define VKLEARNAIN_TOOLS_SHADERTOOLS_HPP

#include "vklearnin/system/pch.hpp"

namespace vkl {
namespace ShaderTools {

    vk::ShaderModule
    module_from_binary(std::string_view filepath, const vk::Device &device);

} // namespace ShaderTools
} // namespace vkl

#endif // #define VKLEARNAIN_TOOLS_SHADERTOOLS_HPP

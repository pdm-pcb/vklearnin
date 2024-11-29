#ifndef VKLEARNIN_VULKAN_PIPELINES_SHADERPROGRAM_HPP
#define VKLEARNIN_VULKAN_PIPELINES_SHADERPROGRAM_HPP

#include "vklearnin/pch.hpp"

#include "vklearnin/vulkan/pipelines/vkShaderModule.hpp"

namespace vkl {

class vkDevice;

class ShaderProgram final {
public:
    ShaderProgram() = default;
    ~ShaderProgram() = default;

    ShaderProgram(ShaderProgram &&) = delete;
    ShaderProgram(ShaderProgram const &) = delete;

    ShaderProgram& operator=(ShaderProgram &&) = delete;
    ShaderProgram& operator=(ShaderProgram const &) = delete;

    ShaderProgram & add_stage(std::string_view const file_path);

private:
    vk::Device _device { nullptr };
    std::vector<vkShaderModule> _shader_modules;
};

} // namespace vkl

#endif // VKLEARNIN_VULKAN_PIPELINES_SHADERPROGRAM_HPP
#ifndef VKL_SHADER_HPP
#define VKL_SHADER_HPP

// #include <shaderc/shaderc.hpp>
#include <vulkan/vulkan.h>

#include <string>

// =============================================================================
class Shader {
public:
    static ::VkShaderModule
    module_from_source(const char *filepath,
                       const ::shaderc_shader_kind shader_kind,
                       const ::VkDevice &logical_device,
                       const bool optimize);

    static ::VkShaderModule
    module_from_binary(const char *filepath,
                       const ::VkDevice &logical_device);
private:
    static char * _read_source(const char *filepath);
    static std::vector<char> _read_binary(const char *filepath);

    static std::string _preprocess(const char *filepath,
                                   const ::shaderc_shader_kind shader_kind,
                                   const char *source);

    static std::vector<uint32_t> _compile(const std::string &source,
                                          const ::shaderc_shader_kind shader_kind,
                                          const char *filepath);

    // static shaderc::Compiler       _compiler;
    // static shaderc::CompileOptions _options;
};

#endif // VKL_SHADER_HPP
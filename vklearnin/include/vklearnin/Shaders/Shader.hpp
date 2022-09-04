#ifndef VKLEARNIN_SHADER_HPP
#define VKLEARNIN_SHADER_HPP

#include "vklearnin/pch.hpp"

// =============================================================================
class Shader {
public:
    // static vk::ShaderModule
    // module_from_source(const char *filepath,
    //                    const ::shaderc_shader_kind shader_kind,
    //                    const vk::Device &logical_device,
    //                    const bool optimize);

    static vk::ShaderModule
    module_from_binary(const char *filepath, const vk::Device &device);

private:
    // static char * _read_source(const char *filepath);
    static std::vector<char> _read_binary(const char *filepath);

    // static std::string _preprocess(const char *filepath,
    //                                const ::shaderc_shader_kind shader_kind,
    //                                const char *source);

    // static std::vector<uint32_t> _compile(const std::string &source,
    //                                       const ::shaderc_shader_kind shader_kind,
    //                                       const char *filepath);

    // static shaderc::Compiler       _compiler;
    // static shaderc::CompileOptions _options;
};

#endif // VKLEARNIN_SHADER_HPP
#include "vklearnin/common.hpp"
#include "vklearnin/Shaders/Shader.hpp"

#include <fstream>

// =============================================================================
vk::ShaderModule
Shader::module_from_binary(const char *filepath, const vk::Device &device)
{
    CONSOLE_INFO("");

    if(filepath == nullptr) {
        CONSOLE_ERROR("No file path provided for shader binary");
    }

    std::vector<char> binary = _read_binary(filepath);

    // the only tricky thing here is remembering to use the sizeof(uint32_t) as
    // a multiplier. I guess it shouldn't be that tricky; it's just like GL
    vk::ShaderModuleCreateInfo module_info {
        .codeSize = binary.size(),
        .pCode = reinterpret_cast<uint32_t *>(binary.data()),
    };

    auto module = device.createShaderModule(module_info);

    return module;
}

// =============================================================================
std::vector<char> Shader::_read_binary(const char *filepath) {
    CONSOLE_INFO("");

    std::ifstream input_file(filepath, std::ios::ate | std::ios::binary);

    if(!input_file.good()) {
        CONSOLE_ERROR("Unable to open binary '{}'", filepath);
        return std::vector<char>();
    }

    auto filesize = static_cast<size_t>(input_file.tellg());
    input_file.seekg(0, input_file.beg);

    std::vector<char> dest(filesize);
    input_file.read(dest.data(), static_cast<std::streamsize>(filesize));
    input_file.close();

    CONSOLE_TRACE("Loaded shader binary {}", filepath);

    return dest;
}

/*
using namespace shaderc;

Compiler       Shader::_compiler;
CompileOptions Shader::_options;

// =============================================================================
vk::ShaderModule
Shader::module_from_source(const char *filepath,
                           const ::shaderc_shader_kind shader_kind,
                           const vk::Device &logical_device,
                           const bool optimize)
{
    CONSOLE_INFO("");

    if(filepath == nullptr) {
        CONSOLE_ERROR("No file path provided for shader source");
    }

    // setting the various compiler options such that we'll wind up with
    // compatible binaries
    _options.SetForcedVersionProfile(460, ::shaderc_profile_core);
    _options.SetTargetEnvironment(
        ::shaderc_target_env_vulkan,
        ::shaderc_env_version_vulkan_1_3
    );
    _options.SetTargetSpirv(::shaderc_spirv_version_1_6);
    _options.SetWarningsAsErrors();

#ifdef DEBUG
    _options.SetGenerateDebugInfo();
    CONSOLE_TRACE("Debug Info enabled for '{}'", filepath);
#endif // DEBUG

    // default to no optimization, but make it go fast if requested
    if(optimize) {
        _options.SetOptimizationLevel(::shaderc_optimization_level_performance);
        CONSOLE_TRACE("performance optimization enabled for '{}'", filepath);
    }
    else {
        _options.SetOptimizationLevel(::shaderc_optimization_level_zero);
        CONSOLE_TRACE("no optimization enabled for '{}'", filepath);
    }

    // run the preprocessor on the raw character string, then delete the
    // allocated char[]
    char *source = _read_source(filepath);
    std::string preprocessed_source =
        _preprocess(source, shader_kind, filepath);
    delete[] source;

    // compile away
    std::vector<uint32_t> binary = _compile(
        preprocessed_source,
        shader_kind,
        filepath
    );

    // the only tricky thing here is remembering to use the sizeof(uint32_t) as
    // a multiplier. I guess it shouldn't be that tricky; it's just like GL
    vk::ShaderModuleCreateInfo module_info { };
    module_info.sType    = ::VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    module_info.codeSize = binary.size() * sizeof(uint32_t);
    module_info.pCode    = binary.data();

    vk::ShaderModule module;
    auto result = ::vkCreateShaderModule(
        logical_device,
        &module_info,
        nullptr,
        &module
    );

    if(result != VK_SUCCESS) {
        CONSOLE_ERROR("Failed to create shader module for '{}'",
                      filepath);
    }

    return module;
}

// =============================================================================
char * Shader::_read_source(const char *filepath) {
    CONSOLE_INFO("");

    // I haven't the slightest if starting at the end and then seeking backward
    // is somehow faster than just seekg()ing to the end, then tellg(), then
    // seekg() to the beinning
    std::ifstream input_file(filepath, std::ios::ate);

    if(!input_file.good()) {
        CONSOLE_ERROR("Unable to open source '{}'", filepath);
        return nullptr;
    }

    auto filesize = static_cast<size_t>(input_file.tellg());
    input_file.seekg(0, input_file.beg);

    // I suppose this could just as easily be done with a vector, and then
    // there'd be no need for the new and delete pair
    char *dest = new char[filesize + 1] { };
    memset(dest, '\0', filesize + 1);

    input_file.read(dest, static_cast<std::streamsize>(filesize));
    input_file.close();

    CONSOLE_TRACE("Loaded shader source {}", filepath);

    return dest;
}

// =============================================================================
std::string Shader::_preprocess(const char *source,
                                const ::shaderc_shader_kind shader_kind,
                                const char *filepath)
{
    CONSOLE_INFO("");

    // I do wonder what the filepath is used for as parameters to the
    // preprocessor and the compiler
    auto result =
        _compiler.PreprocessGlsl(
            source,
            shader_kind,
            filepath,
            _options
        );

    if(result.GetCompilationStatus() != ::shaderc_compilation_status_success) {
        CONSOLE_ERROR(
            "Preprocessing shader '{}' failed with error:\n\t'{}'",
            filepath,
            result.GetErrorMessage()
        );
        return "";
    }
    else {
        return { result.cbegin(), result.cend() };
    }
}

// =============================================================================
std::vector<uint32_t> Shader::_compile(const std::string &source,
                                       const ::shaderc_shader_kind shader_kind,
                                       const char *filepath)
{
    CONSOLE_INFO("");

    SpvCompilationResult result =
        _compiler.CompileGlslToSpv(
            source,
            shader_kind,
            filepath,   // same as above. Why is this needed?
            _options
        );

    if(result.GetCompilationStatus() != shaderc_compilation_status_success) {
        CONSOLE_ERROR(
            "Compiling shader '{}' failed with error:\n\t'{}'",
            filepath,
            result.GetErrorMessage()
        );
        return std::vector<uint32_t>();
    }

    return { result.cbegin(), result.cend() };
}
*/
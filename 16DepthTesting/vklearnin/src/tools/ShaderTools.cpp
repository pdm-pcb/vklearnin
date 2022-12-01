#include "vklearnin/vklearnin.hpp"
#include "vklearnin/tools/ShaderTools.hpp"

namespace vkl {
namespace ShaderTools {

using Binary = std::vector<uint32_t>;
using String = std::vector<char>;

Binary _spirv_to_binary(const char *filepath);

// =============================================================================
vk::ShaderModule
module_from_binary(const char *filepath, const vk::Device &device) {
    if(filepath == nullptr) {
        CONSOLE_CRITICAL("No file path provided for shader binary");
        return { };
    }

    auto binary = _spirv_to_binary(filepath);

    vk::ShaderModuleCreateInfo module_info {
        .codeSize = binary.size() * sizeof(uint32_t),
        .pCode = binary.data(),
    };

    auto [result, module] = device.createShaderModule(module_info);
    if(result != vk::Result::eSuccess) {
        CONSOLE_CRITICAL("Unable to create shader module.");
    }

    return module;
}

// =============================================================================
Binary _spirv_to_binary(const char *filepath) {
    std::ifstream input_file(filepath, std::ios::ate | std::ios::binary);

    if(!input_file.good()) {
        CONSOLE_CRITICAL("Unable to open binary '{}'", filepath);
        return Binary();
    }

    auto filesize = static_cast<size_t>(input_file.tellg());
    input_file.seekg(0, input_file.beg);

    String shader_string(filesize);
    input_file.read(
        shader_string.data(),
        static_cast<std::streamsize>(filesize)
    );
    input_file.close();

    Binary shader_binary(shader_string.size() / sizeof(uint32_t));
    memcpy(shader_binary.data(), shader_string.data(), shader_string.size());

    CONSOLE_TRACE("Loaded shader binary from '{}'", filepath);

    return shader_binary;
}

} // namespace ShaderTools
} // namespace vkl
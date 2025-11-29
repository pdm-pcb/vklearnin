#include "vklearnin/vklearnin.hpp"
#include "vklearnin/vulkan/pipelines/vkShaderModule.hpp"

#include "vklearnin/vulkan/devices/vkDevice.hpp"

namespace vkl {

// =============================================================================
bool vkShaderModule::create(std::string_view const filepath,
                            vkDevice const &device)
{
    if(_handle) {
        Log::error("Shader module {} already exists.", _handle);
        return false;
    }

    if(!device.native()) {
        Log::error("Cannot create shader module with invalid device.");
        return false;
    }

    _device = device.native();

    std::filesystem::path shader_path = VKL_ASSET_PATH / filepath.data();
    shader_path += VKL_SHADER_EXT;

    auto const shader_string = _spirv_to_string(shader_path);

    _reflect_shader(shader_string);

    auto const shader_binary = _string_to_binary(shader_string);

    const vk::ShaderModuleCreateInfo module_info {
        .codeSize = shader_binary.size() * sizeof(uint32_t),
        .pCode = shader_binary.data(),
    };

    _handle = _device.createShaderModule(module_info);
    Log::trace("Created shader module {} from '{}'",
               _handle,
               shader_path.string());

    return true;
}

// =============================================================================
bool vkShaderModule::destroy() {
    if(!_handle) {
        Log::error("Must create shader module before calling destroy.");
        return false;
    }

    Log::trace("Destroying shader module {}", _handle);
    _device.destroy(_handle);
    _handle = nullptr;
    _device = nullptr;
    _stage = { };
    _entry_point = { };
    return true;
}

// =============================================================================
vkShaderModule::StringData
vkShaderModule::_spirv_to_string(std::filesystem::path const &filepath) {
    // Open the SPIR-V binary file and place the "cursor" at the end
    std::ifstream input_file(filepath.native(),
                             std::ios::binary | std::ios::ate);

    if(!input_file.good()) {
        Log::critical("Unable to open SPIRV '{}'", filepath.string());
        return StringData { };
    }

    // Since we're already at the end, std::ifstream::tellg() will give us the
    // file's size. Capture that, then seek back to the beginning in order to
    // begin reading the file's content properly.
    auto filesize = static_cast<size_t>(input_file.tellg());
    input_file.seekg(0, std::ifstream::beg);

    // Start by reading in an array of characters
    StringData shader_string(filesize);
    input_file.read(shader_string.data(),
                    static_cast<std::streamsize>(filesize));
    input_file.close();

    return shader_string;
}

// =============================================================================
vkShaderModule::BinaryData
vkShaderModule::_string_to_binary(StringData const &shader_string) {
    // Copy the character array into an integer array to provide Vulkan with
    // the binary data in the size and configuration it expects.
    BinaryData shader_binary(shader_string.size() / sizeof(uint32_t));
    ::memcpy(shader_binary.data(), shader_string.data(), shader_string.size());

    return shader_binary;
}

// =============================================================================
void vkShaderModule::_reflect_shader(StringData const &shader_string) {
	::SpvReflectShaderModule module { };
	auto result = ::spvReflectCreateShaderModule(
        shader_string.size(),
        shader_string.data(),
        &module
    );

    if(result != ::SPV_REFLECT_RESULT_SUCCESS) {
        Log::error("SPIRV-Reflect failed to create module: '{}'",
                   static_cast<uint32_t>(result) );
        return;
    }

    // Gather the reflected details
    _get_stage(module);
    _get_entry_point(module);

    if(_stage == vk::ShaderStageFlagBits::eVertex) {
        _get_vertex_inputs(module);
    }

    _get_descriptors(module);

	::spvReflectDestroyShaderModule(&module);
}

// =============================================================================
void vkShaderModule::_get_stage(::SpvReflectShaderModule const &module) {
    switch(module.shader_stage) {
        case ::SPV_REFLECT_SHADER_STAGE_VERTEX_BIT:
            _stage = vk::ShaderStageFlagBits::eVertex;
            break;
        case ::SPV_REFLECT_SHADER_STAGE_TESSELLATION_CONTROL_BIT:
            _stage = vk::ShaderStageFlagBits::eTessellationControl;
            break;
        case ::SPV_REFLECT_SHADER_STAGE_TESSELLATION_EVALUATION_BIT:
            _stage = vk::ShaderStageFlagBits::eTessellationEvaluation;
            break;
        case ::SPV_REFLECT_SHADER_STAGE_GEOMETRY_BIT:
            _stage = vk::ShaderStageFlagBits::eGeometry;
            break;
        case ::SPV_REFLECT_SHADER_STAGE_FRAGMENT_BIT:
            _stage = vk::ShaderStageFlagBits::eFragment;
            break;
        case ::SPV_REFLECT_SHADER_STAGE_COMPUTE_BIT:
            _stage = vk::ShaderStageFlagBits::eCompute;
            break;
        default:
            Log::error("Unsupported SPIRV-reflect shader stage: {:#x}",
                       static_cast<uint32_t>(module.shader_stage));
            return;
    }

    Log::trace("Stage: {}", vk::to_string(_stage));
}

// =============================================================================
void vkShaderModule::_get_entry_point(::SpvReflectShaderModule const &module) {
    if(module.entry_point_count != 1) {
        Log::error("Shader module has {} entry points; must only have one.",
                   module.entry_point_count);
        return;
    }

    _entry_point = std::string(module.entry_points[0].name);

    Log::trace("Entry point: '{:s}'", _entry_point);
}

// =============================================================================
void vkShaderModule::_get_vertex_inputs(::SpvReflectShaderModule const &module)
{
    // Demonstrates how to generate all necessary data structures to populate
    // a VkPipelineVertexInputStateCreateInfo structure, given the module's
    // expected input variables.
    //
    // Simplifying assumptions:
    // - All vertex input attributes are sourced from a single vertex buffer,
    //   bound to VB slot 0.
    // - Each vertex's attribute are laid out in ascending order by location.
    // - The format of each attribute matches its usage in the shader;
    //   float4 -> VK_FORMAT_R32G32B32A32_FLOAT, etc. No attribute compression
    //   is applied.
    // - All attributes are provided per-vertex, not per-instance.

    uint32_t count = 0u;

    auto result = ::spvReflectEnumerateInputVariables(
        &module,
        &count,
        nullptr
    );

    if(result != ::SPV_REFLECT_RESULT_SUCCESS) {
        Log::error("SPIRV-Reflect failed to get count of input variables: '{}'",
                   static_cast<uint32_t>(result));
        return;
    }

    std::vector<::SpvReflectInterfaceVariable *> input_vars;
    input_vars.resize(count);

    result = ::spvReflectEnumerateInputVariables(
        &module,
        &count,
        input_vars.data()
    );

    if(result != ::SPV_REFLECT_RESULT_SUCCESS) {
        Log::error("SPIRV-Reflect failed to enumerate input variables: '{}' ",
                   static_cast<uint32_t>(result));
        return;
    }

    _vert_input = vk::VertexInputBindingDescription {
        .binding   = 0u,
        .stride    = 0u, // calculated below
        .inputRate = vk::VertexInputRate::eVertex,
    };

    _vert_attribs.reserve(input_vars.size());

    for(size_t i_var = 0; i_var < input_vars.size(); ++i_var) {
        auto const &refl_var = *(input_vars[i_var]);

        // ignore built-in variables
        if(refl_var.decoration_flags & ::SPV_REFLECT_DECORATION_BUILT_IN) {
            continue;
        }

        _vert_attribs.emplace_back(
            vk::VertexInputAttributeDescription {
                .location = refl_var.location,
                .binding  = _vert_input.binding,
                .format   = vk::Format(refl_var.format),
                .offset   = 0u,
            }
        );
    }

    // Sort attributes by location
    std::sort(
        std::begin(_vert_attribs),
        std::end(_vert_attribs),
        [](const vk::VertexInputAttributeDescription& a,
           const vk::VertexInputAttributeDescription& b) {
            return a.location < b.location;
        }
    );

    // Compute final offsets of each attribute, and total vertex stride.
    for (auto& attrib : _vert_attribs) {
        attrib.offset = _vert_input.stride;
        _vert_input.stride += _get_format_size(attrib.format);
    }

    _vert_input_info = vk::PipelineVertexInputStateCreateInfo {
        .pNext = nullptr,
        .flags = { },
        .vertexBindingDescriptionCount = 1u,
        .pVertexBindingDescriptions = &_vert_input,
        .vertexAttributeDescriptionCount =
            static_cast<uint32_t>(_vert_attribs.size()),
        .pVertexAttributeDescriptions = _vert_attribs.data(),
    };

    // Nothing further is done with attribute_descriptions or
    // binding_description in this sample. A real application would probably
    // derive this information from its mesh format(s); a similar mechanism
    // could be used to ensure mesh/shader compatibility.
}

// =============================================================================
uint32_t vkShaderModule::_get_format_size(vk::Format const format) {
    switch(format) {
        case vk::Format::eR32Uint:
        case vk::Format::eR32Sint:
        case vk::Format::eR32Sfloat:
            return 4u;

        case vk::Format::eR32G32Uint:
        case vk::Format::eR32G32Sint:
        case vk::Format::eR32G32Sfloat:
            return 8u;

        case vk::Format::eR32G32B32Uint:
        case vk::Format::eR32G32B32Sint:
        case vk::Format::eR32G32B32Sfloat:
            return 12u;

        case vk::Format::eR32G32B32A32Uint:
        case vk::Format::eR32G32B32A32Sint:
        case vk::Format::eR32G32B32A32Sfloat:
            return 16u;

        case vk::Format::eUndefined:
        default:
            Log::error("Unsupported vertex format ");
            return std::numeric_limits<uint32_t>::max();
    }
}

// =============================================================================
void vkShaderModule::_get_descriptors(::SpvReflectShaderModule const &module) {
    // Demonstrates how to generate all necessary data structures to create a
    // VkDescriptorSetLayout for each descriptor set in this shader.

    uint32_t count = 0u;

    auto result = ::spvReflectEnumerateDescriptorSets(
        &module,
        &count,
        nullptr
    );

    if(result != ::SPV_REFLECT_RESULT_SUCCESS) {
        Log::error("SPIRV-Reflect failed to get count of descriptor sets: '{}'",
                   static_cast<uint32_t>(result));
        return;
    }

    std::vector<::SpvReflectDescriptorSet *> sets;
    sets.resize(count);

    result = ::spvReflectEnumerateDescriptorSets(
        &module,
        &count,
        sets.data()
    );

    if(result != ::SPV_REFLECT_RESULT_SUCCESS) {
        Log::error("SPIRV-Reflect failed to enumerate descriptor sets: '{}' ",
                   static_cast<uint32_t>(result));
        return;
    }

    _set_layout_data.resize(sets.size());

    for(size_t i_set = 0u; i_set < sets.size(); ++i_set) {
        auto const &refl_set = *(sets[i_set]);

        auto &layout = _set_layout_data[i_set];
        layout.set_number = refl_set.set;
        layout.bindings.resize(refl_set.binding_count);

        for(uint32_t i_binding = 0u; i_binding < refl_set.binding_count; ++i_binding) {
            auto const &refl_binding = *(refl_set.bindings[i_binding]);

            auto &layout_binding = layout.bindings[i_binding];
            layout_binding = vk::DescriptorSetLayoutBinding {
                .binding = refl_binding.binding,
                .descriptorType = vk::DescriptorType(refl_binding.descriptor_type),
                .descriptorCount = 1u, // may increase below
                .stageFlags = vk::ShaderStageFlagBits(module.shader_stage),
            };

            for(uint32_t i_dim = 0u; i_dim < refl_binding.array.dims_count; ++i_dim) {
                layout_binding.descriptorCount *= refl_binding.array.dims[i_dim];
            }
        }

        layout.create_info.bindingCount = refl_set.binding_count;
        layout.create_info.pBindings = layout.bindings.data();
    }
    // Nothing further is done with set_layouts in this sample; in a real
    // application they would be merged with similar structures from other shader
    // stages and/or pipelines to create a VkPipelineLayout.
}

} // namespace vkl
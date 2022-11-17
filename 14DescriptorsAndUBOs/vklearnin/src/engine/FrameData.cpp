#include "vklearnin/vklearnin.hpp"
#include "vklearnin/engine/FrameData.hpp"

#include "vklearnin/rendering/devices/LogicalDevice.hpp"
#include "vklearnin/shaders/InstanceData.hpp"

namespace vkl {

// =============================================================================
void FrameData::update_ubo(const glm::mat4 &model_matrix) {
    auto result = LogicalDevice::native().mapMemory(_ubo.memory, 0u, _ubo.size);
    if(result.result != vk::Result::eSuccess) {
        CONSOLE_CRITICAL("Unable to map device memory");
    }

    void *map = result.value;
        memcpy(map, &model_matrix, _ubo.size);
    LogicalDevice::native().unmapMemory(_ubo.memory);
}

// =============================================================================
void FrameData::create() {
    _cmd_pool.create();
    _cmd_buffer.create(_cmd_pool);
    _init_descriptors();
}

// =============================================================================
void FrameData::destroy() {
    _cmd_buffer.destroy();
    _cmd_pool.destroy();

    BufferTools::destroy_buffer(_ubo);

    LogicalDevice::native().destroyDescriptorPool(_dsc_pool);
    for(auto &layout : _set_layouts) {
        LogicalDevice::native().destroyDescriptorSetLayout(layout);
    }
}

// =============================================================================
void FrameData::_init_descriptors() {
    _set_layouts.resize(1);
    _sets.resize(1);

    //--------------------------------------------------------------------------
    // The layout
    vk::DescriptorSetLayoutBinding descriptor_bindings[] {{
        .binding = 0u,
        .descriptorType = vk::DescriptorType::eUniformBuffer,
        .descriptorCount = 1u,
        .stageFlags = vk::ShaderStageFlagBits::eVertex,
        .pImmutableSamplers = nullptr
    }};

    vk::DescriptorSetLayoutCreateInfo descriptor_info {
        .bindingCount = static_cast<uint32_t>(std::size(descriptor_bindings)),
        .pBindings = descriptor_bindings,
    };

    auto desc_set_result = LogicalDevice::native().createDescriptorSetLayout(
        &descriptor_info,
        nullptr,
        &_set_layouts[0]
    );
    if(desc_set_result != vk::Result::eSuccess) {
        CONSOLE_CRITICAL("Unable to create descriptor set layout");
    }

    //--------------------------------------------------------------------------
    // The pool
    vk::DescriptorPoolSize pool_sizes[] {{
        vk::DescriptorType::eUniformBuffer, 10u
    }};

    vk::DescriptorPoolCreateInfo pool_info {
        .maxSets = 10u,
        .poolSizeCount = static_cast<uint32_t>(std::size(pool_sizes)),
        .pPoolSizes = pool_sizes
    };

    auto result = LogicalDevice::native().createDescriptorPool(
        &pool_info,
        nullptr,
        &_dsc_pool
    );
    if(result != vk::Result::eSuccess) {
        CONSOLE_CRITICAL("Failed to create descriptor pool");
    }

    //--------------------------------------------------------------------------
    // The allocation
    vk::DescriptorSetAllocateInfo alloc_info {
        .descriptorPool = _dsc_pool,
        .descriptorSetCount = 1u,
        .pSetLayouts = _set_layouts.data()
    };

    _ubo = BufferTools::create_buffer(
        sizeof(InstanceData),
        vk::BufferUsageFlagBits::eUniformBuffer,
        vk::SharingMode::eExclusive,
        (vk::MemoryPropertyFlagBits::eHostVisible |
        vk::MemoryPropertyFlagBits::eHostCoherent)
    );

    vk::DescriptorBufferInfo buffer_info {
        .buffer = _ubo.buffer,
        .offset = 0u,
        .range = sizeof(InstanceData)
    };
    
    result = LogicalDevice::native().allocateDescriptorSets(
        &alloc_info,
        &_sets[0]
    );
    if(result != vk::Result::eSuccess) {
        CONSOLE_CRITICAL("Could not allocate descriptor sets");
    }

    //--------------------------------------------------------------------------
    // The update
    vk::WriteDescriptorSet set_writes[] {{
        .dstSet = _sets[0],
        .dstBinding = 0u,
        .dstArrayElement = 0u,
        .descriptorCount = 1u,
        .descriptorType = vk::DescriptorType::eUniformBuffer,
        .pImageInfo = nullptr,
        .pBufferInfo = &buffer_info,
        .pTexelBufferView = nullptr
    }};

    LogicalDevice::native().updateDescriptorSets(set_writes, nullptr);
}

// =============================================================================
FrameData::FrameData(FrameData &&other) :
    _cmd_pool    { std::move(other._cmd_pool)    },
    _cmd_buffer  { std::move(other._cmd_buffer)  },
    _ubo         { std::move(other._ubo)         },
    _dsc_pool    { std::move(other._dsc_pool)    },
    _set_layouts { std::move(other._set_layouts) },
    _sets        { std::move(other._sets)        }
{ }

} // namespace vkl
#include "vklearnin/vklearnin.hpp"
#include "vklearnin/engine/FrameData.hpp"

#include "vklearnin/rendering/devices/LogicalDevice.hpp"

namespace vkl {

// =============================================================================
void FrameData::init() {
    _command_pool.create();
    _command_buffer.create(_command_pool);

    PoolSizes pool_sizes;

    {
        auto &layout = _per_frame.layout();
        for(const auto &binding : layout.bindings()) {
            pool_sizes.push_back({
                .type = binding.descriptorType,
                .descriptorCount = 10u
            });

            CONSOLE_TRACE("Per Frame: {}", to_string(binding.descriptorType));
        }
    }

    for(const auto &set : _per_material) {
        auto &layout = set.layout();
        for(const auto &binding : layout.bindings()) {
            pool_sizes.push_back({
                .type = binding.descriptorType,
                .descriptorCount = 10u
            });

            CONSOLE_TRACE("Per Material: {}", to_string(binding.descriptorType));
        }
    }

    for(const auto &set : _per_draw) {
        auto &layout = set.layout();
        for(const auto &binding : layout.bindings()) {
            pool_sizes.push_back({
                .type = binding.descriptorType,
                .descriptorCount = 10u
            });

            CONSOLE_TRACE("Per Draw: {}", to_string(binding.descriptorType));
        }
    }

    _descriptor_pool.create(pool_sizes);
}

// =============================================================================
void FrameData::create() {
    _per_frame.create(_descriptor_pool);
    _frame_layouts.push_back(_per_frame.layout().native());

    for(auto &set : _per_material) {
        set.create(_descriptor_pool);
        _frame_layouts.push_back(set.layout().native());
    }
    for(auto &set : _per_draw) {
        set.create(_descriptor_pool);
        _frame_layouts.push_back(set.layout().native());
    }
}

// =============================================================================
void FrameData::destroy() {
    _command_buffer.destroy();
    _command_pool.destroy();
    
    _per_frame.destroy();

    for(auto &set : _per_material) {
        set.destroy();
    }

    for(auto &set : _per_draw) {
        set.destroy();
    }

    _descriptor_pool.destroy();
}

// =============================================================================
FrameData::FrameData(FrameData &&other) :
    _command_pool    { std::move(other._command_pool) },
    _command_buffer  { std::move(other._command_buffer) },
    _descriptor_pool { std::move(other._descriptor_pool) },
    _per_frame       { std::move(other._per_frame) },
    _per_material    { std::move(other._per_material) },
    _per_draw        { std::move(other._per_draw) }
{ }

} // namespace vkl
#ifndef VKLEARNIN_ENGINE_FRAMEDATA_HPP
#define VKLEARNIN_ENGINE_FRAMEDATA_HPP

#include "vklearnin/system/pch.hpp"
#include "vklearnin/rendering/devices/CmdPool.hpp"
#include "vklearnin/rendering/devices/CmdBuffer.hpp"
#include "vklearnin/shaders/DescriptorPool.hpp"
#include "vklearnin/shaders/DescriptorSet.hpp"

namespace vkl {

class FrameData {
public:
    void init();
    void create();
    void destroy();

    inline const auto & command_pool()   const { return _command_pool; }
    inline const auto & command_buffer() const { return _command_buffer; }

    inline const auto & layouts() const { return _frame_layouts; }

    inline auto & per_frame_set()     { return _per_frame;    }
    inline auto & per_material_sets() { return _per_material; }
    inline auto & per_draw_sets()     { return _per_draw;     } 

    FrameData() = default;
    ~FrameData() = default;

    FrameData(FrameData &&other);
    FrameData(const FrameData &other) = delete;

    FrameData & operator=(FrameData &&other) = delete;
    FrameData & operator=(const FrameData &other) = delete;

private:
    CmdPool   _command_pool;
    CmdBuffer _command_buffer;

    DescriptorPool             _descriptor_pool;
    DescriptorSet              _per_frame;
    std::vector<DescriptorSet> _per_material;
    std::vector<DescriptorSet> _per_draw;

    DescriptorSetLayouts _frame_layouts;
};

} // namespace vkl

#endif // VKLEARNIN_ENGINE_FRAMEDATA_HPP
#ifndef VKLEARNIN_RENDERING_PIPELINE_FLATCOLORPIPELINE_HPP
#define VKLEARNIN_RENDERING_PIPELINE_FLATCOLORPIPELINE_HPP

#include "vklearnin/system/pch.hpp"
#include "vklearnin/rendering/pipeline/Pipeline.hpp"

namespace vkl {

class FlatColorPipeline final : public Pipeline {
public:
    void execute(uint32_t const frame_index,
                 vk::CommandBuffer const &cmd_buffer) override;
};

} // namespce vkl

#endif // VKLEARNIN_RENDERING_PIPELINE_FLATCOLORPIPELINE_HPP
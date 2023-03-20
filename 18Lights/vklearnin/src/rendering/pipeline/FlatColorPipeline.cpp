#include "vklearnin/vklearnin.hpp"
#include "vklearnin/rendering/pipeline/FlatColorPipeline.hpp"

namespace vkl {

void FlatColorPipeline::execute(uint32_t const frame_index,
                                vk::CommandBuffer const &cmd_buffer)
{
    bind(cmd_buffer);
}

} // namespace vkl
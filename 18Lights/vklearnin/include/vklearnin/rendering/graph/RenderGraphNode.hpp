#ifndef VKLEARNIN_RENDERING_GRAPH_RENDERGRAPHNODE_HPP
#define VKLEARNIN_RENDERING_GRAPH_RENDERGRAPHNODE_HPP

#include "vklearnin/system/pch.hpp"

namespace vkl {

struct RenderGraphNode {
    std::string name;

    std::vector<vk::ImageView> input_attachments;
    std::vector<vk::ImageView> output_attachments;


};

} // namespace vkl

#endif // VKLEARNIN_RENDERING_GRAPH_RENDERGRAPHNODE_HPP
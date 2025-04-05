#pragma once

#include "msgui/node/AbstractNode.hpp"

namespace msgui::renderer
{
/* Class responsible for simple rendering of any node */
class NodeRenderer
{
public:
    /**
        Render the current node.

        @param node Node to be rendered
        @param projMat Orthographic projection matrix to be used
        @param frameSizeY Vetical size of the frame (window)
    */
    static void render(AbstractNodePtr node, const glm::mat4& projMat, int32_t frameSizeY);
};
} // namespace msgui::renderer
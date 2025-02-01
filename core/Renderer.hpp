#pragma once

#include <GL/glew.h>

#include "core/Window.hpp"
#include "core/node/AbstractNode.hpp"

namespace msgui
{
/* Class responsible for simple rendering of any node */
class Renderer
{
public:
    /**
        Render the current node.

        @param node Node to be rendered
        @param projMat Orthographic projection matrix to be used
        @param frameSizeY Vetical size of the frame (window)
    */
    static void render(AbstractNodePtr node, const glm::mat4& projMat, int32_t frameSizeY)
    {
        auto& t = node->getTransform();

        // Skip rendering objects that have no viewable area.
        if (t.vScale.x <= 0 || t.vScale.y <= 0) { return; }


        node->getMesh()->bind();
        node->setShaderAttributes();
        node->getShader()->setMat4f("uProjMat", projMat);

        // printf("%d\n", frameSizeY);
        // printf("vPos %d %d vScale %d %d\n", t.vPos.x, t.vPos.y, t.vScale.x, t.vScale.y);
        // Window::setScissorTest(false);
        glScissor(
            t.vPos.x,
            // Height can be computed based on the projMat alone
            frameSizeY - t.vPos.y - t.vScale.y,
            // std::floor((-2.0f / projMat[1][1])) - t.vPos.y - t.vScale.y,
            t.vScale.x,
            t.vScale.y);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)0);
    }
};
} // namespace msgui
#pragma once

#include <GL/glew.h>

#include "core/node/AbstractNode.hpp"

#include <iostream>

namespace msgui
{
class Renderer
{
public:
    static void render(AbstractNodePtr node, const glm::mat4& projMat)
    {
        auto& t = node->getTransform();

        // Skip rendering objects that have no viewable area.
        if (t.vScale.x <= 0 || t.vScale.y <= 0) { return; }

        node->getMesh().bind();
        node->setShaderAttributes();
        node->getShader().setMat4f("uProjMat", projMat);

        glScissor(
            t.vPos.x,
            // Height can be computed based on the projMat alone
            (-2.0f / projMat[1][1]) - t.vPos.y - t.vScale.y,
            t.vScale.x,
            t.vScale.y);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)0);
    }
};
} // namespace msgui
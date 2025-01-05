#pragma once

#include <GL/glew.h>

#include "core/node/AbstractNode.hpp"

#include <iostream>

namespace msgui
{
class Renderer
{
public:
    static void render(AbstractNodePtr node, const glm::mat4 projMat)
    {
        node->getMesh().bind();
        node->setShaderAttributes();
        node->getShader().setMat4f("uProjMat", projMat);

        // TODO: This is NOT efficient
        AbstractNodePtr p = node->getParent().lock();
        if (p)
        {
            auto& t = p->getTransform();
            glScissor(
                t.pos.x,
                // Height can be computed based on the projMat alone
                (-2.0f / projMat[1][1]) - t.pos.y - t.scale.y,
                t.scale.x,
                t.scale.y);
        }

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)0);
    }
};
} // namespace msgui
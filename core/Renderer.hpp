#pragma once

#include <GL/glew.h>

#include "core/node/AbstractNode.hpp"

namespace msgui
{
class Renderer
{
public:
    static void render(AbstractNode* node, const glm::mat4 projMat)
    {
        node->getMesh().bind();
        node->setShaderAttributes();
        node->getShader().setMat4f("uProjMat", projMat);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)0);
    }
};
}
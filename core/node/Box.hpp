#pragma once

#include <memory>

#include "core/node/AbstractNode.hpp"

namespace msgui
{
class Box : public AbstractNode
{
public:
    Box(const std::string& name);

    // Setters
    void setColor(const glm::vec4& color);

private:
    // Overrides
    void setShaderAttributes() override;

private:
    glm::vec4 color_{1.0f};

};
using BoxPtr = std::shared_ptr<Box>;
} // namespace msgui
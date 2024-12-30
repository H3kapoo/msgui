#pragma once

#include <glm/glm.hpp>

namespace msgui
{
struct Transform
{
public:
    // Normal
    glm::mat4& computeModelMatrix();

    // Setters
    void setPos(const glm::vec3& pos);
    void setScale(const glm::vec3& scale);

public:
    glm::vec3 pos{0, 0, 1};
    glm::vec3 scale{1};
    glm::mat4 modelMatrix;
    bool dirty{false};
};
} // namespace msgui
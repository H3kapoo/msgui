#pragma once

#include <glm/glm.hpp>

namespace msgui
{
struct Transform
{
public:
    void setPos(const glm::vec3& pos);
    void setScale(const glm::vec3& scale);
    glm::mat4& computeModelMatrix();

public:
    glm::vec3 pos{0};
    glm::vec3 scale{1};
    glm::mat4 modelMatrix;
    bool dirty{false};
};
} // namespace msgui
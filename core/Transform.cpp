#include "Transform.hpp"

#include <glm/gtc/matrix_transform.hpp>

namespace msgui
{
void Transform::setPos(const glm::vec3& pos)
{
    this->pos = pos;
}

void Transform::setScale(const glm::vec3& scale)
{
    this->scale = scale;
}

glm::mat4& Transform::computeModelMatrix()
{
    // if (!dirty)
    // {
    //     return modelMatrix;
    // };

    modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, pos);
    modelMatrix = glm::scale(modelMatrix, scale);
    dirty = false;
    return modelMatrix;
}
} // namespace msgui
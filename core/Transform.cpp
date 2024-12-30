#include "Transform.hpp"

#include <glm/gtc/matrix_transform.hpp>

namespace msgui
{
// ---- Normal ---- //
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

// ---- Setters ---- //
void Transform::setPos(const glm::vec3& posIn)
{
    pos = posIn;
}

void Transform::setScale(const glm::vec3& scaleIn)
{
    scale = scaleIn;
}
} // namespace msgui
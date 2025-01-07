#include "Transform.hpp"

#include <cstdio>
#include <glm/gtc/matrix_transform.hpp>

namespace msgui
{
glm::mat4& Transform::computeModelMatrix()
{
    modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, pos);
    modelMatrix = glm::scale(modelMatrix, scale);
    return modelMatrix;
}

void Transform::computeViewableArea(const Transform& otherTrans)
{
    const glm::ivec2 posScale = pos + scale;
    const glm::ivec2 otherVPosScale = otherTrans.vPos + otherTrans.vScale;

    vPos.x = std::max(otherTrans.vPos.x, (int32_t)pos.x);
    vPos.y = std::max(otherTrans.vPos.y, (int32_t)pos.y);
    vScale.x = std::min(otherVPosScale.x, posScale.x) - vPos.x;
    vScale.y = std::min(otherVPosScale.y, posScale.y) - vPos.y;
}
} // namespace msgui
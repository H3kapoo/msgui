#include "Transform.hpp"

#include <cstdio>
#include <glm/gtc/matrix_transform.hpp>

namespace msgui::layoutengine::utils
{
glm::mat4& Transform::computeModelMatrix()
{
    modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, pos);
    modelMatrix = glm::scale(modelMatrix, scale);
    return modelMatrix;
}

void Transform::computeViewableArea(const Transform& otherTrans, const utils::Layout::TBLR& otherBorder)
{
    /* Available viewing space shinks with parent's border */
    const glm::vec2 posScale = pos + scale;
    const glm::ivec2 newVscale = otherTrans.vScale
        - glm::ivec2{otherBorder.left + otherBorder.right, otherBorder.top + otherBorder.bot};
    const glm::ivec2 newVPos = otherTrans.vPos + glm::ivec2{otherBorder.left, otherBorder.top};
    const glm::vec2 otherVPosScale = newVPos + newVscale;

    vPos.x = std::max(newVPos.x, (int32_t)pos.x);
    vPos.y = std::max(newVPos.y, (int32_t)pos.y);
    vScale.x = std::min(otherVPosScale.x, posScale.x) - vPos.x;
    vScale.y = std::min(otherVPosScale.y, posScale.y) - vPos.y;
}
} // namespace msgui::layoutengine::utils
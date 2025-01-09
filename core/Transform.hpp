#pragma once

#include <glm/glm.hpp>

#include "core/node/utils/LayoutData.hpp"

namespace msgui
{
struct Transform
{
public:
    glm::mat4& computeModelMatrix();
    void computeViewableArea(const Transform& otherTrans, const Layout::TBLR& otherBorder);

public:
    glm::vec3 pos{0, 0, 1};
    glm::vec3 scale{1};
    glm::ivec2 vPos{0};
    glm::ivec2 vScale{1};
    glm::mat4 modelMatrix;
};
} // namespace msgui
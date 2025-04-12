#pragma once

#include <glm/glm.hpp>

#include "msgui/layoutEngine/utils/LayoutData.hpp"

namespace msgui::layoutengine::utils
{
/* Holds & computes position and scale info */
struct Transform
{
public:
    /**
        Compute the model matrix based on current scale and position.

        @return Computed model matrix
    */
    glm::mat4& computeModelMatrix();

    /**
        Compute the model's viewable area based on another transform plus the borders of the other model.
        In this case, otherTrans+otherBorder acts as the parent's bounding box so what we actually calculate
        here is how much of this model is visible inside of the parent's model.

        @param otherTrans Parent's transform, for all intents and purposes
        @param otherBorder Parent's border size, for all intents and purposes
    */
    void computeViewableArea(const Transform& otherTrans, const utils::Layout::TBLR& otherBorder);

public:
    glm::vec3 pos{0, 0, 1};
    glm::vec3 scale{1};
    glm::ivec2 vPos{0};
    glm::ivec2 vScale{0};
    glm::mat4 modelMatrix;
};
using TransformPtr = Transform*;
} // namespace msgui::layoutengine::utils
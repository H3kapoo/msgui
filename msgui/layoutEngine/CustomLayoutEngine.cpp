#include "msgui/layoutEngine/CustomLayoutEngine.hpp"
#include "msgui/layoutEngine/utils/LayoutData.hpp"
#include <cmath>
#include <glm/ext/vector_int2.hpp>

namespace msgui
{
using namespace layoutengine::utils;

glm::vec2 CustomLayoutEngine::process(const AbstractNodePtr& node)
{
    const auto& layout = node->getLayout();
    const bool isLayoutHorizontal = layout.type == Layout::Type::HORIZONTAL;
    const bool isLayoutVertical = layout.type == Layout::Type::VERTICAL;

    if (isLayoutHorizontal || isLayoutVertical)
    {
        computeSubNodesScale(node);
        computeSubNodesPosition(node);
    }
    else
    {
        log_.debugLn("Unhandled layout type for now");
    }

    return {0, 0};
}

void CustomLayoutEngine::computeSubNodesScale(const AbstractNodePtr& node)
{
    const auto& layout = node->getLayout();
    const bool isLayoutHorizontal = layout.type == Layout::Type::HORIZONTAL;
    const auto& nodeScale = node->getTransform().scale; // shall add padding also and borders
    auto& subNodes = node->getChildren();
    glm::vec2 nodeAvailableScale = nodeScale;
    glm::ivec2 fillSubNodesCnt{!isLayoutHorizontal, isLayoutHorizontal};
    for (const auto& subNode : subNodes)
    {
        auto& trScale = subNode->getTransform().scale;
        const auto& subNodeLayout = subNode->getLayout();
        const bool isXFit = subNodeLayout.scaleType.x == Layout::ScaleType::FIT;
        const bool isYFit = subNodeLayout.scaleType.y == Layout::ScaleType::FIT;
        const bool isXPx = subNodeLayout.scaleType.x == Layout::ScaleType::PX;
        const bool isYPx = subNodeLayout.scaleType.y == Layout::ScaleType::PX;
        const bool isXRel = subNodeLayout.scaleType.x == Layout::ScaleType::REL;
        const bool isYRel = subNodeLayout.scaleType.y == Layout::ScaleType::REL;
        const bool isXFill = subNodeLayout.scaleType.x == Layout::ScaleType::FILL;
        const bool isYFill = subNodeLayout.scaleType.y == Layout::ScaleType::FILL;

        if (isXFill && isLayoutHorizontal) { fillSubNodesCnt.x++; }
        if (isYFill && !isLayoutHorizontal) { fillSubNodesCnt.y++; }

        if (isXPx) { trScale.x = subNodeLayout.scale.x; }
        if (isYPx) { trScale.y = subNodeLayout.scale.y; }

        if (isXRel) { trScale.x = subNodeLayout.scale.x * nodeScale.x; }
        if (isYRel) { trScale.y = subNodeLayout.scale.y * nodeScale.y; }

        if (isXFit || isYFit)
        {
            const glm::ivec2 fitScale = computeFitScale(subNode);
            trScale.x = isXFit ? fitScale.x : trScale.x;
            trScale.y = isYFit ? fitScale.y : trScale.y;
        }

        if (!isXFill && isLayoutHorizontal) { nodeAvailableScale.x -= trScale.x; }
        if (!isYFill && !isLayoutHorizontal) { nodeAvailableScale.y -= trScale.y; }

        trScale.x = std::max(0.0f, trScale.x);
        trScale.y = std::max(0.0f, trScale.y);
    }

    if (fillSubNodesCnt.x <= 0 && fillSubNodesCnt.y <= 0) { return; }

    const glm::vec2 fillEqualPart{nodeAvailableScale.x / fillSubNodesCnt.x, nodeAvailableScale.y / fillSubNodesCnt.y};
    for (const auto& subNode : subNodes)
    {
        const auto& subNodeLayout = subNode->getLayout();
        const bool isXFill = subNodeLayout.scaleType.x == Layout::ScaleType::FILL;
        const bool isYFill = subNodeLayout.scaleType.y == Layout::ScaleType::FILL;

        if (!isXFill && !isYFill) { continue; }

        auto& trScale = subNode->getTransform().scale;
        if (isXFill) { trScale.x = fillEqualPart.x; }
        if (isYFill) { trScale.y = fillEqualPart.y; }

        trScale.x = std::max(0.0f, trScale.x);
        trScale.y = std::max(0.0f, trScale.y);
    }
}

glm::vec2 CustomLayoutEngine::computeFitScale(const AbstractNodePtr& node)
{
    /* We ended up here because the node is FIT, so we need to compute what the children's scale would be. */
    glm::vec2 totalScale{0, 0};
    const auto& layout = node->getLayout();
    const bool isLayoutHorizontal = layout.type == Layout::Type::HORIZONTAL;
    auto& subNodes = node->getChildren();
    for (const auto& subNode : subNodes)
    {
        const auto& subNodeLayout = subNode->getLayout();
        const bool isXFit = subNodeLayout.scaleType.x == Layout::ScaleType::FIT;
        const bool isYFit = subNodeLayout.scaleType.y == Layout::ScaleType::FIT;
        glm::vec2 fitScale{0, 0};
        if (isXFit || isYFit) { fitScale = computeFitScale(subNode); }

        if (subNodeLayout.scaleType.x == Layout::ScaleType::PX)
        {
            totalScale.x = !isLayoutHorizontal
                ? std::max(totalScale.x, subNodeLayout.scale.x)
                : totalScale.x + subNodeLayout.scale.x;
        }
        else if (subNodeLayout.scaleType.x == Layout::ScaleType::FIT)
        {
            totalScale.x = !isLayoutHorizontal
                ? std::max(totalScale.x, fitScale.x)
                : totalScale.x + fitScale.x;
        }
        else
        {
            log_.debugLn("Invalid scale type");
        }

        if (subNodeLayout.scaleType.y == Layout::ScaleType::PX)
        {
            totalScale.y = isLayoutHorizontal
                ? std::max(totalScale.y, subNodeLayout.scale.y)
                : totalScale.y + subNodeLayout.scale.y;
        }
        else if (subNodeLayout.scaleType.y == Layout::ScaleType::FIT)
        {
            totalScale.y = isLayoutHorizontal
                ? std::max(totalScale.y, fitScale.y)
                : totalScale.y + fitScale.y;
        }
        else
        {
            log_.debugLn("Invalid scale type");
        }
    }
    return totalScale;
}

void CustomLayoutEngine::computeSubNodesPosition(const AbstractNodePtr& node)
{
    glm::vec2 startPos{0, 0};
    const auto& layout = node->getLayout();
    const auto& nodeTrPos = node->getTransform().pos;
    const bool isLayoutHorizontal = layout.type == Layout::Type::HORIZONTAL;
    auto& subNodes = node->getChildren();
    for (const auto& subNode : subNodes)
    {
        const auto& subNodeLayout = subNode->getLayout();
        auto& trPos = subNode->getTransform().pos;
        auto& trScale = subNode->getTransform().scale;
        trPos = {startPos.x, startPos.y, trPos.z};

        if (isLayoutHorizontal) { startPos.x += trScale.x; }
        if (!isLayoutHorizontal) { startPos.y += trScale.y; }

        /* Change reference frame for each subnode to the node's frame. */
        // trPos += glm::vec3{nodeTrPos.x, nodeTrPos.y, 0};
    }
}

} // namespace msgui
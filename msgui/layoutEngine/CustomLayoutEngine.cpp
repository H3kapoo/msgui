#include "msgui/layoutEngine/CustomLayoutEngine.hpp"
#include "msgui/layoutEngine/utils/LayoutData.hpp"
#include <format>

namespace msgui
{
using namespace layoutengine::utils;
template<typename T>
using Result = CustomLayoutEngine::Result<T>;
using Void = CustomLayoutEngine::Void;

Result<glm::vec2> CustomLayoutEngine::process(const AbstractNodePtr& node)
{
    const auto& layout = node->getLayout();
    const bool isLayoutHorizontal = layout.type == Layout::Type::HORIZONTAL;
    const bool isLayoutVertical = layout.type == Layout::Type::VERTICAL;

    if (isLayoutHorizontal || isLayoutVertical)
    {
        if (const auto& result = computeSubNodesScale(node); !result.error.empty())
        {
            return Result<glm::vec2>{.error = result.error};
        };

        if (const auto& result = computeSubNodesPosition(node); !result.error.empty())
        {
            return Result<glm::vec2>{.error = result.error};
        };
    }
    else
    {   // Shall return result
        return Result<glm::vec2>{.error = "Unsupported layout type"};
    }

    return Result<glm::vec2>{};
}

Result<Void> CustomLayoutEngine::computeSubNodesScale(const AbstractNodePtr& node)
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
        const auto& subNodeScale = subNode->getLayout().newScale;
        const bool isXFit = subNodeScale.x.type == Layout::ScaleType::FIT;
        const bool isYFit = subNodeScale.y.type == Layout::ScaleType::FIT;
        const bool isXPx = subNodeScale.x.type == Layout::ScaleType::PX;
        const bool isYPx = subNodeScale.y.type == Layout::ScaleType::PX;
        const bool isXRel = subNodeScale.x.type == Layout::ScaleType::REL;
        const bool isYRel = subNodeScale.y.type == Layout::ScaleType::REL;
        const bool isXFill = subNodeScale.x.type == Layout::ScaleType::FILL;
        const bool isYFill = subNodeScale.y.type == Layout::ScaleType::FILL;

        if (isXFill && isLayoutHorizontal) { fillSubNodesCnt.x++; }
        if (isYFill && !isLayoutHorizontal) { fillSubNodesCnt.y++; }

        if (isXPx) { trScale.x = subNodeScale.x.value; }
        if (isYPx) { trScale.y = subNodeScale.y.value; }

        if (isXRel) { trScale.x = subNodeScale.x.value * nodeScale.x; }
        if (isYRel) { trScale.y = subNodeScale.y.value * nodeScale.y; }

        if (isXFit || isYFit)
        {
            const CustomLayoutEngine::Result<glm::vec2> fitScale = computeFitScale(subNode);
            if (!fitScale.error.empty()) { return Result<Void>{.error = fitScale.error}; }

            trScale.x = isXFit ? fitScale.value.x : trScale.x;
            trScale.y = isYFit ? fitScale.value.y : trScale.y;
        }

        if (!isXFill && isLayoutHorizontal) { nodeAvailableScale.x -= trScale.x; }
        if (!isYFill && !isLayoutHorizontal) { nodeAvailableScale.y -= trScale.y; }

        trScale.x = std::max(0.0f, trScale.x);
        trScale.y = std::max(0.0f, trScale.y);
    }

    if (fillSubNodesCnt.x <= 0 && fillSubNodesCnt.y <= 0) { return Result<Void>{}; }

    const glm::vec2 fillEqualPart{nodeAvailableScale.x / fillSubNodesCnt.x, nodeAvailableScale.y / fillSubNodesCnt.y};
    for (const auto& subNode : subNodes)
    {
        const auto& subNodeScale = subNode->getLayout().newScale;
        const bool isXFill = subNodeScale.x.type == Layout::ScaleType::FILL;
        const bool isYFill = subNodeScale.y.type == Layout::ScaleType::FILL;

        if (!isXFill && !isYFill) { continue; }

        auto& trScale = subNode->getTransform().scale;
        if (isXFill) { trScale.x = fillEqualPart.x; }
        if (isYFill) { trScale.y = fillEqualPart.y; }

        trScale.x = std::max(0.0f, trScale.x);
        trScale.y = std::max(0.0f, trScale.y);
    }

    return Result<Void>{};
}

Result<glm::vec2> CustomLayoutEngine::computeFitScale(const AbstractNodePtr& node)
{
    /* We ended up here because the node is FIT, so we need to compute what the children's scale would be. */
    glm::vec2 totalScale{0, 0};
    const auto& layout = node->getLayout();
    const bool isLayoutHorizontal = layout.type == Layout::Type::HORIZONTAL;
    auto& subNodes = node->getChildren();

    if (subNodes.empty())
    {
        return Result<glm::vec2>{
            .error = std::format("Node '{}' is ScaleType::FIT on one of the axis but it has no "
                 "subNodes to FIT around!", node->getName())};
    }

    const auto nodeScale = node->getLayout().newScale;
    for (const auto& subNode : subNodes)
    {
        const auto& subNodeScale = subNode->getLayout().newScale;
        const bool isXFit = subNodeScale.x.type == Layout::ScaleType::FIT;
        const bool isYFit = subNodeScale.y.type == Layout::ScaleType::FIT;
        glm::vec2 fitScale{0, 0};
        if (isXFit || isYFit)
        {
            Result<glm::vec2> result = computeFitScale(subNode);
            if (!result.error.empty()) { return Result<glm::vec2>{.error = result.error}; }
        }

        if (nodeScale.x.type == Layout::ScaleType::FIT)
        {
            if (subNodeScale.x.type == Layout::ScaleType::PX)
            {
                totalScale.x = !isLayoutHorizontal
                ? std::max(totalScale.x, subNodeScale.x.value)
                : totalScale.x + subNodeScale.x.value;
            }
            else if (subNodeScale.x.type == Layout::ScaleType::FIT)
            {
                totalScale.x = !isLayoutHorizontal
                    ? std::max(totalScale.x, fitScale.x)
                    : totalScale.x + fitScale.x;
            }
            else
            {
                return Result<glm::vec2>{
                    .error = std::format("Node '{}' is ScaleType::FIT on X axis but subNode '{}' is "
                        "NOT ScaleType::FIT or ScaleType::PX on that axis", node->getName(), subNode->getName())};
            }
        }

        if (nodeScale.y.type == Layout::ScaleType::FIT)
        {
            if (subNodeScale.y.type == Layout::ScaleType::PX)
            {
                totalScale.y = isLayoutHorizontal
                    ? std::max(totalScale.y, subNodeScale.x.value)
                    : totalScale.y + subNodeScale.x.value;
            }
            else if (subNodeScale.y.type == Layout::ScaleType::FIT)
            {
                totalScale.y = isLayoutHorizontal
                    ? std::max(totalScale.y, fitScale.y)
                    : totalScale.y + fitScale.y;
            }
            else
            {
                return Result<glm::vec2>{
                    .error = std::format("Node '{}' is ScaleType::FIT on Y axis but subNode '{}' is "
                        "NOT ScaleType::FIT or ScaleType::PX on that axis", node->getName(), subNode->getName())};
            }
        }
    }
    return Result<glm::vec2>{.value = totalScale};
}

CustomLayoutEngine::Result<Void> CustomLayoutEngine::computeSubNodesPosition(const AbstractNodePtr& node)
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
        trPos += glm::vec3{nodeTrPos.x, nodeTrPos.y, 0};
    }

    return Result<Void>{};
}

} // namespace msgui
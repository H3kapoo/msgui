#include "msgui/layoutEngine/CustomLayoutEngine.hpp"
#include "msgui/layoutEngine/utils/LayoutData.hpp"
#include "msgui/node/AbstractNode.hpp"
#include <format>

namespace msgui
{
/*
    Note:
        .getTransform().scale contains only the border+padding data, margins need to be added manually
        .getTransform().pos again, doesn't contain margin data. So add that as well when figuring out the
            real position of the node.
*/
using namespace layoutengine::utils;
template<typename T>
using Result = CustomLayoutEngine::Result<T>;
using Void = CustomLayoutEngine::Void;

#define SKIP_COMPOSED_DIRECTIONS\
    case Layout::TOP_LEFT:\
    case Layout::TOP_RIGHT:\
    case Layout::CENTER_LEFT:\
    case Layout::CENTER_RIGHT:\
    case Layout::CENTER_TOP:\
    case Layout::CENTER_BOTTOM:\
    case Layout::BOTTOM_LEFT:\
    case Layout::BOTTOM_RIGHT:\
        return Result<Void>{.error = "Unsupported subNodes compound alignment!"};\

#define SKIP_HORIZONTAL_DIRECTIONS\
    case Layout::LEFT:\
    case Layout::RIGHT:\
        return Result<Void>{.error = "Unsupported subNodes alignment for Y axis!"};\
        
#define SKIP_VERTICAL_DIRECTIONS\
    case Layout::TOP:\
    case Layout::BOTTOM:\
        return Result<Void>{.error = "Unsupported subNodes alignment for X axis!"};\

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
            // check if the REL/PX scale can hold the elements
            const CustomLayoutEngine::Result<glm::vec2> fitScale = computeFitScale(subNode);
            if (!fitScale.error.empty()) { return Result<Void>{.error = fitScale.error}; }

            trScale.x = isXFit ? fitScale.value.x : trScale.x;
            trScale.y = isYFit ? fitScale.value.y : trScale.y;
        }

        if (!isXFill && isLayoutHorizontal) { nodeAvailableScale.x -= trScale.x; }
        if (!isYFill && !isLayoutHorizontal) { nodeAvailableScale.y -= trScale.y; }

        if (trScale.x <= 0 || trScale.y <= 0)
        {
            return Result<Void>{.error = std::format("Node '{}' computes to a zero/negative scale on one of it's axis",
                subNode->getName()) };
        }

        // trScale.x = std::max(0.0f, trScale.x);
        // trScale.y = std::max(0.0f, trScale.y);
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
    glm::vec2 nonFitTotalScale{0, 0};
    glm::vec2 maximum{0, 0};
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
            float scaleValue = 0;
            if (subNodeScale.x.type == Layout::ScaleType::PX) { scaleValue = subNodeScale.x.value; }
            else if (subNodeScale.x.type == Layout::ScaleType::FIT) { scaleValue = fitScale.x; }
            else
            {
                return Result<glm::vec2>{
                    .error = std::format("Node '{}' is ScaleType::FIT on X axis but subNode '{}' is "
                        "NOT ScaleType::FIT or ScaleType::PX on that axis", node->getName(), subNode->getName())};
            }

            totalScale.x = !isLayoutHorizontal
                ? std::max(totalScale.x, scaleValue)
                : totalScale.x + scaleValue;

            if (layout.allowWrap && !isLayoutHorizontal && nodeScale.y.type != Layout::ScaleType::FIT)
            {
                nonFitTotalScale.y += subNodeScale.y.value;
                maximum.x = std::max(maximum.x, subNodeScale.x.value);
                if (nonFitTotalScale.y > node->getTransform().scale.y)
                {
                    totalScale.x += maximum.x;
                    nonFitTotalScale.y = subNodeScale.y.value;
                    maximum.x = 0;
                }
            }
        }

        if (nodeScale.y.type == Layout::ScaleType::FIT)
        {
            float scaleValue = 0;
            if (subNodeScale.y.type == Layout::ScaleType::PX) { scaleValue = subNodeScale.y.value; }
            else if (subNodeScale.y.type == Layout::ScaleType::FIT) { scaleValue = fitScale.y; }
            else
            {
                return Result<glm::vec2>{
                    .error = std::format("Node '{}' is ScaleType::FIT on Y axis but subNode '{}' is "
                        "NOT ScaleType::FIT or ScaleType::PX on that axis", node->getName(), subNode->getName())};
            }

            totalScale.y = isLayoutHorizontal
                ? std::max(totalScale.y, scaleValue)
                : totalScale.y + scaleValue;

            /* If Y scaleType is FIT but X scaleType is not and wrap is enabled, it can happen for some
               subnodes to jump to the next row and in that case we need to increase the FIT computed scale
               to account for that Y increase. We take the maximum Y value that can best fit the elements drop. */
            if (layout.allowWrap && isLayoutHorizontal && nodeScale.x.type != Layout::ScaleType::FIT)
            {
                nonFitTotalScale.x += subNodeScale.x.value;
                maximum.y = std::max(maximum.y, subNodeScale.y.value);
                if (nonFitTotalScale.x > node->getTransform().scale.x)
                {
                    totalScale.y += maximum.y;
                    nonFitTotalScale.x = subNodeScale.x.value;
                    maximum.y = 0;
                }
            }
        }
    }

    return Result<glm::vec2>{.value = totalScale};
}

CustomLayoutEngine::Result<Void> CustomLayoutEngine::computeSubNodesPosition(const AbstractNodePtr& node)
{
    /*
        Note: Wrap doesn't work with FILL subnodes. It's best to use fixed size elements
    */
    glm::vec2 startPos{0, 0};
    glm::vec2 maximum{0, 0};
    uint32_t sliceStartIdx{0};
    uint32_t sliceEndIdx{0};
    auto& subNodes = node->getChildren();
    const auto& layout = node->getLayout();
    const auto& nodeTrPos = node->getTransform().pos;
    const auto& nodeScale = node->getTransform().scale;
    const bool isWrapEnabled = layout.allowWrap;
    const bool isLayoutHorizontal = layout.type == Layout::Type::HORIZONTAL;
    for (const auto& subNode : subNodes)
    {
        const auto& subNodeLayout = subNode->getLayout();
        auto& trPos = subNode->getTransform().pos;
        auto& trScale = subNode->getTransform().scale;

        /* Deal with wrapping if needed */
        if (isWrapEnabled && isLayoutHorizontal)
        {
            const bool layoutOverflows = startPos.x + trScale.x > nodeScale.x;
            if (layoutOverflows)
            {
                selfAlignSubNodeSlice(node, maximum, sliceStartIdx, sliceEndIdx);
                sliceStartIdx += sliceEndIdx;

                startPos.x = 0; // should be padding + borders
                startPos.y += maximum.y;
                maximum.y = 0;

                // if (layout.newScale.y.type == Layout::ScaleType::FIT)
                // {
                //     node->getTransform().scale.y += trScale.y;
                // }
            }
        }
        else if (isWrapEnabled && !isLayoutHorizontal)
        {
            const bool layoutOverflows = startPos.y + trScale.y > nodeScale.y;
            if (layoutOverflows)
            {
                selfAlignSubNodeSlice(node, maximum, sliceStartIdx, sliceEndIdx);
                sliceStartIdx += sliceEndIdx;

                startPos.y = 0;
                startPos.x += maximum.x;
                maximum.x = 0;
            }
        }

        /* Put elements into position relative to the (0, 0) reference frame. */
        trPos = {startPos.x, startPos.y, trPos.z};

        if (isLayoutHorizontal)
        {
            startPos.x += trScale.x;
            maximum.y = std::max(maximum.y, trScale.y);
        }
        else if (!isLayoutHorizontal)
        {
            startPos.y += trScale.y;
            maximum.x = std::max(maximum.x, trScale.x);
        }

        /* Change reference frame for each subnode to the node's frame. */
        trPos += glm::vec3{nodeTrPos.x, nodeTrPos.y, 0};

        sliceEndIdx++;
    }

    /* Align each subNode in relationship to the other nodes on the same row/column */
    selfAlignSubNodeSlice(node, maximum, sliceStartIdx, subNodes.size());

    /* Compute overflow as it is needed in order to satisfy node's alignSubNodes policy. */
    const glm::vec2 overflow = computeOverflow(node);
    if (const auto& result = alignSubNodes(node, overflow); !result.error.empty())
    {
        return Result<Void>{.error = result.error};
    }

    return Result<Void>{};
}

Result<Void> CustomLayoutEngine::alignSubNodes(const AbstractNodePtr& node, const glm::vec2 computedOverflow)
{
    glm::vec2 positionToAdd{0, 0};
    const auto alignValue = node->getLayout().alignChild;
    switch(alignValue.x)
    {
        SKIP_COMPOSED_DIRECTIONS;
        SKIP_VERTICAL_DIRECTIONS;
        case Layout::LEFT:
            break; /* By default at left, nothing to do. */
        case Layout::CENTER:
            positionToAdd.x = std::max(0.0f, -computedOverflow.x * 0.5f);
            break;
        case Layout::RIGHT:
            positionToAdd.x = std::max(0.0f, -computedOverflow.x);
            break;
        default:
            return Result<Void>{.error = "Unsupported subNodes alignment!"};
    }

    switch(alignValue.y)
    {
        SKIP_COMPOSED_DIRECTIONS;
        SKIP_HORIZONTAL_DIRECTIONS;
        case Layout::TOP:
            break; /* By default at top, nothing to do. */
        case Layout::CENTER:
            positionToAdd.y = std::max(0.0f, -computedOverflow.y * 0.5f);
            break;
        case Layout::BOTTOM:
            positionToAdd.y = std::max(0.0f, -computedOverflow.y);
            break;
        default:
            return Result<Void>{.error = "Unsupported subNodes alignment!"};
    }

    auto& subNodes = node->getChildren();
    for (auto& subNode : subNodes)
    {
        auto& subNodePos = subNode->getTransform().pos;
        subNodePos.x += positionToAdd.x;
        subNodePos.y += positionToAdd.y;
    }

    return Result<Void>{};
}

Result<Void> CustomLayoutEngine::selfAlignSubNodeSlice(const AbstractNodePtr& node, const glm::vec2 maximum,
    const uint32_t startIdx, const uint32_t endIdx)
{
    auto& subNodes = node->getChildren();
    const bool isLayoutHorizontal = node->getLayout().type == Layout::Type::HORIZONTAL;
    for (uint32_t i = startIdx; i < endIdx; ++i)
    {
        auto& subNodePos = subNodes[i]->getTransform().pos;
        const auto& subNodeScale = subNodes[i]->getTransform().scale;
        const auto& subNodeLayout = subNodes[i]->getLayout();

        if (isLayoutHorizontal)
        {
            switch (subNodeLayout.alignSelf)
            {
                SKIP_COMPOSED_DIRECTIONS;
                SKIP_HORIZONTAL_DIRECTIONS;
                case Layout::TOP:
                    break; /* Nothing to do */
                case Layout::CENTER:
                    subNodePos.y += (maximum.y - subNodeScale.y) * 0.5f;
                    break;
                case Layout::BOTTOM:
                    subNodePos.y += maximum.y - subNodeScale.y;
                    break;
                default:
                    return Result<Void>{.error = "Unsupported subNodes self alignment!"};
            }
        }
        else if (!isLayoutHorizontal)
        {
            switch (subNodeLayout.alignSelf)
            {
                SKIP_COMPOSED_DIRECTIONS;
                SKIP_VERTICAL_DIRECTIONS;
                case Layout::LEFT:
                    break; /* Nothing to do */
                case Layout::CENTER:
                    subNodePos.x += (maximum.x - subNodeScale.x) * 0.5f;
                    break;
                case Layout::RIGHT:
                    subNodePos.y += maximum.x - subNodeScale.x;
                    break;
                default:
                    return Result<Void>{.error = "Unsupported subNodes self alignment!"};
            }
        }
    }

    return Result<Void>{};
}

glm::vec2 CustomLayoutEngine::computeOverflow(const AbstractNodePtr& node)
{
    /* Subnodes need to in the node's reference frame */
    const auto& subNodes = node->getChildren();
    const auto nodeScale = node->getTransform().scale; // should be scale with padding and borders added
    const auto nodePos = node->getTransform().pos; // should be scale with padding and borders added

    glm::vec2 maximumPoints{0, 0};
    for (const auto& subNode : subNodes)
    {
        const auto& subNodeScale = subNode->getTransform().scale; // shall contain margins
        const auto& subNodePos = subNode->getTransform().pos; // same
        maximumPoints.x = std::max(maximumPoints.x, subNodePos.x + subNodeScale.x);
        maximumPoints.y = std::max(maximumPoints.y, subNodePos.y + subNodeScale.y);
    }

    /* Positive values in case of overflow, negative in case of underflow. */
    return {maximumPoints.x - (nodePos.x + nodeScale.x), maximumPoints.y - (nodePos.y + nodeScale.y)};
}

#undef SKIP_COMPOSED_DIRECTIONS
#undef SKIP_HORIZONTAL_DIRECTIONS
#undef SKIP_VERTICAL_DIRECTIONS
} // namespace msgui::layoutengine
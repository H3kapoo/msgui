#include "msgui/layoutEngine/CustomLayoutEngine.hpp"
#include "msgui/layoutEngine/utils/LayoutData.hpp"
#include "msgui/node/AbstractNode.hpp"
#include "msgui/node/Box.hpp"
#include "msgui/node/Slider.hpp"
#include "msgui/node/utils/SliderKnob.hpp"
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

#define SKIP_SCROLL_NODE(node)\
    if (node->getType() == AbstractNode::NodeType::SCROLL) { continue; }\

Result<glm::vec2> CustomLayoutEngine::process(const AbstractNodePtr& node)
{
    const auto& layout = node->getLayout();
    const bool isLayoutHorizontal = layout.type == Layout::Type::HORIZONTAL;
    const bool isLayoutVertical = layout.type == Layout::Type::VERTICAL;
    const bool isLayoutGrid = layout.type == Layout::Type::GRID;
    const auto nodeType = node->getType();

    /* Handling of uncommon node types */
    if (nodeType == AbstractNode::NodeType::SCROLL || nodeType == AbstractNode::NodeType::SLIDER)
    {
        handlerSliderNode(node);
        return Result<glm::vec2>{};
    }

    /* Compute which scrollbar is active and their contribution */
    Result<ScrollContribution> sc = computeScrollNodeContribution(node);
    if (!sc.error.empty()) { return Result<glm::vec2>{.error = sc.error}; }

    /* Handling of common Horizontal and Verical layouts*/
    if (isLayoutHorizontal || isLayoutVertical)
    {
        if (const auto& result = computeSubNodesScale(node, sc.value); !result.error.empty())
        {
            return Result<glm::vec2>{.error = result.error};
        };

        if (const auto& result = computeSubNodesPosition(node, sc.value); !result.error.empty())
        {
            return Result<glm::vec2>{.error = result.error};
        };
    }
    else if (isLayoutGrid)
    {
        return Result<glm::vec2>{.error = "GRID layout not implemeted yet"};
    }
    else
    {
        return Result<glm::vec2>{.error = "Unsupported layout type"};
    }

    return Result<glm::vec2>{};
}

Result<Void> CustomLayoutEngine::computeSubNodesScale(const AbstractNodePtr& node, const ScrollContribution& sc)
{
    const auto& layout = node->getLayout();
    const bool isLayoutHorizontal = layout.type == Layout::Type::HORIZONTAL;
    const auto& freeNodeScale = computeNodeFreeSpace(node, sc); // shall add padding also and borders
    auto& subNodes = node->getChildren();
    glm::vec2 nodeAvailableScale = freeNodeScale;
    glm::ivec2 fillSubNodesCnt{!isLayoutHorizontal, isLayoutHorizontal};
    glm::vec2 maxMargin{0, 0};
    for (const auto& subNode : subNodes)
    {
        auto& trScale = subNode->getTransform().scale;
        const auto& subNodeScale = subNode->getLayout().newScale;
        const auto& subNodeMargin = subNode->getLayout().margin;
        const bool isXFit = subNodeScale.x.type == Layout::ScaleType::FIT;
        const bool isYFit = subNodeScale.y.type == Layout::ScaleType::FIT;
        const bool isXPx = subNodeScale.x.type == Layout::ScaleType::PX;
        const bool isYPx = subNodeScale.y.type == Layout::ScaleType::PX;
        const bool isXRel = subNodeScale.x.type == Layout::ScaleType::REL;
        const bool isYRel = subNodeScale.y.type == Layout::ScaleType::REL;
        const bool isXFill = subNodeScale.x.type == Layout::ScaleType::FILL;
        const bool isYFill = subNodeScale.y.type == Layout::ScaleType::FILL;

        /* Scrollbars are special Sliders which need to have their scale calculated separately. */
        if (subNode->getType() == AbstractNode::NodeType::SCROLL)
        {
            /* Scrollbar scale will be affected only by node border, not padding. */
            const glm::vec2 customScrollScale = {
                node->getTransform().scale.x - layout.border.left - layout.border.right,
                node->getTransform().scale.y - layout.border.top - layout.border.bot};
            const bool isScrollHorizontal = subNode->getLayout().type == Layout::Type::HORIZONTAL;
            trScale.x = isScrollHorizontal ? (customScrollScale.x - sc.barScale.x) : sc.barScale.x;
            trScale.y = isScrollHorizontal ? sc.barScale.y : (customScrollScale.y - sc.barScale.y);
            continue;
        }

        if (isXFill && isLayoutHorizontal) { fillSubNodesCnt.x++; }
        if (isYFill && !isLayoutHorizontal) { fillSubNodesCnt.y++; }

        if (isXPx) { trScale.x = subNodeScale.x.value; }
        if (isYPx) { trScale.y = subNodeScale.y.value; }

        /* REL scale accomodated margins as well in its final size */
        if (isXRel) { trScale.x = subNodeScale.x.value * freeNodeScale.x - subNodeMargin.left - subNodeMargin.right; }
        if (isYRel) { trScale.y = subNodeScale.y.value * freeNodeScale.y - subNodeMargin.top - subNodeMargin.bot; }

        if (isXFit || isYFit)
        {
            const CustomLayoutEngine::Result<glm::vec2> fitScale = computeFitScale(subNode);
            if (!fitScale.error.empty()) { return Result<Void>{.error = fitScale.error}; }

            trScale.x = isXFit ? fitScale.value.x : trScale.x;
            trScale.y = isYFit ? fitScale.value.y : trScale.y;
        }

        if (!isXFill && isLayoutHorizontal) { nodeAvailableScale.x -= subNodeScale.x.value; }
        if (!isYFill && !isLayoutHorizontal) { nodeAvailableScale.y -= subNodeScale.y.value; }

        /* Margins of subnodes eat up available space for the FILL node scale type. */
        maxMargin = {
            std::max(maxMargin.x, subNodeMargin.left + subNodeMargin.right),
            std::max(maxMargin.y, subNodeMargin.top + subNodeMargin.bot),
        };
        nodeAvailableScale.x -= isLayoutHorizontal ? subNodeMargin.left + subNodeMargin.right : 0;
        nodeAvailableScale.y -= isLayoutHorizontal ? 0 : subNodeMargin.top + subNodeMargin.bot;

        // if (trScale.x <= 0 || trScale.y <= 0)
        // {
            // return Result<Void>{.error = std::format("Node '{}' computes to a zero/negative scale on one of it's axis",
            //     subNode->getName()) };
        // }
    }

    if (fillSubNodesCnt.x <= 0 && fillSubNodesCnt.y <= 0) { return Result<Void>{}; }

    /* For fill we need to store max margin and subtract it later depending on the layout type.
       FILL type accomodates margins in it's final size. */
    if (isLayoutHorizontal) { nodeAvailableScale.y -= maxMargin.y; }
    if (!isLayoutHorizontal) { nodeAvailableScale.x -= maxMargin.x; }

    fillSubNodesCnt.x = std::max(1, fillSubNodesCnt.x);
    fillSubNodesCnt.y = std::max(1, fillSubNodesCnt.y);

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

    const auto nodeScale = node->getLayout().newScale; /* TODO: newScale -> scale */
    glm::vec2 nonFitTotalScale{0, 0};
    glm::vec2 maximum{0, 0};
    for (const auto& subNode : subNodes)
    {
        SKIP_SCROLL_NODE(subNode);
        const auto& subNodeScale = subNode->getLayout().newScale;
        const auto& subNodeMargin = subNode->getLayout().margin;
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
            if (subNodeScale.x.type == Layout::ScaleType::PX)
            {
                scaleValue = subNodeScale.x.value + subNodeMargin.left + subNodeMargin.right;
            }
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
                return Result<glm::vec2>{
                    .error = std::format("Node '{}' is ScaleType::FIT on X and NOT FIT on the other axis with allow"
                        " wrap enabled. This is broken for now :(", node->getName(), subNode->getName())
                };
                // nonFitTotalScale.y += subNodeScale.y.value;
                // maximum.x = std::max(maximum.x, subNodeScale.x.value);
                // if (nonFitTotalScale.y > node->getTransform().scale.y) /* Node scale should be freeSpaceScale */
                // {
                //     totalScale.x += maximum.x;
                //     nonFitTotalScale.y = subNodeScale.y.value;
                //     maximum.x = 0;
                // }
            }
        }

        if (nodeScale.y.type == Layout::ScaleType::FIT)
        {
            float scaleValue = 0;
            if (subNodeScale.y.type == Layout::ScaleType::PX) { scaleValue = subNodeScale.y.value + subNodeMargin.top + subNodeMargin.bot; }
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
                return Result<glm::vec2>{
                    .error = std::format("Node '{}' is ScaleType::FIT on Y and NOT FIT on the other axis with allow"
                        " wrap enabled. This is broken for now :(", node->getName(), subNode->getName())
                };
                // nonFitTotalScale.x += subNodeScale.x.value;
                // // maximum.y += subNodeScale.y.value;
                // if (nonFitTotalScale.x > node->getTransform().scale.x)
                // {
                //     // totalScale.y += subNodeScale.y.value;
                //     totalScale.y = std::max((subNodeScale.y.value + maximum.y) - 0, totalScale.y);
                //     maximum.y = 0;
                //     nonFitTotalScale.x = subNodeScale.x.value;
                // }
                // maximum.y = std::max(maximum.y, subNodeScale.y.value); // keep max of line
            }
        }
    }

    /* Padding & Borders also need to be taken into consideration */
    totalScale.x += layout.padding.left + layout.padding.right + layout.border.left + layout.border.right;
    totalScale.y += layout.padding.top + layout.padding.bot + layout.border.top + layout.border.bot;

    return Result<glm::vec2>{.value = totalScale};
}

CustomLayoutEngine::Result<Void> CustomLayoutEngine::computeSubNodesPosition(const AbstractNodePtr& node,
    const ScrollContribution& sc)
{
    /*
        Note: Wrap doesn't work with FILL subnodes. It's best to use fixed size elements
    */
    glm::vec2 maximum{0, 0};
    uint32_t sliceStartIdx{0};
    uint32_t sliceEndIdx{0};
    auto& subNodes = node->getChildren();
    const auto& layout = node->getLayout();
    const auto& nodeTrPos = node->getTransform().pos;
    const glm::vec2 nodePosOffsets = computeNodeInnerStartOffsets(node);
    const auto& freeNodeScale = computeNodeFreeSpace(node, sc);
    const bool isWrapEnabled = layout.allowWrap;
    const bool isLayoutHorizontal = layout.type == Layout::Type::HORIZONTAL;

    glm::vec2 startPos{nodePosOffsets.x, nodePosOffsets.y};
    for (const auto& subNode : subNodes)
    {
        const auto& subNodeLayout = subNode->getLayout();
        const auto& subNodeMargin = subNodeLayout.margin;
        auto& trPos = subNode->getTransform().pos;
        auto& trScale = subNode->getTransform().scale;

        /* Put elements into position relative to the (0, 0) reference frame. */
        trPos = glm::vec3{startPos.x, startPos.y, trPos.z};

        trPos.x += subNodeMargin.left;
        trPos.y += subNodeMargin.top;

        /* Scrollbars are special Sliders which need to have their position calculated separately. */
        if (subNode->getType() == AbstractNode::NodeType::SCROLL)
        {
            /* Scrollbar will only be affected by border size, not by padding */
            const auto& rawNodeTrScale = node->getTransform().scale;
            const auto& rawNodeTrPos = node->getTransform().pos;

            const bool isScrollHorizontal = subNodeLayout.type == Layout::Type::HORIZONTAL;
            trPos.x = isScrollHorizontal
                ? rawNodeTrPos.x + layout.border.left
                : (rawNodeTrPos.x + rawNodeTrScale.x - sc.barScale.x - layout.border.right);
            trPos.y = isScrollHorizontal
                ? (rawNodeTrPos.y + rawNodeTrScale.y - sc.barScale.y - layout.border.bot)
                : rawNodeTrPos.y + layout.border.top;
            continue;
        }

        /* Deal with wrapping if needed */
        if (isWrapEnabled)
        {
            const bool layoutOverflowsX = startPos.x + trScale.x
                + subNodeLayout.margin.left + subNodeLayout.margin.right > freeNodeScale.x;
            const bool layoutOverflowsY = startPos.y + trScale.y
                + subNodeLayout.margin.top + subNodeLayout.margin.bot > freeNodeScale.y;
            if (isLayoutHorizontal && layoutOverflowsX)
            {
                selfAlignSubNodeSlice(node, maximum, sliceStartIdx, sliceEndIdx);
                sliceStartIdx += sliceEndIdx;

                startPos.x = nodePosOffsets.x;
                startPos.y += maximum.y;
                maximum.y = 0;
            }
            else if (!isLayoutHorizontal && layoutOverflowsY)
            {
                selfAlignSubNodeSlice(node, maximum, sliceStartIdx, sliceEndIdx);
                sliceStartIdx += sliceEndIdx;

                startPos.y = nodePosOffsets.y;
                startPos.x += maximum.x;
                maximum.x = 0;
            }
        }

        if (isLayoutHorizontal)
        {
            startPos.x += trScale.x + subNodeLayout.margin.left + subNodeLayout.margin.right;
            maximum.y = std::max(maximum.y, trScale.y + (subNodeLayout.margin.top + subNodeLayout.margin.bot));
        }
        else if (!isLayoutHorizontal)
        {
            startPos.y += trScale.y + subNodeLayout.margin.top + subNodeLayout.margin.bot;
            maximum.x = std::max(maximum.x, trScale.x + (subNodeLayout.margin.left + subNodeLayout.margin.right));
        }

        /* Change reference frame for each subnode to the node's frame. */
        trPos += glm::vec3{nodeTrPos.x, nodeTrPos.y, 0};

        sliceEndIdx++;
    }

    /* Align each subNode in relationship to the other nodes on the same row/column */
    selfAlignSubNodeSlice(node, maximum, sliceStartIdx, subNodes.size());

    /* Compute overflow as it is needed in order to satisfy node's alignSubNodes policy. */
    const glm::vec2 overflow = computeOverflow(node, sc);
    if (const auto& result = alignSubNodes(node, overflow); !result.error.empty())
    {
        return Result<Void>{.error = result.error};
    }

    /* Only Box and Box derived types support overflow handling. Like RecycleLists/TreeViews. */
    applyOverflowAndScrollOffsets(node, overflow, sc);

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
        SKIP_SCROLL_NODE(subNode);
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
        SKIP_SCROLL_NODE(subNodes[i]);
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

void CustomLayoutEngine::applyOverflowAndScrollOffsets(const AbstractNodePtr& node, const glm::vec2 overflow,
    const ScrollContribution& sc)
{
    if (node->getType() == AbstractNode::NodeType::BOX)
    {
        const auto& box = Utils::as<Box>(node);
        box->setOverflow(overflow);

        /* Apply scrollbar offsets */
        if (sc.offset.x >= 0 || sc.offset.y >= 0)
        {
            auto& subNodes = node->getChildren();
            for (auto& subNode : subNodes)
            {
                SKIP_SCROLL_NODE(subNode);
                subNode->getTransform().pos -= glm::vec3{sc.offset.x, sc.offset.y, 0};
            }
        }
    }
}

glm::vec2 CustomLayoutEngine::computeOverflow(const AbstractNodePtr& node, const ScrollContribution& sc)
{
    const auto& subNodes = node->getChildren();
    const auto nodeScale = computeNodeFreeSpace(node, sc);
    const glm::vec2 nodePos = glm::vec2{
        node->getTransform().pos.x, node->getTransform().pos.y } + computeNodeInnerStartOffsets(node);
    glm::vec2 maximumPoints{0, 0};
    for (const auto& subNode : subNodes)
    {
        SKIP_SCROLL_NODE(subNode);
        const glm::vec2 subNodeScale = computeNoteOccupiedSpace(subNode);
        glm::vec2 subNodePos = subNode->getTransform().pos;
        subNodePos.x -= subNode->getLayout().margin.left;
        subNodePos.y -= subNode->getLayout().margin.top;
        maximumPoints.x = std::max(maximumPoints.x, subNodePos.x + subNodeScale.x);
        maximumPoints.y = std::max(maximumPoints.y, subNodePos.y + subNodeScale.y);
    }

    /* Positive values in case of overflow, negative in case of underflow. */
    return {maximumPoints.x - (nodePos.x + nodeScale.x), maximumPoints.y - (nodePos.y + nodeScale.y)};
}

glm::vec2 CustomLayoutEngine::computeNodeFreeSpace(const AbstractNodePtr& node, const ScrollContribution& sc)
{
    /* Contains padding and borders are those are INSIDE the node, but it doesn't contain margins. */ 
    const auto& nodeTrScale = node->getTransform().scale;
    const auto& nodeLayout = node->getLayout();
    glm::vec2 nodeScale = glm::vec2{nodeTrScale.x, nodeTrScale.y};

    return {
        nodeTrScale.x - sc.barScale.x
            - nodeLayout.padding.left - nodeLayout.padding.right
            - nodeLayout.border.left - nodeLayout.border.right,
        nodeTrScale.y - sc.barScale.y 
            - nodeLayout.padding.top - nodeLayout.padding.bot
            - nodeLayout.border.top - nodeLayout.border.bot
    };
}

glm::vec2 CustomLayoutEngine::computeNodeInnerStartOffsets(const AbstractNodePtr& node)
{
    /* Compute from where we shall actually start to place subNodes taking into consideration the node's
       padding and borders. */
    const auto& nodeLayout = node->getLayout();

    return {
        nodeLayout.padding.left + nodeLayout.border.left,
        nodeLayout.padding.top + nodeLayout.border.top
    };
}

glm::vec2 CustomLayoutEngine::computeNoteOccupiedSpace(const AbstractNodePtr& node)
{
    /* This adds the margins to the computed scale */
    const auto& nodeLayout = node->getLayout();
    const auto& nodeTrScale = node->getTransform().scale;
    return {
        nodeTrScale.x + nodeLayout.margin.left + nodeLayout.margin.right,
        nodeTrScale.y + nodeLayout.margin.top + nodeLayout.margin.bot
    };
}

Result<CustomLayoutEngine::ScrollContribution> CustomLayoutEngine::computeScrollNodeContribution(
    const AbstractNodePtr& node)
{
    /* Compute by how much the parent should shrink and, at the end, by how much it should shift the layout. */
    ScrollContribution sc;
    if (node->getType() == AbstractNode::NodeType::BOX)
    {
        const auto& box = Utils::as<Box>(node);
        if (box->isScrollBarActive(Layout::Type::HORIZONTAL))
        {
            SliderPtr sl = box->getHBar().lock();
            const auto& slLayout = sl->getLayout();
            if (slLayout.newScale.x.type != Layout::ScaleType::REL || slLayout.newScale.x.value != 1.0f)
            {
                return Result<ScrollContribution>{
                    .error = std::format("Horizontal scroll subNode of {} is NOT ScaleType::REL of value 1 on X axis!",
                        node->getName())};
            }
            else if (slLayout.newScale.y.type != Layout::ScaleType::PX)
            {
                return Result<ScrollContribution>{
                    .error = std::format("Horizontal scroll subNode of {} is NOT ScaleType::PX on Y axis!",
                        node->getName())};
            }
            else if (slLayout.newScale.y.value <= 0)
            {
                return Result<ScrollContribution>{
                    .error = std::format("Horizontal scroll subNode of {} has a zero/negative scale value on Y axis!",
                        node->getName())};
            }

            sc.barScale.y = slLayout.newScale.y.value;
            sc.offset.x = sl->getSlideCurrentValue();
        }

        if (box->isScrollBarActive(Layout::Type::VERTICAL))
        {
            SliderPtr sl = box->getVBar().lock();
            const auto& slLayout = sl->getLayout();
            if (slLayout.newScale.y.type != Layout::ScaleType::REL || slLayout.newScale.y.value != 1.0f)
            {
                return Result<ScrollContribution>{
                    .error = std::format("Vertical scroll subNode of {} is NOT ScaleType::REL of value 1 on Y axis!",
                        node->getName())};
            }
            else if (slLayout.newScale.x.type != Layout::ScaleType::PX)
            {
                return Result<ScrollContribution>{
                    .error = std::format("Vertical scroll subNode of {} is NOT ScaleType::PX on X axis!",
                        node->getName())};
            }
            else if (slLayout.newScale.y.value <= 0)
            {
                return Result<ScrollContribution>{
                    .error = std::format("Horizontal scroll subNode of {} has a zero/negative scale value on Y axis!",
                        node->getName())};
            }

            sc.barScale.x = sl->getLayout().newScale.x.value; // should be scale of the slider in the opposite direction
            sc.offset.y = sl->getSlideCurrentValue();
        }
    }
    return Result<ScrollContribution>{.value = sc};
}

void CustomLayoutEngine::handlerSliderNode(const AbstractNodePtr& node)
{
    /* Scrollbars are just specialized Sliders and sliders will always be guaranteed to have
       a slider knob subNode.
    */

    const auto& nodePos = node->getTransform().pos;
    const auto& nodeScale = node->getTransform().scale; /* Here's ok to compute it manually. Account for node border/padding*/
    const bool isLayoutHorizontal = node->getLayout().type == Layout::Type::HORIZONTAL;
    const auto castSlider = Utils::as<Slider>(node);
    const bool dynamicKnob = castSlider->isDyanmicKnobEnabled();

    auto knob = castSlider->getKnob().lock();
    auto& knobPos = knob->getTransform().pos;
    auto& knobScale = knob->getTransform().scale;

    const float sliderPercOffset = castSlider->getOffsetPerc();
    const float sliderMaxValue = castSlider->getSlideTo();
    const float sliderCurrentValue = castSlider->getSlideCurrentValue();

    /* Setting knob scale and positioning */
    if (isLayoutHorizontal)
    {
        knobScale.x = dynamicKnob ? (nodeScale.x - sliderMaxValue) : nodeScale.y;
        knobScale.x = std::max(nodeScale.y, knobScale.x);
        knobScale.y = nodeScale.y;

        const float newX = Utils::remap(sliderPercOffset,
            0.0f, 1.0f, nodePos.x + knobScale.x / 2, nodePos.x + nodeScale.x - knobScale.x / 2);
        knobPos.y = nodePos.y;
        knobPos.x = newX - knobScale.x / 2;
    }
    else
    {
        knobScale.y = dynamicKnob ? (nodeScale.y - sliderMaxValue) : nodeScale.x;
        knobScale.y = std::max(nodeScale.x, knobScale.y);
        knobScale.x = nodeScale.x;

        /* In scrollbar mode values need to go from top to bottom, but in slider mode they need to
           go from bottom to top, so we need to invert the percentage. */
        const float remapVal = castSlider->getType() == AbstractNode::NodeType::SLIDER ? 
            1.0 - sliderPercOffset : sliderPercOffset;
        const float newY = Utils::remap(remapVal,
            0.0f, 1.0f, nodePos.y + knobScale.y / 2, nodePos.y + nodeScale.y - knobScale.y / 2);
        knobPos.x = nodePos.x;
        knobPos.y = newY - knobScale.y / 2;
    }
}

#undef SKIP_COMPOSED_DIRECTIONS
#undef SKIP_HORIZONTAL_DIRECTIONS
#undef SKIP_VERTICAL_DIRECTIONS
#undef SKIP_SCROLL_NODE
} // namespace msgui::layoutengine
#include "msgui/layoutEngine/CustomLayoutEngine.hpp"
#include "msgui/layoutEngine/utils/LayoutData.hpp"
#include "msgui/node/AbstractNode.hpp"
#include "msgui/node/Box.hpp"
#include "msgui/node/Slider.hpp"
#include "msgui/node/utils/SliderKnob.hpp"
#include <cmath>
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

#define RETURN_ON_ERROR(result, returnType)\
    if (!result.error.empty()) { return Result<returnType>{.error = result.error}; };\

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

/*
    This function aims to calculate position and scale of the subNodes of "node" and runs under the assumption that
    "node" position and scale has already been calculated.
    There are two types of routes it can take: calculate common nodes (such as boxes/buttons/etc) and uncommon nodes
    such as Sliders/Scrollbars/etc.

    Common nodes have full access to modifiers such as allowing overflow, wrapping, padding, borders, margins,
    alignment, subNodes alignment and subNodes spacing.

    Uncommon nodes have limited support for modifiers due to their functional nature (for now) and take a different,
    custom logic path stright from the beggining.

    There are 2 passes for common nodes: computeSubNodeScale and computeSubNodePos. Pretty self explanatory.

    There are 3 types of node layout types: HORIZONTAL, VERTICAL and GRID. H/V layout types are similar but flipped
    so they have the same logic but flipped. Grid layouts are more involved and take a different path.

    Output of these two functions consists in the subNodes being places and scaled correctly. Additionaly the function
    can return error messages that shall halt the layout calculation of that node.
*/
Result<Void> CustomLayoutEngine::process(const AbstractNodePtr& node)
{
    const AbstractNode::NodeType nodeType = node->getType();
    const Layout& layout = node->getLayout();
    const bool isLayoutHorizontal = layout.type == Layout::Type::HORIZONTAL;
    const bool isLayoutVertical = layout.type == Layout::Type::VERTICAL;
    const bool isLayoutGrid = layout.type == Layout::Type::GRID;

    /* Handling of uncommon node types */
    if (nodeType == AbstractNode::NodeType::SCROLL || nodeType == AbstractNode::NodeType::SLIDER)
    {
        handlerSliderNode(node);
        return Result<Void>{};
    }

    /*
        Box nodes or nodes derived from Box (such as RecycleList) can have scrollbars to scroll the content via
        user interaction.
        These bars use up "node" space and we need the actual available node space for later computations and
        that space needs to exclude space taken by active scrollbars.
        Additionaly we need to extract the current offset of the scrollbar (how much it is scrolled already)
        so that we can pass this information to position calculations later on.
    */
    Result<ScrollContribution> sc = computeScrollNodeContribution(node);
    RETURN_ON_ERROR(sc, Void);

    if (!sc.error.empty()) { return Result<Void>{.error = sc.error}; }

    /* Handling of common Horizontal and Verical layouts */
    if (isLayoutHorizontal || isLayoutVertical)
    {
        const Result<Void>& scaleResult = computeSubNodesScale(node, sc.value);
        RETURN_ON_ERROR(scaleResult, Void);

        const Result<Void>& posResult = computeSubNodesPosition(node, sc.value);
        RETURN_ON_ERROR(posResult, Void);
    }
    /* Handling of grid layouts (not yet) */
    else if (isLayoutGrid)
    {
        return Result<Void>{.error = "GRID layout not implemeted yet"};
    }
    else
    {
        return Result<Void>{.error = "Unsupported layout type"};
    }

    return Result<Void>{};
}

/*
    Function aims to compute the final scale of the subNodes of "node" so that the positioning pass has
    all it needs to its computation.

    Scaling can be of 4 types: PX, REL, FIT and FILL on each of the axis. This will determine the size of the node.
    PX  - Scale of the subNode expressed in plain pixel values (i.e 50_px)
    REL - Scale relative to the parent node scale expressed in (0, 1] percentage range (i.e 0.4_rel). The parent
          node scale is affected by: space taken by scroll bars, padding and borders (of the parent).
          Additionally, the final subNode scale will be also affected by the subNode's margins. So if any of those are
          active, the node scale used for REL computation will be smaller.
    FIT - A Fit subNode will try to "gift-wrap" around it's OWN subNodes such that they fit perfectly inside.
          The fit space necessary is increased by the padding and borders of the subNode itself and by any margins
          it's OWN subNodes.
          Constraints for FIT subNode subNodes on an arbitrary axis:
            - Cannot be of type FILL/REL as that requires subNode scale to be already know but it's not yet. We
              try to compute it in FIT.
            - FIT subNodes can be FIT on their own but the leaf nodes MUST be of type PX, otherwise it's impossible
              to find a size to fit around.
          A subNode can be REL on X axis and FIT on Y axis. This will scale de Y axis to FIT everything on that axis
          perfectly while leaving X axis to be dynamic in scale.
          If "wrapping" is enabled, FIT Y axis will adapt to accomodate nodes that will fall down from the upper
          row to a lower row to satisfy "wrap" policy. NOTE: **This is broken for now**
    FILL - Scale of the subNode will be an equal part (between it and other fill subNodes of parent) of what's left
           after the PX/REL/FIT nodes aquire their scale.
           A FILL subNode scale is affected (shrunk) by it's parent padding and border scale and also by it's
           own margins.

    Scroll subNode will also be calculated here as it is a subNode of a Box (in general) and calculating it here
    means one less loop to find them later on.
*/
Result<Void> CustomLayoutEngine::computeSubNodesScale(const AbstractNodePtr& node, const ScrollContribution& sc)
{
    const Layout& layout = node->getLayout();
    const bool isLayoutHorizontal = layout.type == Layout::Type::HORIZONTAL;
    const glm::vec2& nodeTrScale = node->getTransform().scale;
    const glm::vec2& usableNodeSpace = glm::vec2{
        nodeTrScale.x - sc.barScale.x
            - layout.padding.left - layout.padding.right
            - layout.border.left - layout.border.right,
        nodeTrScale.y - sc.barScale.y 
            - layout.padding.top - layout.padding.bot
            - layout.border.top - layout.border.bot
    };

    glm::vec2 fillAvailableScale{usableNodeSpace};
    glm::vec2 maxMarginOnAxis{0, 0};
    glm::ivec2 fillSubNodesCnt{0, 0};
    AbstractNodePVec& subNodes = node->getChildren();
    for (const AbstractNodePtr& subNode : subNodes)
    {
        const Layout& subNodeLayout = subNode->getLayout();
        const Layout::ScaleXY& subNodeScale = subNodeLayout.newScale;
        const Layout::TBLR& subNodeMargin = subNodeLayout.margin;
        const bool isXPx = subNodeScale.x.type == Layout::ScaleType::PX;
        const bool isYPx = subNodeScale.y.type == Layout::ScaleType::PX;
        const bool isXRel = subNodeScale.x.type == Layout::ScaleType::REL;
        const bool isYRel = subNodeScale.y.type == Layout::ScaleType::REL;
        const bool isXFit = subNodeScale.x.type == Layout::ScaleType::FIT;
        const bool isYFit = subNodeScale.y.type == Layout::ScaleType::FIT;
        const bool isXFill = subNodeScale.x.type == Layout::ScaleType::FILL;
        const bool isYFill = subNodeScale.y.type == Layout::ScaleType::FILL;
        glm::vec3& subNodeTrScale = subNode->getTransform().scale;

        /*
            Scrollbars are special Sliders which need to have their scale calculated separately.
            Their scale is influenced by the parent node's border only. Padding doesn't affect it.
        */
        if (subNode->getType() == AbstractNode::NodeType::SCROLL)
        {
            const glm::vec2& sbAdjustedScale = glm::vec2{
                nodeTrScale.x - layout.border.left - layout.border.right,
                nodeTrScale.y - layout.border.top - layout.border.bot
            };
            const bool isScrollHorizontal = subNodeLayout.type == Layout::Type::HORIZONTAL;
            subNodeTrScale.x = isScrollHorizontal ? (sbAdjustedScale.x - sc.barScale.x) : sc.barScale.x;
            subNodeTrScale.y = isScrollHorizontal ? sc.barScale.y : (sbAdjustedScale.y - sc.barScale.y);

            /* For now, there's nothing else to do in regards to scrollbar's scale. */
            continue;
        }

        /*  
            FILL subNodes are counted depending on the layout type as they need to be distributed later on based
            on that type.
        */
        if (isXFill && isLayoutHorizontal) { fillSubNodesCnt.x++; }
        if (isYFill && !isLayoutHorizontal) { fillSubNodesCnt.y++; }

        /* PX nodes get scaled "as is". */
        if (isXPx) { subNodeTrScale.x = subNodeScale.x.value; }
        if (isYPx) { subNodeTrScale.y = subNodeScale.y.value; }

        /* REL subNode scale needs to account for subNode's magins (subtract that). */
        if (isXRel)
        {
            subNodeTrScale.x = (subNodeScale.x.value * usableNodeSpace.x)
                - (subNodeMargin.left + subNodeMargin.right);
        }
        if (isYRel)
        {
            subNodeTrScale.y = (subNodeScale.y.value * usableNodeSpace.y)
                - (subNodeMargin.top + subNodeMargin.bot);
        }

        /*
            FIT subNodes need to be calculated only if one direction is FIT, otherwise it's pointless.
            Function tries to fit the subNodes of "subNode" so that subNode has the minimal required size
            to "gift wrap" the subNodes on the FIT axis.
            SubNode's padding and borders are taken into account to expand the required scale.
            SubNode's sub-node's margins are also taken into account to expand the required scale.
            The subnode's subnodes will not be calculated here (no assignment of computed values).
        */
        if (isXFit || isYFit)
        {
            const Result<glm::vec2>& fitScale = computeFitScale(subNode);
            RETURN_ON_ERROR(fitScale, Void);

            subNodeTrScale.x = isXFit ? fitScale.value.x : subNodeTrScale.x;
            subNodeTrScale.y = isYFit ? fitScale.value.y : subNodeTrScale.y;
        }

        /* Sometimes things don't fit quite perfectly to the pixel grid, so we need to do some rounding. */
        subNodeTrScale.x = std::round(subNodeTrScale.x);
        subNodeTrScale.y = std::round(subNodeTrScale.y);

        if (!isXFill && isLayoutHorizontal) { fillAvailableScale.x -= subNodeScale.x.value; }
        if (!isYFill && !isLayoutHorizontal) { fillAvailableScale.y -= subNodeScale.y.value; }

        /*
            Margins of subnodes eat up available space for the FILL subNodes.
            Depending on the layout type, we need to figour out the maximum margin on that axis as it's needed
            later on.
            While at it, the fill available scale will also shrink by each subNode's margin.
        */
        maxMarginOnAxis = {
            std::max(maxMarginOnAxis.x, subNodeMargin.left + subNodeMargin.right),
            std::max(maxMarginOnAxis.y, subNodeMargin.top + subNodeMargin.bot),
        };
        fillAvailableScale.x -= isLayoutHorizontal ? subNodeMargin.left + subNodeMargin.right : 0;
        fillAvailableScale.y -= isLayoutHorizontal ? 0 : subNodeMargin.top + subNodeMargin.bot;
    }

    /* Don't bother do to more FILL related suff if we don't have any. */
    if (fillSubNodesCnt.x <= 0 && fillSubNodesCnt.y <= 0) { return Result<Void>{}; }

    /* Depending on layout, subtract the maximum margins found earlier for each subNode. */
    if (isLayoutHorizontal) { fillAvailableScale.y -= maxMarginOnAxis.y; }
    if (!isLayoutHorizontal) { fillAvailableScale.x -= maxMarginOnAxis.x; }

    /* Cap FILL subNodes to a minimum of 1 to prevent divides by zero. */
    fillSubNodesCnt.x = std::max(1, fillSubNodesCnt.x);
    fillSubNodesCnt.y = std::max(1, fillSubNodesCnt.y);

    /* Each FILL subNode gets an equal part of the available space to scale in. */
    const glm::vec2 fillEqualPart{
        fillAvailableScale.x / fillSubNodesCnt.x,
        fillAvailableScale.y / fillSubNodesCnt.y
    };
    for (const AbstractNodePtr& subNode : subNodes)
    {
        const Layout::ScaleXY& subNodeScale = subNode->getLayout().newScale;
        const bool isXFill = subNodeScale.x.type == Layout::ScaleType::FILL;
        const bool isYFill = subNodeScale.y.type == Layout::ScaleType::FILL;

        if (!isXFill && !isYFill) { continue; }

        glm::vec3& trScale = subNode->getTransform().scale;
        if (isXFill) { trScale.x = fillEqualPart.x; }
        if (isYFill) { trScale.y = fillEqualPart.y; }

        /* Sometimes things don't fit quite perfectly to the pixel grid, so we need to do some rounding. */
        trScale.x = std::round(trScale.x);
        trScale.y = std::round(trScale.y);
    }

    return Result<Void>{};
}

/*
    Function will try to compute the scale required for "node" such that the subNodes of it fit perfectly inside.
    Node's padding/borders and all subNode's margins are taken into consideration for this calculation.
    If a subNode is itself of scale type FIT, then this function will recurse down on it until the end.
    Leaf subNodes are REQUIRED to be of scale type PX, otherwise it is impossible to compute the FIT scale of the
    initial node.
    This function WILL NOT set any scale for any node/subNode, it just tries to compute the minimum gift-wrapped scale.
*/
Result<glm::vec2> CustomLayoutEngine::computeFitScale(const AbstractNodePtr& node)
{
    const Layout& layout = node->getLayout();
    const bool isLayoutHorizontal = layout.type == Layout::Type::HORIZONTAL;
    AbstractNodePVec& subNodes = node->getChildren();
    glm::vec2 scaleNeeded{0, 0};

    /* Immediately fail if we cannot wrap around anything. */
    if (subNodes.empty())
    {
        return Result<glm::vec2>{
            .error = std::format("Node '{}' is ScaleType::FIT on one of the axis but it has no "
                 "subNodes to FIT around!", node->getName())};
    }

    const Layout::ScaleXY& nodeScale = node->getLayout().newScale;
    glm::vec2 nonFitTotalScale{0, 0};
    // glm::vec2 maximum{0, 0}; /* Unusued for now. Will be used for FIT+REL+Wrap feature. */
    for (const AbstractNodePtr& subNode : subNodes)
    {
        SKIP_SCROLL_NODE(subNode);
        const Layout::ScaleXY& subNodeScale = subNode->getLayout().newScale;
        const Layout::TBLR& subNodeMargin = subNode->getLayout().margin;
        const bool isXFit = subNodeScale.x.type == Layout::ScaleType::FIT;
        const bool isYFit = subNodeScale.y.type == Layout::ScaleType::FIT;

        /* If any of our subNodes are fit themselves in any axis, we need to recurse down on them. */
        glm::vec2 subNodeFitScale{0, 0};
        if (isXFit || isYFit)
        {
            Result<glm::vec2> result = computeFitScale(subNode);
            RETURN_ON_ERROR(result, glm::vec2);

            subNodeFitScale = result.value;
        }

        /* Compute the scale needed to FIT subNodes on the X axis. */
        if (nodeScale.x.type == Layout::ScaleType::FIT)
        {
            float scaleValue = 0;
            if (subNodeScale.x.type == Layout::ScaleType::PX)
            {
                scaleValue = subNodeScale.x.value + subNodeMargin.left + subNodeMargin.right;
            }
            else if (subNodeScale.x.type == Layout::ScaleType::FIT)
            {
                scaleValue = subNodeFitScale.x;
            }
            else
            {
                return Result<glm::vec2>{
                    .error = std::format("Node '{}' is ScaleType::FIT on X axis but subNode '{}' is "
                        "NOT ScaleType::FIT or ScaleType::PX on that axis", node->getName(), subNode->getName())};
            }

            scaleNeeded.x = !isLayoutHorizontal
                ? std::max(scaleNeeded.x, scaleValue)
                : scaleNeeded.x + scaleValue;

            /* Unused feature for now. */
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

        /* Compute the scale needed to FIT subNodes on the X axis. */
        if (nodeScale.y.type == Layout::ScaleType::FIT)
        {
            float scaleValue = 0;
            if (subNodeScale.y.type == Layout::ScaleType::PX)
            {
                scaleValue = subNodeScale.y.value + subNodeMargin.top + subNodeMargin.bot;
            }
            else if (subNodeScale.y.type == Layout::ScaleType::FIT)
            {
                scaleValue = subNodeFitScale.y;
            }
            else
            {
                return Result<glm::vec2>{
                    .error = std::format("Node '{}' is ScaleType::FIT on Y axis but subNode '{}' is "
                        "NOT ScaleType::FIT or ScaleType::PX on that axis", node->getName(), subNode->getName())};
            }

            scaleNeeded.y = isLayoutHorizontal
                ? std::max(scaleNeeded.y, scaleValue)
                : scaleNeeded.y + scaleValue;

            /* If Y scaleType is FIT but X scaleType is not and wrap is enabled, it can happen for some
               subnodes to jump to the next row and in that case we need to increase the FIT computed scale
               to account for that Y increase. We take the maximum Y value that can best fit the elements drop. */
            /* Unused feature for now*/
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

    /* Padding & Borders of the node need also to be taken into consideration. */
    scaleNeeded.x += layout.padding.left + layout.padding.right + layout.border.left + layout.border.right;
    scaleNeeded.y += layout.padding.top + layout.padding.bot + layout.border.top + layout.border.bot;

    return Result<glm::vec2>{ .value = scaleNeeded };
}

/*
    Function will compute the final position of the node after the scaling pass.
    The node's subNodes will be arranged from top to bottom or from left to right depening on the layout type.
    Positioning will start taking into consideration node's padding and border size. Subnode's margins will
    also contribue to the positioning of itself and of the next nodes.
    The subNodes can be aligned, as a whole group, relative to the node's unused space as follows:
        TOP_LEFT, TOP_RIGHT, CENTER_LEFT, CENTER_RIGHT, BOTTOM_LEFT, BOTTOM_RIGHT.
    The subNodes can also be self aligned relative to the other nodes on the same row/column as follows:
        1. For Horizontal type layout: TOP, CENTER, BOTTOM
        2. For Vertical type layout: LEFT, CENTER, RIGHT
    SubNodes can also wrap to the next row/column if allowed creating multiple rows/columns each obeying the
    set self alignment of those subNodes.

    Scrollbars will also be positioned at this stage as they are subNodes of Box derived nodes.
*/
Result<Void> CustomLayoutEngine::computeSubNodesPosition(const AbstractNodePtr& node,
    const ScrollContribution& sc)
{
    const Layout& layout = node->getLayout();
    const bool isLayoutHorizontal = layout.type == Layout::Type::HORIZONTAL;
    const glm::vec3& nodeTrPos = node->getTransform().pos;
    const glm::vec3& nodeTrScale = node->getTransform().scale;
    const glm::vec2& nodePosOffsets = glm::vec2{
        layout.padding.left + layout.border.left,
        layout.padding.top + layout.border.top
    };
    const glm::vec2& usableNodeScale = glm::vec2{
        nodeTrScale.x - sc.barScale.x
            - layout.padding.left - layout.padding.right
            - layout.border.left - layout.border.right,
        nodeTrScale.y - sc.barScale.y 
            - layout.padding.top - layout.padding.bot
            - layout.border.top - layout.border.bot
    };

    glm::vec2 maximumOnAxis{0, 0};
    glm::vec2 startPos{nodePosOffsets.x, nodePosOffsets.y};
    uint32_t sliceStartIdx{0}; /* Indexes needed to impose subNodes self alignment on each row/column independently. */
    uint32_t sliceEndIdx{0};
    AbstractNodePVec& subNodes = node->getChildren();
    for (const AbstractNodePtr& subNode : subNodes)
    {
        const Layout& subNodeLayout = subNode->getLayout();
        const Layout::TBLR& subNodeMargin = subNodeLayout.margin;
        glm::vec3& trPos = subNode->getTransform().pos;
        glm::vec3& trScale = subNode->getTransform().scale;

        /*
            Scrollbars are special Sliders which need to have their position calculated separately.
            The position is affected by the node's borders only.
        */
        if (subNode->getType() == AbstractNode::NodeType::SCROLL)
        {
            const bool isScrollHorizontal = subNodeLayout.type == Layout::Type::HORIZONTAL;
            trPos.x = isScrollHorizontal
                ? nodeTrPos.x + layout.border.left
                : (nodeTrPos.x + nodeTrScale.x - sc.barScale.x - layout.border.right);
            trPos.y = isScrollHorizontal
                ? (nodeTrPos.y + nodeTrScale.y - sc.barScale.y - layout.border.bot)
                : nodeTrPos.y + layout.border.top;
            
            /* For now nothing more to be done for scroll node position. */
            continue;
        }

        /* Deal with wrapping if enabled. If wrapping isn't needed at all, this block can be safely removed. */
        if (layout.allowWrap)
        {
            /*
                Subnode margins and node's borders+paddings are taken into consideration to determine if the
                subNode will jump to the next row/column.
                How much they shall jump is calculated depening on the maximum scale on the currently under
                computation row/column.
            */
            const bool layoutOverflowsX = startPos.x + subNodeMargin.left + trScale.x
                + subNodeLayout.margin.left + subNodeLayout.margin.right > usableNodeScale.x;
            const bool layoutOverflowsY = startPos.y + subNodeMargin.top + trScale.y
                + subNodeLayout.margin.top + subNodeLayout.margin.bot > usableNodeScale.y;
            if (isLayoutHorizontal && layoutOverflowsX)
            {
                selfAlignSubNodeSlice(node, maximumOnAxis, sliceStartIdx, sliceEndIdx);
                sliceStartIdx += sliceEndIdx;

                startPos.x = nodePosOffsets.x;
                startPos.y += maximumOnAxis.y;
                maximumOnAxis.y = 0;
            }
            else if (!isLayoutHorizontal && layoutOverflowsY)
            {
                selfAlignSubNodeSlice(node, maximumOnAxis, sliceStartIdx, sliceEndIdx);
                sliceStartIdx += sliceEndIdx;

                startPos.y = nodePosOffsets.y;
                startPos.x += maximumOnAxis.x;
                maximumOnAxis.x = 0;
            }
        }

        /* 
            Push nodes into position relative to the parent's padding+border + subNode's margins but
            NOT relative to the node's position just yey.
        */
        trPos = glm::vec3{startPos.x + subNodeMargin.left, startPos.y + subNodeMargin.top, trPos.z};

        /* Compute the start position for the next subNode & update maximum on axis if needed. */
        const float lrMargin = subNodeLayout.margin.left + subNodeLayout.margin.right;
        const float tbMargin = subNodeLayout.margin.top + subNodeLayout.margin.bot;
        if (isLayoutHorizontal)
        {
            startPos.x += trScale.x + lrMargin;
            maximumOnAxis.y = std::max(maximumOnAxis.y, trScale.y + tbMargin);
        }
        else if (!isLayoutHorizontal)
        {
            startPos.y += trScale.y + tbMargin;
            maximumOnAxis.x = std::max(maximumOnAxis.x, trScale.x + lrMargin);
        }

        /*
            Change reference frame for each subNode to the node's frame aka subNodes are now relative
            to the parent's position.
        */
        trPos += glm::vec3{nodeTrPos.x, nodeTrPos.y, 0};

        sliceEndIdx++;
    }

    /* Align each subNode in relationship to the other nodes on the same row/column. */
    selfAlignSubNodeSlice(node, maximumOnAxis, sliceStartIdx, subNodes.size());

    /* Compute overflow as it is needed in order to satisfy node's alignSubNodes policy. */
    const glm::vec2 overflow = computeOverflow(node, sc);
    RETURN_ON_ERROR(alignSubNodes(node, overflow), Void);

    /* 
        Apply any overflow shifting as needed. Only Box and Box derived types support overflow handling.
        Like RecycleLists/TreeViews.
    */
    applyOverflowAndScrollOffsets(node, overflow, sc);

    return Result<Void>{};
}

/*
    Function will align the subNodes, as a whole group, in the node's specified area which can be:
        TOP_LEFT, TOP_RIGHT, CENTER_LEFT, CENTER_RIGHT, BOTTOM_LEFT, BOTTOM_RIGHT.
    This will occur only if the computed overflow value is NEGATIVE as negative values mean no layout
    overflow and thus free space for positioning subNodes.
*/
Result<Void> CustomLayoutEngine::alignSubNodes(const AbstractNodePtr& node, const glm::vec2 computedOverflow)
{
    glm::vec2 positionToAdd{0, 0};
    const Layout::AlignXY alignValue = node->getLayout().alignChild;
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

    AbstractNodePVec& subNodes = node->getChildren();
    for (AbstractNodePtr& subNode : subNodes)
    {
        SKIP_SCROLL_NODE(subNode);
        glm::vec3& subNodePos = subNode->getTransform().pos;
        subNodePos.x += positionToAdd.x;
        subNodePos.y += positionToAdd.y;
    }

    return Result<Void>{};
}

/*
    Function will align each subNode of node independently as specified by selfAlign, per row/column
    starting from start index to end index (non-inclusive) by making use of the max scale on that row/column.
    Possible self alignment options are:
        1. For Horizontal type layout: TOP, CENTER, BOTTOM
        2. For Vertical type layout: LEFT, CENTER, RIGHT
*/
Result<Void> CustomLayoutEngine::selfAlignSubNodeSlice(const AbstractNodePtr& node, const glm::vec2 maximum,
    const uint32_t startIdx, const uint32_t endIdx)
{
    const bool isLayoutHorizontal = node->getLayout().type == Layout::Type::HORIZONTAL;
    AbstractNodePVec& subNodes = node->getChildren();
    for (uint32_t i = startIdx; i < endIdx; ++i)
    {
        SKIP_SCROLL_NODE(subNodes[i]);
        const glm::vec3& subNodeScale = subNodes[i]->getTransform().scale;
        const Layout& subNodeLayout = subNodes[i]->getLayout();
        glm::vec3& subNodePos = subNodes[i]->getTransform().pos;

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

        /* Your rounding may vary.. */
        subNodePos.x = std::round(subNodePos.x);
        subNodePos.y = std::round(subNodePos.y);
    }

    return Result<Void>{};
}

/*
    Function simply shifts all the subNode positions by overflow amount in order to simulate scrolling
    and additionally updates the internal node overflow value. This is effective only for Box nodes or derived of it.
*/
void CustomLayoutEngine::applyOverflowAndScrollOffsets(const AbstractNodePtr& node, const glm::vec2 overflow,
    const ScrollContribution& sc)
{
    if (node->getType() != AbstractNode::NodeType::BOX)
    {
        return;
    }

    const BoxPtr& box = Utils::as<Box>(node);
    box->setOverflow(overflow);

    if (sc.offset.x < 0 && sc.offset.y < 0)
    {
        return;
    }

    /* Apply scrollbar offsets */
    AbstractNodePVec& subNodes = node->getChildren();
    for (AbstractNodePtr& subNode : subNodes)
    {
        SKIP_SCROLL_NODE(subNode);
        subNode->getTransform().pos -= glm::vec3{sc.offset.x, sc.offset.y, 0};
    }
}

/*
    Function calculates the amount of overflow, in pixels, that each axis will generate. This logic
    assumes that the subNodes are already positioned properly relative to the parent node.
    Claculation takes into consideration active scrollbar sizes, padding and borders of the node as well
    as margins of the subNodes.
*/
glm::vec2 CustomLayoutEngine::computeOverflow(const AbstractNodePtr& node, const ScrollContribution& sc)
{
    const AbstractNodePVec& subNodes = node->getChildren();
    const glm::vec2& nodeTrScale = node->getTransform().scale;
    const Layout& layout = node->getLayout();
    const glm::vec2& nodeScale = glm::vec2{
        nodeTrScale.x - sc.barScale.x
            - layout.padding.left - layout.padding.right
            - layout.border.left - layout.border.right,
        nodeTrScale.y - sc.barScale.y 
            - layout.padding.top - layout.padding.bot
            - layout.border.top - layout.border.bot
    };
    const glm::vec2& nodePos = glm::vec2{
        node->getTransform().pos.x + layout.padding.left + layout.border.left,
        node->getTransform().pos.y + layout.padding.top + layout.border.top
    };

    glm::vec2 maximumPoints{0, 0};
    for (const AbstractNodePtr& subNode : subNodes)
    {
        SKIP_SCROLL_NODE(subNode);
        const Layout::TBLR& subNodeMargin = subNode->getLayout().margin;
        const glm::vec2& subNodeScale = {
            subNode->getTransform().scale.x + subNodeMargin.left + subNodeMargin.right,
            subNode->getTransform().scale.y + subNodeMargin.top + subNodeMargin.bot};
        glm::vec2 subNodePos = subNode->getTransform().pos;
        subNodePos.x -= subNode->getLayout().margin.left;
        subNodePos.y -= subNode->getLayout().margin.top;
        maximumPoints.x = std::max(maximumPoints.x, subNodePos.x + subNodeScale.x);
        maximumPoints.y = std::max(maximumPoints.y, subNodePos.y + subNodeScale.y);
    }

    /* Positive values in case of overflow, negative in case of underflow. */
    return {maximumPoints.x - (nodePos.x + nodeScale.x), maximumPoints.y - (nodePos.y + nodeScale.y)};
}

/*
    Function calculates two things:
        1. The size of the scrollbars in each axis direction
        2. The offset each scrollbar is currently at
    We all all this the scroll contribution.
*/
Result<CustomLayoutEngine::ScrollContribution> CustomLayoutEngine::computeScrollNodeContribution(
    const AbstractNodePtr& node)
{
    ScrollContribution sc;
    if (node->getType() != AbstractNode::NodeType::BOX)
    {
        return Result<ScrollContribution>{.value = sc};
    }

    const BoxPtr& box = Utils::as<Box>(node);
    if (box->isScrollBarActive(Layout::Type::HORIZONTAL))
    {
        SliderPtr sl = box->getHBar().lock();
        const Layout& slLayout = sl->getLayout();
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
        const Layout& slLayout = sl->getLayout();
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

        sc.barScale.x = sl->getLayout().newScale.x.value;
        sc.offset.y = sl->getSlideCurrentValue();
    }

    return Result<ScrollContribution>{.value = sc};
}

/*
    Function will calculate the slider's knob position as well as scale. The knob scale can be dynamic aka
    it will update depending on the slider's size and slide-to value.
*/
void CustomLayoutEngine::handlerSliderNode(const AbstractNodePtr& node)
{
    const SliderPtr& castSlider = Utils::as<Slider>(node);
    const glm::vec2& nodePos = node->getTransform().pos;
    const glm::vec2& nodeScale = node->getTransform().scale;
    const bool isLayoutHorizontal = node->getLayout().type == Layout::Type::HORIZONTAL;
    const bool dynamicKnob = castSlider->isDyanmicKnobEnabled();
    const float sliderPercOffset = castSlider->getOffsetPerc();
    const float sliderMaxValue = castSlider->getSlideTo();
    const float sliderCurrentValue = castSlider->getSlideCurrentValue();
    auto knob = castSlider->getKnob().lock();
    auto& knobPos = knob->getTransform().pos;
    auto& knobScale = knob->getTransform().scale;

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
#undef RETURN_ON_ERROR
} // namespace msgui::layoutengine
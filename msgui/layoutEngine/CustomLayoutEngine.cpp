#include "msgui/layoutEngine/CustomLayoutEngine.hpp"
#include "msgui/layoutEngine/utils/LayoutData.hpp"
#include "msgui/node/AbstractNode.hpp"
#include "msgui/node/Box.hpp"
#include "msgui/node/FloatingBox.hpp"
#include "msgui/node/RecycleList.hpp"
#include "msgui/node/Slider.hpp"
#include "msgui/node/Dropdown.hpp"
#include "msgui/node/TreeView.hpp"
#include "msgui/node/utils/BoxDividerSep.hpp"
#include "msgui/node/utils/SliderKnob.hpp"

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

#define SKIP_DROPDOWN_CONTAINER_NODE(node)\
    if (node->getType() == AbstractNode::NodeType::DROPDOWN_CONTAINTER) { continue; }\

#define SKIP_FLOATING_BOX_NODE(node)\
    if (node->getType() == AbstractNode::NodeType::FLOATING_BOX) { continue; }\

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
        handleSliderNode(node);
        return Result<Void>{};
    }

    if (nodeType == AbstractNode::NodeType::BOX_DIVIDER)
    {
        RETURN_ON_ERROR(handleBoxDividerNode(node), Void);
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

        const Result<glm::vec2>& posResult = computeSubNodesPosition(node, sc.value);
        RETURN_ON_ERROR(posResult, Void);
    }
    /* Handling of grid layouts */
    else if (isLayoutGrid)
    {
        const Result<Void> gridResult = computeGridLayout(node);
        RETURN_ON_ERROR(gridResult, Void);
    }
    else
    {
        return Result<Void>{.error = "Unsupported layout type"};
    }

    /* Handling of uncommon node types AFTER resolving their common subNodes */
    if (node->getType() == AbstractNode::NodeType::DROPDOWN)
    {
        RETURN_ON_ERROR(handleDropdown(node), Void);
        return Result<Void>{};
    }

    if (node->getType() == AbstractNode::NodeType::FLOATING_BOX)
    {
        RETURN_ON_ERROR(handleFloatingBox(node), Void);
        return Result<Void>{};
    }

    if (node->getType() == AbstractNode::NodeType::RECYCLE_LIST)
    {
        RETURN_ON_ERROR(handleRecycleList(node), Void);
        return Result<Void>{};
    }

    if (node->getType() == AbstractNode::NodeType::TREEVIEW)
    {
        RETURN_ON_ERROR(handleTreeView(node), Void);
        return Result<Void>{};
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

    Capping of subNode scale is done via setting a min/max value for each axis. This value DOES NOT take into account
    the subNode's margins. Scroll nodes are unnaffected by those values as well as FIT/FILL scale types.


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
    glm::vec2 totalInt{0, 0};
    glm::vec2 totalFloat{0, 0};
    AbstractNodePVec& subNodes = node->getChildren();
    for (const AbstractNodePtr& subNode : subNodes)
    {
        SKIP_DROPDOWN_CONTAINER_NODE(subNode);
        const Layout& subNodeLayout = subNode->getLayout();
        const Layout::ScaleXY& subNodeScale = subNodeLayout.newScale;
        const Layout::TBLR& subNodeMargin = subNodeLayout.margin;
        const glm::vec2& subNodeMin = subNodeLayout.minScale;
        const glm::vec2& subNodeMax = subNodeLayout.maxScale;
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
        if (isXPx) { subNodeTrScale.x = std::clamp(subNodeScale.x.value, subNodeMin.x, subNodeMax.x); }
        if (isYPx) { subNodeTrScale.y = std::clamp(subNodeScale.y.value, subNodeMin.y, subNodeMax.y); }

        /* REL subNode scale needs to account for subNode's magins (subtract that). */
        if (isXRel)
        {
            subNodeTrScale.x = (subNodeScale.x.value * usableNodeSpace.x)
                - (subNodeMargin.left + subNodeMargin.right);
            subNodeTrScale.x = std::clamp(subNodeTrScale.x, subNodeMin.x, subNodeMax.x);
            totalFloat.x += subNodeTrScale.x;
            subNodeTrScale.x = std::round(subNodeTrScale.x);
            totalInt.x += subNodeTrScale.x;
        }
        if (isYRel)
        {
            subNodeTrScale.y = (subNodeScale.y.value * usableNodeSpace.y)
                - (subNodeMargin.top + subNodeMargin.bot);
            subNodeTrScale.y = std::clamp(subNodeTrScale.y, subNodeMin.y, subNodeMax.y);
            totalFloat.y += subNodeTrScale.y;
            subNodeTrScale.y = std::round(subNodeTrScale.y);
            totalInt.y += subNodeTrScale.y;
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

    /* Sometimes things don't fit quite perfectly to the pixel grid, so we need to do some rounding. */
    resolveCumulativeError(node, totalInt, totalFloat);

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
        // trScale.x = std::round(trScale.x);
        // trScale.y = std::round(trScale.y);
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
        SKIP_DROPDOWN_CONTAINER_NODE(subNode);
        const Layout& subNodeLayout = subNode->getLayout();
        const Layout::ScaleXY& subNodeScale = subNodeLayout.newScale;
        const Layout::TBLR& subNodeMargin = subNodeLayout.margin;
        const glm::vec2& subNodeMinScale = subNodeLayout.minScale;
        const glm::vec2& subNodeMaxScale = subNodeLayout.maxScale;
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
                scaleValue = std::clamp(subNodeScale.x.value, subNodeMinScale.x, subNodeMaxScale.x)
                    + subNodeMargin.left + subNodeMargin.right;
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
                scaleValue = std::clamp(subNodeScale.y.value, subNodeMinScale.y, subNodeMaxScale.y)
                 + subNodeMargin.top + subNodeMargin.bot;
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
Result<glm::vec2> CustomLayoutEngine::computeSubNodesPosition(const AbstractNodePtr& node,
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
        SKIP_DROPDOWN_CONTAINER_NODE(subNode);
        SKIP_FLOATING_BOX_NODE(subNode);

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

        /* Apply the shrink effect. This behaves like a "post processing" effect to emphasise clicking. */
        trPos.x += subNodeLayout.shrink.x;
        trPos.y += subNodeLayout.shrink.y;
        trScale.x -= subNodeLayout.shrink.x * 2;
        trScale.y -= subNodeLayout.shrink.y * 2;

        sliceEndIdx++;
    }

    /* Align each subNode in relationship to the other nodes on the same row/column. */
    selfAlignSubNodeSlice(node, maximumOnAxis, sliceStartIdx, subNodes.size());

    /* Compute overflow as it is needed in order to satisfy node's alignSubNodes policy. */
    const glm::vec2 overflow = computeOverflow(node, sc);
    RETURN_ON_ERROR(alignSubNodes(node, overflow), glm::vec2);

    /* 
        Apply any overflow shifting as needed. Only Box and Box derived types support overflow handling.
        Like RecycleLists/TreeViews.
    */
    applyOverflowAndScrollOffsets(node, overflow, sc);

    return Result<glm::vec2>{.value = overflow};
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
        SKIP_FLOATING_BOX_NODE(subNode);
        SKIP_DROPDOWN_CONTAINER_NODE(subNode);
        SKIP_SCROLL_NODE(subNode);
        glm::vec3& subNodePos = subNode->getTransform().pos;
        subNodePos.x += positionToAdd.x;
        subNodePos.y += positionToAdd.y;

        // subNodePos.x = std::round(subNodePos.x);
        // subNodePos.y = std::round(subNodePos.y);
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
        SKIP_FLOATING_BOX_NODE(subNodes[i]);
        SKIP_DROPDOWN_CONTAINER_NODE(subNodes[i]);
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
        // subNodePos.x = std::round(subNodePos.x);
        // subNodePos.y = std::round(subNodePos.y);
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
        SKIP_FLOATING_BOX_NODE(subNode);
        SKIP_DROPDOWN_CONTAINER_NODE(subNode);
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
        SKIP_FLOATING_BOX_NODE(subNode);
        SKIP_DROPDOWN_CONTAINER_NODE(subNode);
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
    Function calculates the scale and positions of each subNode in the grid. Positions are taken
    from the indices supplied inside the subNode layout. SubNodes can span multiple grid cells.
    If the node is scaleType FIT, this fact will be ignored as subNodes scale and positioning inside
    the grid have priority. The node itself can only be REL/PX or FILL.
    Grid distribution defines the maximum size of each cell. It can be defined in PX or FR scaleType as:
    PX - Grid cell will span exactly "px" pixels.
    FR - Grid cell will span exatly "fr" fractional parts. Fractional parts are equal in size and are computed
         after subtracting all the PX grid cell sizes and node border+padding.
    Example for column axis:
        2_fr, 100_px, 1_fr
    Second cell will occupy exactly 100PX on the X axis.
    Total fractional parts of the column axis will be: 2_fr + 1_fr = 3_fr.
    First cell will get 2 parts of the remaining space after subtracting the pixel occupied cells while
    the Third cell will get only 1 part.
    Simply put the first cell will always be 2 times bigger than the third cell while the second cell remaing
    constant in size.
    Fractional part calculations are affected by the node's border and padding values.
    Subnodes can be self aligned inside the cell(s) at the following positions:
        TOP_LEFT, TOP_RIGHT, CENTER_LEFT, CENTER_RIGHT, BOTTOM_LEFT, BOTTOM_RIGHT.
    Subnodes can only be of scaleType PX, REL or FILL:
    PX   - The scale of the node is exactly the "px" value.
    REL  - Subnode is scaled relative to the size of the grid cell it is in (or cells if span is > 1) and
           takes into consideration subNode's margin.
    FILL - Subnode will fill the entire grid cell taking into consideration subnode's margin.
    Grid cells CANNOT produce overflow and if one subNode is bigger than the scale of it's cell, that element
    will leak into neighbouring cells. It is recommended to use REL or FILL scale types insidead of PX.
*/
Result<Void> CustomLayoutEngine::computeGridLayout(const AbstractNodePtr& node)
{
    /* !!!!!!!!!This part shall be done only if the grid data is dirty, not each time */
    /* Compute how much a FRAC part equates to. */
    RETURN_ON_ERROR(updateGridFracPart(node), Void);

    /* Position and scale the subNodes. */
    const Layout& nodeLayout = node->getLayout();
    const Layout::TBLR& nodeBorder = nodeLayout.border;
    const Layout::TBLR& nodePadding = nodeLayout.padding;
    const glm::vec3& nodeTrScale = node->getTransform().scale;
    const glm::vec2 nodeScale = {
        nodeTrScale.x
            - nodeBorder.right
            - nodePadding.right,
        nodeTrScale.y
            - nodeBorder.bot
            - nodePadding.bot,
    };

    const Layout::GridDistribRC& gridDistribRC = node->getLayout().gridDistribRC;
    const int32_t rowCnt = gridDistribRC.rows.size();
    const int32_t colCnt = gridDistribRC.cols.size();
    if (rowCnt <= 0 || colCnt <= 0)
    {
        return Result<Void>{.error = "Grid layout need to have at least one row and column defined!"};
    }

    AbstractNodePVec& subNodes = node->getChildren();
    for (AbstractNodePtr& subNode : subNodes)
    {
        SKIP_FLOATING_BOX_NODE(subNode);
        SKIP_DROPDOWN_CONTAINER_NODE(subNode);
        SKIP_SCROLL_NODE(subNode);

        const Layout& subNodeLayout = subNode->getLayout();
        const Layout::TBLR& subNodeMargin = subNodeLayout.margin;
        const Layout::GridRC& subNodeGridPos = subNodeLayout.gridPosRC;
        // const Layout::GridRC& subNodeGridSpan = subNodeLayout.gridSpanRC;
        glm::vec3& subNodeTrPos = subNode->getTransform().pos;
        glm::vec3& subNodeTrScale = subNode->getTransform().scale;

        /*
            Find node positioning first by placing each subNode at the previously computed start for each
            row and column in the distribution.
        */
        if (subNodeGridPos.col < 0 || subNodeGridPos.col >= colCnt)
        {
            return Result<Void>{.error = std::format("SubNode '{}' has column position out of bounds!"
                " Min 0 Max {} Actual {}", subNode->getName(), colCnt - 1, subNodeGridPos.col) };
        }
        else if (subNodeGridPos.row < 0 || subNodeGridPos.row >= rowCnt)
        {
            return Result<Void>{.error = std::format("SubNode '{}' has row position out of bounds!"
                " Min 0 Max {} Actual {}", subNode->getName(), rowCnt - 1, subNodeGridPos.row) };
        }

        const Layout::ScaleXY& subNodeScale = subNodeLayout.newScale;
        glm::vec2 gridPosStart{
            gridDistribRC.cols[subNodeGridPos.col].computedStart,
            gridDistribRC.rows[subNodeGridPos.row].computedStart
        };
        glm::vec2 gridPosEnd{
            subNodeGridPos.col + 1 < colCnt ? gridDistribRC.cols[subNodeGridPos.col + 1].computedStart : nodeScale.x,
            subNodeGridPos.row + 1 < rowCnt ? gridDistribRC.rows[subNodeGridPos.row + 1].computedStart : nodeScale.y,
        };

        /* Adjust for subNode margins. */
        gridPosStart.x += subNodeMargin.left;
        gridPosStart.y += subNodeMargin.top;
        gridPosEnd.x -= subNodeMargin.right;
        gridPosEnd.y -= subNodeMargin.bot;

        const glm::vec2 cellSize{gridPosEnd - gridPosStart};

        /* Scale the subNodes according to their scaleType. */
        if (subNodeScale.x.type == Layout::ScaleType::PX)
        {
            subNodeTrScale.x = subNodeScale.x.value;
        }
        else if (subNodeScale.x.type == Layout::ScaleType::REL)
        {
            subNodeTrScale.x = subNodeScale.x.value * cellSize.x;
        }
        else if (subNodeScale.x.type == Layout::ScaleType::FILL)
        {
            subNodeTrScale.x = cellSize.x;
        }
        else
        {
            return Result<Void>{.error = std::format("Grid only supports PX/REL or FILL scale type but"
                " subNode '{}' x axis is not one of those!", subNode->getName())};
        }

        if (subNodeScale.y.type == Layout::ScaleType::PX)
        {
            subNodeTrScale.y = subNodeScale.y.value;
        }
        else if (subNodeScale.y.type == Layout::ScaleType::REL)
        {
            subNodeTrScale.y = subNodeScale.y.value * cellSize.y;
        }
        else if (subNodeScale.y.type == Layout::ScaleType::FILL)
        {
            subNodeTrScale.y = cellSize.y;
        }
        else
        {
            return Result<Void>{.error = std::format("Grid only supports PX/REL or FILL scale type but"
                " subNode '{}' y axis is not one of those!", subNode->getName())};
        }

        /* Position subNodes. */
        subNodeTrPos.x = gridPosStart.x;
        subNodeTrPos.y = gridPosStart.y;

        /* Do self alignment. */
        RETURN_ON_ERROR(selfAlignGridSubNode(subNode, cellSize), Void);
    }

    return Result<Void>{};
}

/*
    Function calculates and updates the "computedPos" filed of the grid layout distribution in order
    for the position&scale algorithm to know where to start placing the subNodes and with what scale.
    Grid distribution scale type values can only be PX or FR (fractional).
    Node's padding and borders influence (aka shrinks) the usable space for distributing the grid cells.
*/
Result<Void> CustomLayoutEngine::updateGridFracPart(const AbstractNodePtr& node)
{
    const Layout& nodeLayout = node->getLayout();
    const Layout::TBLR& nodeBorder = nodeLayout.border;
    const Layout::TBLR& nodePadding = nodeLayout.padding;
    const glm::vec2 nodeScale = {
        node->getTransform().scale.x
            - nodeBorder.left - nodeBorder.right
            - nodePadding.left - nodePadding.right,
        node->getTransform().scale.y
            - nodeBorder.top - nodeBorder.bot
            - nodePadding.top - nodePadding.bot,
    };

    Layout::GridDistribRC& gridDistribRC = node->getLayout().gridDistribRC;
    if (gridDistribRC.rows.size() <= 0 || gridDistribRC.cols.size() <= 0)
    {
        return Result<Void>{
            .error = "Invalid grid distribution. One of the axis has no distribution defined!"
        };
    }

    glm::ivec2 totalPx{0, 0};
    glm::ivec2 totalFrac{0, 0};
    for (const Layout::GridDistrib& gd : gridDistribRC.cols)
    {
        if (gd.type == Layout::ScaleType::FRAC) { totalFrac.x += gd.value; }
        if (gd.type == Layout::ScaleType::PX) { totalPx.x += gd.value; }
    }

    for (const Layout::GridDistrib& gd : gridDistribRC.rows)
    {
        if (gd.type == Layout::ScaleType::FRAC) { totalFrac.y += gd.value; }
        if (gd.type == Layout::ScaleType::PX) { totalPx.y += gd.value; }
    }

    /*
        A fractionary part is calculated by the available node scale minus the px subNodes
        scale then divided by the frac sum.
    */
    totalFrac.x = std::max(1, totalFrac.x); /* Cap just in case of divide by zero. */
    totalFrac.y = std::max(1, totalFrac.y);
    const float fracWidth = (nodeScale.x - totalPx.x) / totalFrac.x;
    const float fracHeight = (nodeScale.y - totalPx.y) / totalFrac.y;

    /* Compute and cache the locations where the grid boundaries will start and end. */
    glm::vec2 currentPos{
        nodeBorder.left + nodePadding.left,
        nodeBorder.top + nodePadding.top
    };
    for (Layout::GridDistrib& gd : gridDistribRC.cols)
    {
        gd.computedStart = currentPos.x;
        if (gd.type == Layout::ScaleType::FRAC) { currentPos.x += gd.value * fracWidth; }
        else if (gd.type == Layout::ScaleType::PX) { currentPos.x += gd.value; }
        else
        {
            return Result<Void>{
                .error = std::format("Grid distribution scale type for node '{}' cannot be other"
                    " than PX/FR on X axis", node->getName())
            };
        }
    }

    for (Layout::GridDistrib& gd : gridDistribRC.rows)
    {
        gd.computedStart = currentPos.y;
        if (gd.type == Layout::ScaleType::FRAC) { currentPos.y += gd.value * fracHeight; }
        else if (gd.type == Layout::ScaleType::PX) { currentPos.y += gd.value; }
        else
        {
            return Result<Void>{
                .error = std::format("Grid distribution scale type for node '{}' cannot be other"
                    " than PX/FR on X axis", node->getName())
            };
        }
    }

    return Result<Void>{};
}

/*
    Function works in the exact same way as alignSubnodes just that in this case it aligns the nodes
    relative to the grid cells they are in.
    Possible positions to align to are:
        TOP_LEFT, TOP_RIGHT, CENTER_LEFT, CENTER_RIGHT, BOTTOM_LEFT, BOTTOM_RIGHT.
    Alignment should take place only if the subNode has enough space in it's grid cell(s). If the
    subNode exceeds the bounds of it's cell(s) then the self align will not take place.
*/
Result<Void> CustomLayoutEngine::selfAlignGridSubNode(const AbstractNodePtr& node, const glm::vec2& cellSize)
{
    const Layout& layout = node->getLayout();
    const Layout::Align& align = layout.alignSelf;
    glm::vec3& nodeTrPos = node->getTransform().pos;
    const glm::vec3& nodeTrScale = node->getTransform().scale;

    /* Not an error, but we shouldn't do anything. */
    if (nodeTrScale.x >= cellSize.x || nodeTrScale.y >= cellSize.y)
    {
        return Result<Void>{};
    }

    switch (align)
    {
        case Layout::TOP:
        case Layout::LEFT:
        case Layout::BOTTOM:
        case Layout::RIGHT:
        case Layout::TOP_LEFT:
            /* Default it to top-left aka do nothing. */
            break;
        case Layout::CENTER:
            nodeTrPos.x += cellSize.x * 0.5f - nodeTrScale.x * 0.5f;
            nodeTrPos.y += cellSize.y * 0.5f - nodeTrScale.y * 0.5f;
            break;
        case Layout::TOP_RIGHT:
            nodeTrPos.x += cellSize.x * 0.5f;
            break;
        case Layout::CENTER_LEFT:
            nodeTrPos.y += cellSize.y * 0.5f - nodeTrScale.y * 0.5f;
            break;
        case Layout::CENTER_RIGHT:
            nodeTrPos.x += cellSize.x * 0.5f;
            nodeTrPos.y += cellSize.y * 0.5f - nodeTrScale.y * 0.5f;
            break;
        case Layout::CENTER_TOP:
            nodeTrPos.x += cellSize.x * 0.5f - nodeTrScale.x * 0.5f;
            break;
        case Layout::CENTER_BOTTOM:
            nodeTrPos.x += cellSize.x * 0.5f - nodeTrScale.x * 0.5f;
            nodeTrPos.y += cellSize.y * 0.5f;
            break;
        case Layout::BOTTOM_LEFT:
            nodeTrPos.y += cellSize.y * 0.5f;
            break;
        case Layout::BOTTOM_RIGHT:
            nodeTrPos.x += cellSize.x * 0.5f;
            nodeTrPos.y += cellSize.y * 0.5f;
            break;
        }

    return Result<Void>{};
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
    if (node->getType() != AbstractNode::NodeType::BOX
        && node->getType() != AbstractNode::NodeType::RECYCLE_LIST
        && node->getType() != AbstractNode::NodeType::TREEVIEW)
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
void CustomLayoutEngine::handleSliderNode(const AbstractNodePtr& node)
{
    const SliderPtr& castSlider = Utils::as<Slider>(node);
    const glm::vec2& nodePos = node->getTransform().pos;
    const glm::vec2& nodeScale = node->getTransform().scale;
    const bool isLayoutHorizontal = node->getLayout().type == Layout::Type::HORIZONTAL;
    const bool dynamicKnob = castSlider->isDyanmicKnobEnabled();
    const float sliderPercOffset = castSlider->getOffsetPerc();
    const float sliderMaxValue = castSlider->getSlideTo();
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

/*
    Function handles scale and positioning of the subNodes of a box divider called "separators". These separators
    are the vertical or horizontal bars the user drags on in order to minimize or maximize the space of a box
    in a box divider layout.
    Each box fully occupies the available space and their size shall not be changed. Layout scale types do not
    affect it so it shouldn't be changed.
    Depending on the boxDivider layout type, the box divider can divide space horizontally or verically.
    Separator's scale type shall also not be tinkered with as they are set to, depending on box divider type:
        1. Horizontal: Separator will be {x_px, 1.0_rel}
        2. Vertical: Separator will be {1.0_rel, x_px}
            - Where x_px means the user can customize the size on that axis
    The difference between the current mouse position and the last mouse position will determine how much we
    shall move the separators.
    The user can set min and max values for each box inside the divider and the boxDivider will try to
    obey the min/max rules as best as it can by spreading values to the other boxes as well.
    One requirement for this to have at least one slot without set min/max value, otherwise the layout will break.
*/
Result<Void> CustomLayoutEngine::handleBoxDividerNode(const AbstractNodePtr& node)
{
    /*
        Go through the boxDivider's subNodes and search for Separator nodes. If the separator is active aka
        it is currently being dragged by the user, find by how much the user has dragged it and update the
        REL scale of the two boxes the separator controls.
    */
    const Layout& layout = node->getLayout();
    const bool isLayoutHorizontal = layout.type == Layout::Type::HORIZONTAL;
    AbstractNodePVec subNodes = node->getChildren();
    int32_t subNodeCnt = subNodes.size();

    /* Compute separators size in order to know how much usable relative space we have left. */
    glm::vec2 separatorsTotalSize{0, 0};
    for (const AbstractNodePtr& subNode : subNodes)
    {
        if (subNode->getType() != AbstractNode::NodeType::BOX_DIVIDER_SEP){ continue; }

        if (isLayoutHorizontal)
        {
            separatorsTotalSize.x += subNode->getLayout().newScale.x.value;
        }
        else if (!isLayoutHorizontal)
        {
            separatorsTotalSize.y += subNode->getLayout().newScale.y.value;
        }
    }

    const glm::vec2& nodeTrScale = node->getTransform().scale;
    glm::vec2 usableNodeSpace = glm::vec2{
        nodeTrScale.x
            - layout.padding.left - layout.padding.right
            - layout.border.left - layout.border.right - separatorsTotalSize.x,
        nodeTrScale.y 
            - layout.padding.top - layout.padding.bot
            - layout.border.top - layout.border.bot - separatorsTotalSize.y
    };

    for (int32_t i = 0; i < subNodeCnt - 1; ++i)
    {
        if (subNodes[i]->getType() != AbstractNode::NodeType::BOX_DIVIDER_SEP){ continue; }

        /* Only handle the currently under drag separator. */
        BoxDividerSepPtr separator = Utils::as<BoxDividerSep>(subNodes[i]);
        if (!separator->checkIfActiveThenReset()) { continue; }

        BoxPtr firstBox = Utils::as<Box>(separator->getFirstBox());
        BoxPtr secondBox = Utils::as<Box>(separator->getSecondBox());
        Layout& fbLayout = firstBox->getLayout();
        Layout& sbLayout = secondBox->getLayout();

        /*
            "tempScale" is the difference between current and previous mouse position. If the user drags to
            the left then the firstBox size decreases and the secondBox size increases and vice versa. The temScale is
            given here in PIXELS difference.
            That pixel difference needs to be translated in a realative-to-the-node value since boxes are scale type
            relative.
            TODO: Maybe there's a better way or naming for "tempScale".
        */
        if (isLayoutHorizontal)
        {
            const float fbOffset = fbLayout.tempScale.x / usableNodeSpace.x;
            const float sbOffset = sbLayout.tempScale.x / usableNodeSpace.x;
            const float fbMinScaleRel = fbLayout.minScale.x / usableNodeSpace.x;
            const float sbMinScaleRel = sbLayout.minScale.x / usableNodeSpace.x;
            const float fbMaxScaleRel = fbLayout.maxScale.x / usableNodeSpace.x;
            const float sbMaxScaleRel = sbLayout.maxScale.x / usableNodeSpace.x;

            /* Min/Max bounds checks shall go here. */
            if (fbOffset + fbLayout.newScale.x.value >= fbMinScaleRel &&
                sbOffset + sbLayout.newScale.x.value >= sbMinScaleRel &&
                fbOffset + fbLayout.newScale.x.value <= fbMaxScaleRel &&
                sbOffset + sbLayout.newScale.x.value <= sbMaxScaleRel)
            {
                /* Apply the relative offsets. */
                fbLayout.newScale.x.value += fbOffset;
                sbLayout.newScale.x.value += sbOffset;
            }

            /* Reset the tempScale as we don't need the previous values. */
            fbLayout.tempScale.x = 0.0f;
            sbLayout.tempScale.x = 0.0f;
        }
        else if (!isLayoutHorizontal)
        {
            const float fbOffset = fbLayout.tempScale.y / nodeTrScale.y;
            const float sbOffset = sbLayout.tempScale.y / nodeTrScale.y;

            /* Min/Max bounds checks shall go here. */

            /* Apply the relative offsets. */
            fbLayout.newScale.y.value += fbOffset;
            sbLayout.newScale.y.value += sbOffset;

            /* Reset the tempScale as we don't need the previous values. */
            fbLayout.tempScale.y = 0.0f;
            sbLayout.tempScale.y = 0.0f;
        }
    }

    /*
        Try to keep each slot within the minimum and maximum user set values.
        This is especially important on first runtime pass and on window resizes to ensure the divider
        always obeys the user's min/max values.
    */
    RETURN_ON_ERROR(tryToSatisfyMinMaxBoxDividerValues(node, usableNodeSpace), Void);

    /* Traditionally, compute the scale and position of the BoxDivider's subnodes. */
    RETURN_ON_ERROR(computeBoxDividerSubNodesScale(node, usableNodeSpace), Void);
    RETURN_ON_ERROR(computeBoxDividerSubNodesPos(node), Void);

    return Result<Void>{};
}

/*
    Function is similar to the common variant "computeSubNodesScale" however it's explicitly made to
    work only for box dividers.
    There's no support for aligning subNodes or self aligning subNodes.
    No support for wrapping as there will be none.
    No overflow will be generated so no support for that either.
    Subnode's margins and node's padding+border will be taken into consideration for subNode scaling and
    positioning. Also the separators size will be subtracted and accounted for.
    The only accepted scaleType for the slots (boxes) is REL, however the separators can be PX on the
    boxDivider's layout direction.
*/
Result<Void> CustomLayoutEngine::computeBoxDividerSubNodesScale(const AbstractNodePtr& node,
    const glm::vec2 usableNodeSpace)
{
    const Layout& layout = node->getLayout();
    const bool isLayoutHorizontal = layout.type == Layout::Type::HORIZONTAL;

    glm::vec2 fillAvailableScale{usableNodeSpace};
    glm::vec2 totalInt{0, 0};
    glm::vec2 totalFloat{0, 0};
    glm::ivec2 fillSubNodesCnt{0, 0};
    int32_t slotCnt{0};
    AbstractNodePVec& subNodes = node->getChildren();
    for (const AbstractNodePtr& subNode : subNodes)
    {
        const Layout& subNodeLayout = subNode->getLayout();
        const Layout::ScaleXY& subNodeScale = subNodeLayout.newScale;
        const Layout::TBLR& subNodeMargin = subNodeLayout.margin;
        const glm::vec2& subNodeMin = subNodeLayout.minScale;
        const glm::vec2& subNodeMax = subNodeLayout.maxScale;
        const bool isXPx = subNodeScale.x.type == Layout::ScaleType::PX;
        const bool isYPx = subNodeScale.y.type == Layout::ScaleType::PX;
        const bool isXRel = subNodeScale.x.type == Layout::ScaleType::REL;
        const bool isYRel = subNodeScale.y.type == Layout::ScaleType::REL;
        glm::vec3& subNodeTrScale = subNode->getTransform().scale;

        /* Ligh validity checks. */
        if (subNode->getType() == AbstractNode::NodeType::BOX)
        {
            slotCnt++;
            if (isLayoutHorizontal && subNodeLayout.newScale.x.type != Layout::ScaleType::REL)
            {
                return Result<Void>{
                    .error = std::format("Subnode '{}' inside box divider is not scaleType REL on X axis!",
                        subNode->getName())
                };
            }
            else if (!isLayoutHorizontal && subNodeLayout.newScale.y.type != Layout::ScaleType::REL)
            {
                return Result<Void>{
                    .error = std::format("Subnode '{}' inside box divider is not scaleType REL on Y axis!",
                        subNode->getName())
                };
            }
        }

        /* PX nodes get scaled "as is". */
        if (isXPx) { subNodeTrScale.x = std::clamp(subNodeScale.x.value, subNodeMin.x, subNodeMax.x); }
        if (isYPx) { subNodeTrScale.y = std::clamp(subNodeScale.y.value, subNodeMin.y, subNodeMax.y); }

        /* REL subNode scale needs to account for subNode's magins (subtract that). */
        if (isXRel)
        {
            subNodeTrScale.x = (subNodeScale.x.value * usableNodeSpace.x)
                - (subNodeMargin.left + subNodeMargin.right);
            totalFloat.x += subNodeTrScale.x;
            subNodeTrScale.x = std::round(subNodeTrScale.x);
            totalInt.x += subNodeTrScale.x;
        }
        if (isYRel)
        {
            subNodeTrScale.y = (subNodeScale.y.value * usableNodeSpace.y)
                - (subNodeMargin.top + subNodeMargin.bot);
            totalFloat.y += subNodeTrScale.y;
            subNodeTrScale.y = std::round(subNodeTrScale.y);
            totalInt.y += subNodeTrScale.y;
        }
    }

    /* Relative scales can create off by one errors. Resolve them. */
    resolveCumulativeError(node, totalInt, totalFloat);

    if (slotCnt < 2)
    {
        return Result<Void>{.error = "Box divider cannot work with less than 2 slots!"};
    }

    return Result<Void>{};
}

/*
    Function is similar to the "computeSubNodesPos" function just that this works exclusively for the
    boxDivider node.
    Takes into account node's padding and borders as well as subNodes margin.
*/
Result<Void> CustomLayoutEngine::computeBoxDividerSubNodesPos(const AbstractNodePtr& node)
{
    const Layout& layout = node->getLayout();
    const bool isLayoutHorizontal = layout.type == Layout::Type::HORIZONTAL;
    const glm::vec3& nodeTrPos = node->getTransform().pos;
    const glm::vec2& nodePosOffsets = glm::vec2{
        layout.padding.left + layout.border.left,
        layout.padding.top + layout.border.top
    };

    if (layout.allowWrap)
    {
        return Result<Void>{.error = "Wrapping is not allowed on BoxDividers as it is useless! Disable it."};
    }

    glm::vec2 startPos{nodePosOffsets.x, nodePosOffsets.y};
    AbstractNodePVec& subNodes = node->getChildren();
    for (const AbstractNodePtr& subNode : subNodes)
    {
        const Layout& subNodeLayout = subNode->getLayout();
        const Layout::TBLR& subNodeMargin = subNodeLayout.margin;
        glm::vec3& trPos = subNode->getTransform().pos;
        glm::vec3& trScale = subNode->getTransform().scale;

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
        }
        else if (!isLayoutHorizontal)
        {
            startPos.y += trScale.y + tbMargin;
        }

        /*
            Change reference frame for each subNode to the node's frame aka subNodes are now relative
            to the parent's position.
        */
        trPos += glm::vec3{nodeTrPos.x, nodeTrPos.y, 0};
    }

    return Result<Void>{};
}

/*
    Function aims to satify as best as it can the min/max requirements per each slot set by the user.
    It will do this by capping and distributing relative values to the slots that can take those values
    without violating their min/max.
*/
Result<Void> CustomLayoutEngine::tryToSatisfyMinMaxBoxDividerValues(const AbstractNodePtr& node,
    const glm::vec2 usableNodeSpace)
{
    const bool isLayoutHorizontal = node->getLayout().type == Layout::Type::HORIZONTAL;
    const AbstractNodePVec& subNodes = node->getChildren();
    
    /*
        Compute by how much we are off in satisfying the min/max requirements of the subNodes.
        If the total relative scales computed get under the value of 1.0f (100%) then it means that
        the layout has capped one or more subNode boxes (due to their minimum) and now there's some
        empty relative space that the rest of the nodes need to stretch an fill.
        The same is true (but reversed) for the layout getting over 1.0f but now due to one or more
        boxes being max capped.
    */
    glm::vec2 totalRelative{0, 0};
    for (const AbstractNodePtr& subNode : subNodes)
    {
        /* Don't care if it's just a separator. */
        if (subNode->getType() == AbstractNode::NodeType::BOX_DIVIDER_SEP) { continue; }

        Layout& subNodeLayout = subNode->getLayout();

        /*
            Convert the minimums expressed in PIXELS to values expressed in REL. If the current scale is
            greater than the minimum, accumulate the difference.
        */
        if (isLayoutHorizontal)
        {
            const float relMinX = subNodeLayout.minScale.x / usableNodeSpace.x;
            const float relMaxX = subNodeLayout.maxScale.x / usableNodeSpace.x;
            subNodeLayout.newScale.x.value = std::clamp(subNodeLayout.newScale.x.value, relMinX, relMaxX);
            totalRelative.x += subNodeLayout.newScale.x.value;
        }
        else if (!isLayoutHorizontal)
        {
            const float relMinY = subNodeLayout.minScale.y / usableNodeSpace.y;
            const float relMaxY = subNodeLayout.maxScale.y / usableNodeSpace.y;
            subNodeLayout.newScale.y.value = std::clamp(subNodeLayout.newScale.y.value, relMinY, relMaxY);
            totalRelative.y += subNodeLayout.newScale.y.value;
        }
    }

    /*
        If the total relative scale is greater than 1.0f, it means that we need to shrink some boxes in order
        to satisfy the layout being always 1.0f.
        If the total relative scale is less than 1.0f, it means that we need to grow some boxes in order
        to satisfy the layout being always 1.0f.
        The amount to grow and shrink have been converted to the positive, greater than 0 domain.
    */
    glm::vec2 minLeftToSatisfy = totalRelative - 1.0f;
    glm::vec2 maxLeftToSatisfy = 1.0f - totalRelative;
    for (const AbstractNodePtr& subNode : subNodes)
    {
        /* Don't care if it's just a separator. */
        if (subNode->getType() == AbstractNode::NodeType::BOX_DIVIDER_SEP) { continue; }

        Layout& subNodeLayout = subNode->getLayout();

        if (isLayoutHorizontal)
        {
            const float relMinX = subNodeLayout.minScale.x / usableNodeSpace.x;
            const float relMaxX = subNodeLayout.maxScale.x / usableNodeSpace.x;
            const float distanceToMinX = subNodeLayout.newScale.x.value - relMinX;
            const float distanceToMaxX = relMaxX - subNodeLayout.newScale.x.value;
    
            /* If there's space to spread the minimum to this subNode, then the distance is positive. */
            if (distanceToMinX > 0 && minLeftToSatisfy.x > 0.0f)
            {
                /*
                    If there's more unsatisfied minimum space than this subNode can take then give this
                    subNode the full amount of spread it can take, but not all.
                */
                if (minLeftToSatisfy.x - distanceToMinX > 0)
                {
                    subNodeLayout.newScale.x.value -= distanceToMinX;
                    minLeftToSatisfy.x -= distanceToMinX;
                }
                /*
                    Otherwise it means that this subNode can take all the unsatisfied minimum without
                    violating min req.
                */
                else
                {
                    subNodeLayout.newScale.x.value -= minLeftToSatisfy.x;
                    minLeftToSatisfy.x = 0;
                }
            }
    
            if (distanceToMaxX > 0 && maxLeftToSatisfy.x > 0)
            {
                if (maxLeftToSatisfy.x - distanceToMaxX > 0)
                {
                    subNodeLayout.newScale.x.value += distanceToMaxX;
                    maxLeftToSatisfy.x -= distanceToMaxX;
                }
                else
                {
                    subNodeLayout.newScale.x.value += maxLeftToSatisfy.x;
                    maxLeftToSatisfy.x = 0;
                }
            }
        }
        else if (!isLayoutHorizontal)
        {
            const float relMinY = subNodeLayout.minScale.y / usableNodeSpace.y;
            const float relMaxY = subNodeLayout.maxScale.y / usableNodeSpace.y;
            const float distanceToMinY = subNodeLayout.newScale.y.value - relMinY;
            const float distanceToMaxY = relMaxY - subNodeLayout.newScale.y.value;
    
            if (distanceToMinY > 0 && minLeftToSatisfy.y > 0.0f)
            {
                if (minLeftToSatisfy.y - distanceToMinY > 0)
                {
                    subNodeLayout.newScale.y.value -= distanceToMinY;
                    minLeftToSatisfy.y -= distanceToMinY;
                }
                else
                {
                    subNodeLayout.newScale.y.value -= minLeftToSatisfy.y;
                    minLeftToSatisfy.y = 0;
                }
            }
    
            if (distanceToMaxY > 0 && maxLeftToSatisfy.y > 0)
            {
                if (maxLeftToSatisfy.y - distanceToMaxY > 0)
                {
                    subNodeLayout.newScale.y.value += distanceToMaxY;
                    maxLeftToSatisfy.y -= distanceToMaxY;
                }
                else
                {
                    subNodeLayout.newScale.y.value += maxLeftToSatisfy.y;
                    maxLeftToSatisfy.y = 0;
                }
            }
        }
    }

    /*
        If there's some unsatisfied min/max at this stage, it means the layout is in an invalid state as we cannot
        obey user's requirements.
    */
    if ((isLayoutHorizontal && minLeftToSatisfy.x > 0) || (!isLayoutHorizontal && minLeftToSatisfy.y > 0))
    {
        return Result<Void>{
            .error = "Minimum values for the slots cannot be satisfied! Consider relaxing the minimums."};
    }

    if ((isLayoutHorizontal && maxLeftToSatisfy.x > 0) || (!isLayoutHorizontal && maxLeftToSatisfy.y > 0))
    {
        return Result<Void>{
            .error = "Maximum values for the slots cannot be satisfied! Consider relaxing the maximums."};
    }

    return Result<Void>{};
}

/*
    Function will align the subItems container of a dropdown node in a user preferred position:
     -> TOP, BOTTOM, LEFT, RIGHT of the Dropdown node itself.
    In case the user chosen position cannot show the entire sub items container on the screen, the next
    best position that will satisfy this condition will be chosen. (to be implemented sometime later)
    If there's no such place, the last checked position will be used even if it goes out of the screen.
*/
Result<Void> CustomLayoutEngine::handleDropdown(const AbstractNodePtr& node)
{
    const DropdownPtr& dropdown = Utils::as<Dropdown>(node);

    /* Nothing to be done is the dropdown is not open. */
    if (!dropdown->isDropdownOpen()) { return Result<Void>{}; }

    /* Dropdown is always guarnateed here to have one subNode. */
    AbstractNodePtr& subItemsContainer = dropdown->getChildren()[0];

    const Result<glm::vec2>& res = computeFitScale(subItemsContainer);
    RETURN_ON_ERROR(res, Void);

    const glm::vec3& dropdownPos = dropdown->getTransform().pos;
    const glm::vec3& dropdownScale = dropdown->getTransform().scale;
    glm::vec3& containerPos = subItemsContainer->getTransform().pos;
    glm::vec3& containerScale = subItemsContainer->getTransform().scale;
    containerScale.x = res.value.x;
    containerScale.y = res.value.y;

    const Dropdown::Expand expandDir = dropdown->getExpandDirection();
    switch (expandDir)
    {
        case Dropdown::Expand::LEFT:
            containerPos.x = dropdownPos.x - containerScale.x;
            containerPos.y = dropdownPos.y;
            break;
        case Dropdown::Expand::RIGHT:
            containerPos.x = dropdownPos.x + dropdownScale.x;
            containerPos.y = dropdownPos.y;
            break;
        case Dropdown::Expand::TOP:
            containerPos.x = dropdownPos.x;
            containerPos.y = dropdownPos.y - containerScale.y;
            break;
        case Dropdown::Expand::BOTTOM:
            containerPos.x = dropdownPos.x;
            containerPos.y = dropdownPos.y + dropdownScale.y;
            break;
    }

    return Result<Void>{};
}

/*
    Function deals with a box that's "floating" above all the other nodes. Generally used as a pop-up window
    or a context menu when right clicking.
    The layout is calculted just as a normal Box, but at the end the final position is user supplied.
*/
Result<Void> CustomLayoutEngine::handleFloatingBox(const AbstractNodePtr& node)
{
    FloatingBoxPtr floatingBoxPtr = Utils::as<FloatingBox>(node);

    const auto& preferredPos = floatingBoxPtr->getPreferredPos();
    auto& pos = floatingBoxPtr->getTransform().pos;
    pos = {preferredPos.x, preferredPos.y, pos.z};

    node->getTransform().pos.x = pos.x;
    node->getTransform().pos.y = pos.y;
    node->getChildren()[0]->getTransform().pos.x = pos.x;
    node->getChildren()[0]->getTransform().pos.y = pos.y;

    return Result<Void>{};
}

/*
    Function handles mainly overflow control (Scrollbars) of recycle lists. Additionally computes what element
    index is the new top of the list such that the recycle list can remove old items and append new ones.
*/
Result<Void> CustomLayoutEngine::handleRecycleList(const AbstractNodePtr& node)
{
    const RecycleListPtr& rlPtr = Utils::as<RecycleList>(node);
    const int32_t rowSize = rlPtr->getItemScale().y.value;
    const glm::vec3& trPos = node->getTransform().pos;
    const glm::vec3& trScale = node->getTransform().scale;
    const Layout::TBLR& itemMargin = rlPtr->getItemMargin();
    RecycleList::Internals& internalsRef = rlPtr->getInternalsRef();

    const int32_t rowSizeAndMargin = rowSize + itemMargin.top + itemMargin.bot;
    const int32_t maxDisplayAmt = trScale.y / rowSizeAndMargin + 1;
    const SliderPtr vBar = rlPtr->getVBar().lock();
    const SliderPtr hBar = rlPtr->getHBar().lock();
    internalsRef.topOfListIdx = vBar->getSlideCurrentValue() / rowSizeAndMargin;
    internalsRef.visibleNodes = maxDisplayAmt + 1;

    /* Trigger nodes readdition if top of the list changed or if the list has new changes. */
    if (internalsRef.isDirty || internalsRef.topOfListIdx != internalsRef.oldTopOfListIdx
        || internalsRef.oldVisibleNodes != internalsRef.visibleNodes)
    {
        internalsRef.isDirty = false;
        rlPtr->onLayoutDirtyPost();
    }

    const int32_t hBarActiveSize = rlPtr->isScrollBarActive(Layout::Type::HORIZONTAL)
        ? hBar->getLayout().newScale.y.value : 0;
    const int32_t vBarActiveSize = rlPtr->isScrollBarActive(Layout::Type::VERTICAL)
        ? vBar->getLayout().newScale.x.value : 0;

    /* Compute overflow value now that elements are in place. */
    float maxX{0};
    auto& subNodes = node->getChildren();
    for (AbstractNodePtr& subNode : subNodes)
    {
        SKIP_SCROLL_NODE(subNode);
        glm::vec3& subNodeTrScale = subNode->getTransform().scale;
        subNodeTrScale.x -= vBarActiveSize;
        maxX = std::max(maxX, subNodeTrScale.x);
    }

    internalsRef.overflow.x = maxX - trScale.x;
    internalsRef.overflow.x = std::max(0, internalsRef.overflow.x + 2);
    internalsRef.overflow.y = internalsRef.elementsCount * rowSizeAndMargin - trScale.y + hBarActiveSize;
    internalsRef.overflow.y = std::max(0, internalsRef.overflow.y + 2);

    /* Update internals. */
    rlPtr->setOverflow(internalsRef.overflow);

    internalsRef.oldTopOfListIdx = internalsRef.topOfListIdx;
    internalsRef.oldVisibleNodes = internalsRef.visibleNodes;
    internalsRef.lastScaleY = trScale.y;
    internalsRef.lastScaleX = trScale.x;

    /* Apply the scroll offset. */
    for (AbstractNodePtr& subNode : subNodes)
    {
        SKIP_SCROLL_NODE(subNode);
        subNode->getTransform().pos.y -= (int32_t)vBar->getSlideCurrentValue() % rowSizeAndMargin;
        subNode->getTransform().pos.x -= hBar->getSlideCurrentValue();
    }

    return Result<Void>{};
}

/*
    Function handles mainly overflow control (Scrollbars) of tree view. Additionally computes what element
    index is the new top of the list such that the tree view can remove old items and append new ones.
*/
Result<Void> CustomLayoutEngine::handleTreeView(const AbstractNodePtr& node)
{
    const TreeViewPtr& twPtr = Utils::as<TreeView>(node);
    const int32_t rowSize = twPtr->getItemScale().y.value;
    const glm::vec3& trPos = node->getTransform().pos;
    const glm::vec3& trScale = node->getTransform().scale;
    const Layout::TBLR& itemMargin = twPtr->getItemMargin();
    TreeView::Internals& internalsRef = twPtr->getInternalsRef();

    const int32_t rowSizeAndMargin = rowSize + itemMargin.top + itemMargin.bot;
    const int32_t maxDisplayAmt = trScale.y / rowSizeAndMargin + 1;
    const SliderPtr vBar = twPtr->getVBar().lock();
    const SliderPtr hBar = twPtr->getHBar().lock();
    internalsRef.topOfListIdx = vBar->getSlideCurrentValue() / rowSizeAndMargin;
    internalsRef.visibleNodes = maxDisplayAmt + 1;

    /* Trigger nodes readdition if top of the list changed or if the list has new changes. */
    if (internalsRef.isDirty || internalsRef.topOfListIdx != internalsRef.oldTopOfListIdx
        || internalsRef.oldVisibleNodes != internalsRef.visibleNodes)
    {
        internalsRef.isDirty = false;
        twPtr->onLayoutDirtyPost();
    }

    const int32_t hBarActiveSize = twPtr->isScrollBarActive(Layout::Type::HORIZONTAL)
        ? hBar->getLayout().newScale.y.value : 0;
    const int32_t vBarActiveSize = twPtr->isScrollBarActive(Layout::Type::VERTICAL)
        ? vBar->getLayout().newScale.x.value : 0;

    /* Compute overflow value now that elements are in place. */
    float maxX{0};
    auto& subNodes = node->getChildren();
    for (AbstractNodePtr& subNode : subNodes)
    {
        SKIP_SCROLL_NODE(subNode);
        glm::vec3& subNodeTrScale = subNode->getTransform().scale;
        subNodeTrScale.x -= vBarActiveSize;
        maxX = std::max(maxX, subNodeTrScale.x);
    }

    internalsRef.overflow.x = maxX - trScale.x;
    internalsRef.overflow.x = std::max(0, internalsRef.overflow.x + 2);
    internalsRef.overflow.y = internalsRef.elementsCount * rowSizeAndMargin - trScale.y + hBarActiveSize;
    internalsRef.overflow.y = std::max(0, internalsRef.overflow.y + 2);

    /* Update internals. */
    twPtr->setOverflow(internalsRef.overflow);

    internalsRef.oldTopOfListIdx = internalsRef.topOfListIdx;
    internalsRef.oldVisibleNodes = internalsRef.visibleNodes;
    internalsRef.lastScaleY = trScale.y;
    internalsRef.lastScaleX = trScale.x;

    /* Apply the scroll offset. */
    for (AbstractNodePtr& subNode : subNodes)
    {
        SKIP_SCROLL_NODE(subNode);
        subNode->getTransform().pos.y -= (int32_t)vBar->getSlideCurrentValue() % rowSizeAndMargin;
        subNode->getTransform().pos.x -= hBar->getSlideCurrentValue();
    }

    return Result<Void>{};
}

/*
    Function aims to distribute the rounding errors accumulated so far in a layout, mostly due to relative
    subNodes scaling.
    This is achieved by distributing one pixel to each subNode until there's no more error to distribute.
    It is possible now for some nodes to be +/- 1 pixel off when rendering but it's a tradeoff to be taken.
*/
void CustomLayoutEngine::resolveCumulativeError(const AbstractNodePtr& node, const glm::vec2 totalInt,
    const glm::vec2 totalFloat)
{
    AbstractNodePVec& subNodes = node->getChildren();
    glm::vec2 diff = {std::round(totalFloat.x - totalInt.x), std::round(totalFloat.y - totalInt.y)};
    for (AbstractNodePtr& subNode : subNodes)
    {
        if (diff.x == 0 && diff.y == 0) { break; }

        glm::vec3& subNodeTrScale = subNode->getTransform().scale;
        if (diff.x)
        {
            const bool negDiffX = std::signbit(diff.x);
            subNodeTrScale.x = negDiffX ? subNodeTrScale.x - 1.0f : subNodeTrScale.x + 1.0f;
            diff.x = negDiffX ? diff.x + 1 : diff.x - 1;
        }

        if (diff.y)
        {
            const bool negDiffY = std::signbit(diff.y);
            subNodeTrScale.y = negDiffY ? subNodeTrScale.y - 1.0f : subNodeTrScale.y + 1.0f;
            diff.y = negDiffY ? diff.y + 1 : diff.y - 1;
        }
    }
}

#undef SKIP_COMPOSED_DIRECTIONS
#undef SKIP_HORIZONTAL_DIRECTIONS
#undef SKIP_VERTICAL_DIRECTIONS
#undef SKIP_SCROLL_NODE
#undef SKIP_DROPDOWN_CONTAINER_NODE
#undef RETURN_ON_ERROR
} // namespace msgui::layoutengine
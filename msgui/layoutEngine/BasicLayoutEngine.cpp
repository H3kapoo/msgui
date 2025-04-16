#include "BasicLayoutEngine.hpp"

#include <algorithm>
#include <cmath>

#include "msgui/layoutEngine/utils/LayoutData.hpp"
#include "msgui/node/AbstractNode.hpp"
#include "msgui/node/Box.hpp"
#include "msgui/node/Dropdown.hpp"
#include "msgui/node/FloatingBox.hpp"
#include "msgui/node/Slider.hpp"
#include "msgui/node/TreeView.hpp"
#include "msgui/node/utils/BoxDividerSep.hpp"
#include "msgui/node/utils/SliderKnob.hpp"

namespace msgui
{

/* Some simple macros to ignore cases in switches or to ignore scrollbars from being computed inside
   some of the for loops. */
#define IGNORE_GRID_ALIGN\
    case utils::Layout::TOP_LEFT:\
    case utils::Layout::TOP_RIGHT:\
    case utils::Layout::CENTER_LEFT:\
    case utils::Layout::CENTER_RIGHT:\
    case utils::Layout::CENTER_TOP:\
    case utils::Layout::CENTER_BOTTOM:\
    case utils::Layout::BOTTOM_LEFT:\
    case utils::Layout::BOTTOM_RIGHT:\
        break;\

#define IGNORE_LR_ALIGN\
    case utils::Layout::LEFT:\
    case utils::Layout::RIGHT:\
        break;\

#define IGNORE_TB_ALIGN\
    case utils::Layout::TOP:\
    case utils::Layout::BOTTOM:\
        break;\

#define IGNORE_SCROLLBAR\
    if (ch->getType() == AbstractNode::NodeType::SCROLL ||\
        ch->getType() == AbstractNode::NodeType::SCROLL_KNOB)\
    { continue; }\

#define IGNORE_FLOATING_BOX\
    if (ch->getType() == AbstractNode::NodeType::FLOATING_BOX) { continue; }\
    if (ch->getType() == AbstractNode::NodeType::DROPDOWN_CONTAINTER) { continue; }\


glm::vec2 BasicLayoutEngine::process(const AbstractNodePtr& node)
{
    const AbstractNodePVec& children = node->getChildren();
    /* Treeview is excused because it's children get added as this function runs so it may happen that
       at the start of the function it doesn't have any children yet.*/
    if (children.empty() && node->getType() != AbstractNode::NodeType::TREEVIEW) { return {0, 0}; }

    /* Useless to compute further if node is a scroll node type. We compute these separately */
    if (node->getType() == AbstractNode::NodeType::SCROLL) { return {0, 0}; }

    const utils::Layout& layout = node->getLayout();
    const glm::vec2 pbScale = getPaddedAndBorderedNodeScale(node);
    if (layout.type == utils::Layout::Type::GRID)
    {
        processGridLayout(pbScale, node);
        return {0, 0}; /* Grid layout will not generate overflow */
    }

    /* Compute Scrollbars positioning/scale and by how much to shrink node scale for further calcs */
    ScrollBarsData scrollNodeData = processScrollbars(node);

    const glm::vec2 nShrunkScale = pbScale - glm::vec2{scrollNodeData.shrinkBy.x, scrollNodeData.shrinkBy.y};

    /* BoxDividers need to be calculated separately due to their special attributes */
    if (node->getType() == AbstractNode::NodeType::BOX_DIVIDER)
    {
        processBoxDivider(nShrunkScale, children);
    }

    /* Compute node children scale */
    computeNodeScale(nShrunkScale, node);


    /* Compute sliders separately */
    if (node->getType() == AbstractNode::NodeType::SLIDER)
    {
        processSlider(node);
        return {0, 0}; /* Slider will not generate overflow */
    }

    /* Process HV layout */
    processHVLayout(node, nShrunkScale);

    /* Compute children overflow pass */
    const glm::vec2 computedOverflow = computeOverflow(nShrunkScale, children);

    /* Apply scrollbar offsets + any group offseting from zero offset */
    applyFinalOffsets(node, computedOverflow, scrollNodeData);

    /* TreeView has even MORE final offsets due to the scrollbar it can have. As such, calculations need to be done
       after applying "final" offsets. */
    if (node->getType() == AbstractNode::NodeType::TREEVIEW)
    {
        processTreeView(node);
    }

    if (node->getType() == AbstractNode::NodeType::DROPDOWN)
    {
        processDropdown(node);
        return {0, 0}; /* Dropdown will not generate overflow by itself */
    }

    /* Compute floating boxes separately as their positioning is special */
    if (node->getType() == AbstractNode::NodeType::FLOATING_BOX)
    {
        processFloatingBox(node);
        return {0, 0}; /* Floating box will not generate overflow */
    }

    return computedOverflow;
}

glm::vec2 BasicLayoutEngine::getPaddedAndBorderedNodeScale(const AbstractNodePtr& node)
{
    const utils::Layout& layout = node->getLayout();

    glm::vec3 newScale = node->getTransform().scale;
    newScale.x -= layout.padding.left + layout.padding.right
        + layout.border.left + layout.border.right;
    newScale.y -= layout.padding.top + layout.padding.bot
        + layout.border.bot + layout.border.top;
    return newScale;
}

glm::vec2 BasicLayoutEngine::getTotalChildrenAbsScale(const AbstractNodePVec& children)
{
    glm::vec2 totalAbsChildSize{0};
    for (auto& ch : children)
    {
        IGNORE_SCROLLBAR;
        IGNORE_FLOATING_BOX;

        const utils::Layout& chLayout = ch->getLayout();
        if (chLayout.scaleType.x == utils::Layout::ScaleType::PX)
        {
            totalAbsChildSize.x += chLayout.scale.x;
        }

        if (chLayout.scaleType.y == utils::Layout::ScaleType::PX)
        {
            totalAbsChildSize.y += chLayout.scale.y;
        }
    }
    return totalAbsChildSize;
}

glm::vec2 BasicLayoutEngine::getBoxDivUnsatisfiedMinimumTotal(const AbstractNodePVec& children,
    const glm::vec2 nScale)
{
    glm::vec2 runningMinOverflow{0, 0};
    for (auto& ch : children)
    {
        if (ch->getType() == AbstractNode::NodeType::BOX_DIVIDER_SEP) { continue; }

        utils::Layout& chLayout = ch->getLayout();
        float newMinX = Utils::remap(chLayout.minScale.x, 0, nScale.x, 0.0f, 1.0f);
        if (newMinX - chLayout.scale.x > 0)
        {
            runningMinOverflow.x += newMinX - chLayout.scale.x;
        }
        chLayout.scale.x = std::max(chLayout.scale.x, newMinX);

        float newMinY = Utils::remap(chLayout.minScale.y, 0, nScale.y, 0.0f, 1.0f);
        if (newMinY - chLayout.scale.y > 0)
        {
            runningMinOverflow.y += newMinY - chLayout.scale.y;
        }
        chLayout.scale.y = std::max(chLayout.scale.y, newMinY);
    }
    return runningMinOverflow;
}

void BasicLayoutEngine::trySpreadBoxDivUnsatMinimum(const AbstractNodePtr& chNode, const glm::vec2 nScale,
    glm::vec2& unsatMin)
{
    utils::Layout& chLayout = chNode->getLayout();
    if (unsatMin.x > 0.01f)
    {
        float newMinX = Utils::remap(chLayout.minScale.x, 0, nScale.x, 0.0f, 1.0f);
        float distToMinX = chLayout.scale.x - newMinX;
        if (distToMinX > 0)
        {
            if (unsatMin.x - distToMinX > 0)
            {
                chLayout.scale.x -= distToMinX;
                unsatMin.x -= distToMinX;
            }
            else
            {
                chLayout.scale.x -= unsatMin.x;
                unsatMin.x = 0;
            }
        }
    }

    if (unsatMin.y > 0.01f)
    {
        float newMinY = Utils::remap(chLayout.minScale.y, 0, nScale.y, 0.0f, 1.0f);
        float distToMinY = chLayout.scale.y - newMinY;
        if (distToMinY > 0)
        {
            if (unsatMin.y - distToMinY > 0)
            {
                chLayout.scale.y -= distToMinY;
                unsatMin.y -= distToMinY;
            }
            else
            {
                chLayout.scale.y -= unsatMin.y;
                unsatMin.y = 0;
            }
        }
    }
}

glm::vec2 BasicLayoutEngine::computeTotalRealNodesScale(const AbstractNodePVec& children)
{
    glm::vec2 totalChildSize{0, 0};
    for (auto& ch : children)
    {
        IGNORE_SCROLLBAR;
        IGNORE_FLOATING_BOX;

        const utils::Layout& chLayout = ch->getLayout();
        totalChildSize.x += ch->getTransform().scale.x + (chLayout.margin.left + chLayout.margin.right);
        totalChildSize.y += ch->getTransform().scale.y + (chLayout.margin.top + chLayout.margin.bot);
    }
    return totalChildSize;
}

glm::vec2 BasicLayoutEngine::computeSpacing(const AbstractNodePtr& node, const float nScale,
    const float totalChildSize)
{
    const utils::Layout& layout = node->getLayout();
    const auto children = node->getChildren();
    const auto childrenSize = children.size();

    float spacing{0};
    float additionalStartPush{0};
    switch (layout.spacing)
    {
        case utils::Layout::TIGHT:
            // Do nothing
            break;
        case utils::Layout::EVEN_WITH_NO_START_GAP:
            spacing = (nScale - totalChildSize) / (childrenSize - 1);
            spacing = std::max(0.0f, spacing);
            break;
        case utils::Layout::EVEN_WITH_START_GAP:
            spacing = (nScale - totalChildSize) / childrenSize;
            spacing = std::max(0.0f, spacing);
            additionalStartPush += spacing * 0.5f;
            break;
        default:
            log_.warnLn("Unrecognized Spacing value: ENUM(%d)",
                static_cast<uint8_t>(layout.spacing));
    }

    return {spacing, additionalStartPush};
}

void BasicLayoutEngine::computeNodeScale(const glm::vec2& pScale, const AbstractNodePtr& node)
{
    auto& children = node->getChildren();
    const glm::vec2 total = getTotalChildrenAbsScale(children);
    glm::vec2 newParentScale = pScale;

    newParentScale.x -= node->getLayout().type == utils::Layout::Type::HORIZONTAL ? total.x : 0;
    newParentScale.y -= node->getLayout().type == utils::Layout::Type::VERTICAL ? total.y : 0;
    for (auto& ch : children)
    {
        /* Shall not be taken into consideration as these are calculated differently */
        IGNORE_SCROLLBAR;

        const utils::Layout& chLayout = ch->getLayout();
        if (chLayout.scaleType.x == utils::Layout::ScaleType::REL)
        {
            auto& scale = ch->getTransform().scale;
            scale.x = newParentScale.x * chLayout.scale.x;
            scale.x -= chLayout.margin.left + chLayout.margin.right;
            scale.x = std::round(scale.x);
        }
        else if (chLayout.scaleType.x == utils::Layout::ScaleType::PX)
        {
            auto& scale = ch->getTransform().scale;
            scale.x = chLayout.scale.x;
        }

        if (chLayout.scaleType.y == utils::Layout::ScaleType::REL)
        {
            auto& scale = ch->getTransform().scale;
            scale.y = newParentScale.y * chLayout.scale.y;
            scale.y -= chLayout.margin.top + chLayout.margin.bot;
            scale.y = std::round(scale.y);
        }
        else if (chLayout.scaleType.y == utils::Layout::ScaleType::PX)
        {
            auto& scale = ch->getTransform().scale;
            scale.y = chLayout.scale.y;
        }
    }
}

void BasicLayoutEngine::resolveAlignSelf(const AbstractNodePVec& children, const uint32_t idxStart,
    const uint32_t idxEnd, const int32_t max, const utils::Layout::Type type)
{
    for (uint32_t i = idxStart; i < idxEnd; i++)
    {
        const utils::Layout& chLayout = children[i]->getLayout();
        auto& pos = children[i]->getTransform().pos;
        auto& scale = children[i]->getTransform().scale;

        if (type == utils::Layout::Type::HORIZONTAL)
        {
            switch (chLayout.alignSelf)
            {
                IGNORE_GRID_ALIGN
                IGNORE_LR_ALIGN
                case utils::Layout::TOP:
                    // Do nothing
                    break;
                case utils::Layout::CENTER:
                    pos.y += (max - scale.y - chLayout.margin.top - chLayout.margin.bot) * 0.5f;
                    break;
                case utils::Layout::BOTTOM:
                    pos.y += max - scale.y - chLayout.margin.top - chLayout.margin.bot;
                    break;
                default:
                    log_.warnLn("Unrecognized horizontal alignSelf value: ENUM(%d)",
                        static_cast<uint8_t>(chLayout.alignSelf));
            }
        }
        else if (type == utils::Layout::Type::VERTICAL)
        {
            switch (chLayout.alignSelf)
            {
                IGNORE_GRID_ALIGN
                IGNORE_TB_ALIGN
                case utils::Layout::LEFT:
                    // Do nothing
                    break;
                case utils::Layout::CENTER:
                    pos.x += (max - scale.x - chLayout.margin.left - chLayout.margin.right) * 0.5f;
                    break;
                case utils::Layout::RIGHT:
                    pos.x += max - scale.x - chLayout.margin.left - chLayout.margin.right;
                    break;
                default:
                    log_.warnLn("Unrecognized vertical alignSelf value: ENUM(%d)",
                        static_cast<uint8_t>(chLayout.alignSelf));
            }
        }
    }
}

glm::vec2 BasicLayoutEngine::computeOverflow(const glm::vec2& pScale, const AbstractNodePVec& children)
{
    glm::vec2 currentScale{0, 0};
    for (auto& ch : children)
    {
        /* Shall not be taken into consideration for overflow */
        IGNORE_SCROLLBAR;
        IGNORE_FLOATING_BOX;

        const utils::Layout& chLayout = ch->getLayout();
        auto scale = ch->getTransform().scale;
        auto pos = ch->getTransform().pos;
        scale.x += chLayout.margin.right;
        scale.y += chLayout.margin.bot;
        currentScale.x = std::max(currentScale.x, pos.x + scale.x);
        currentScale.y = std::max(currentScale.y, pos.y + scale.y);
    }

    return {currentScale.x - pScale.x, currentScale.y - pScale.y};
}

void BasicLayoutEngine::applyFinalOffsets(const AbstractNodePtr& node, const glm::vec2 overflow,
    const ScrollBarsData& scrollNodeData)
{
    const auto& children = node->getChildren();
    const utils::Layout& layout = node->getLayout();
    const auto& nPos = node->getTransform().pos;
    for (auto& ch : children)
    {
        /* Already calculated, skip */
        IGNORE_SCROLLBAR;
        IGNORE_FLOATING_BOX;

        auto& pos = ch->getTransform().pos;
        auto& scale = ch->getTransform().scale;

        pos.x += -scrollNodeData.offsetPx.x + nPos.x + layout.padding.left + layout.border.left;
        pos.y += -scrollNodeData.offsetPx.y + nPos.y + layout.padding.top + layout.border.top;

        /* AlignChild. Negative overflow means we still have X amount of pixels until the parent is full on that axis
           We can leverage this to position elements top, left, right, bot, center. */
        if (overflow.x < 0 && (layout.spacing == utils::Layout::Spacing::TIGHT || layout.type == utils::Layout::Type::VERTICAL))
        {
            switch (layout.alignChild.x)
            {
                IGNORE_GRID_ALIGN
                IGNORE_TB_ALIGN
                case utils::Layout::LEFT:
                    // Do nothing
                    break;
                case utils::Layout::CENTER:
                    pos.x += -overflow.x * 0.5f;
                    break;
                case utils::Layout::RIGHT:
                    pos.x += -overflow.x;
                    break;
                default:
                    log_.warnLn("Unrecognized alignChildX value: ENUM(%d)",
                        static_cast<uint8_t>(layout.alignChild.x));
            }
        }

        if (overflow.y < 0 && (layout.spacing == utils::Layout::Spacing::TIGHT || layout.type == utils::Layout::Type::HORIZONTAL))
        {
            switch (layout.alignChild.y)
            {
                IGNORE_GRID_ALIGN
                IGNORE_LR_ALIGN
                case utils::Layout::TOP:
                    // Do nothing
                    break;
                case utils::Layout::CENTER:
                    pos.y += -overflow.y * 0.5f;
                    break;
                case utils::Layout::BOTTOM:
                    pos.y += -overflow.y;
                    break;
                default:
                    log_.warnLn("Unrecognized alignChildY value: ENUM(%d)",
                        static_cast<uint8_t>(layout.alignChild.y));
            }
        }

        pos.x = std::round(pos.x);
        pos.y = std::round(pos.y);

        pos.x += ch->getLayout().shrink.x;
        pos.y += ch->getLayout().shrink.y;
        scale.x -= ch->getLayout().shrink.x * 2;
        scale.y -= ch->getLayout().shrink.y * 2;
    }
}

BasicLayoutEngine::ScrollBarsData BasicLayoutEngine::processScrollbars(const AbstractNodePtr& parent)
{
    const AbstractNodePVec& children = parent->getChildren();
    if (children.empty())
    {
        return ScrollBarsData{};
    }

    const utils::Layout& layout = parent->getLayout();

    /* Return by how much should the parent "shrink" to fit scrollbars */
    ScrollBarsData data;

    //TODO: The else/if shall be refactored
    bool bothSlOn{false};
    if (parent->getType() == AbstractNode::NodeType::BOX)
    {
        BoxPtr castBox = Utils::as<Box>(parent);
        if (!castBox->isScrollBarActive(utils::Layout::Type::HORIZONTAL)
            && !castBox->isScrollBarActive(utils::Layout::Type::VERTICAL))
        {
            return ScrollBarsData{};
        }

        if (castBox->isScrollBarActive(utils::Layout::Type::HORIZONTAL)
            && castBox->isScrollBarActive(utils::Layout::Type::VERTICAL))
        {
            bothSlOn = true;
        }
    }
    // else if (parent->getType() == AbstractNode::NodeType::TREEVIEW)
    // {
    //     TreeViewPtr castTv = Utils::as<TreeView>(parent);
    //     if (castTv->isScrollBarActive(ScrollBar::Type::NONE))
    //     {
    //         return ScrollBarsData{};
    //     }

    //     if (castTv->isScrollBarActive(ScrollBar::Type::ALL))
    //     {
    //         bothSbOn = true;
    //     }
    // }

    auto& nPos = parent->getTransform().pos;
    auto& pScale = parent->getTransform().scale;

    /* Dealing with scrollbars, if any */
    for (auto& ch : children)
    {   
        /* Ignore non-Scrollbar elements */
        if (ch->getType() != AbstractNode::NodeType::SCROLL) { continue; }

        SliderPtr sl = Utils::as<Slider>(ch);
        if (!sl)
        {
            log_.errorLn("Failed to cast to ScrollBar!");
            return ScrollBarsData{};
        }

        auto& pos = ch->getTransform().pos;
        auto& scale = ch->getTransform().scale;

        if (sl->getLayout().type == utils::Layout::Type::VERTICAL)
        {
            /* Scrollbar positioning */
            pos.x = nPos.x + pScale.x - scale.x - layout.border.right;
            pos.y = nPos.y + layout.border.top;
            scale.y = pScale.y - (bothSlOn ? scale.x : 0)
             - (layout.border.top + layout.border.bot);
            scale.x = ch->getLayout().scale.x;

            /* Knob positioning */
            AbstractNodePtr knob = sl->getChildren()[0]; /* Always exists */
            auto& kScale = knob->getTransform().scale;
            auto& kPos = knob->getTransform().pos;
            float slOffset = sl->getOffsetPerc();

            int32_t slOverflowSize = sl->getSlideTo();
            kScale.x = scale.x;
            kScale.y = scale.y - slOverflowSize;
            kScale.y = std::max(scale.x, kScale.y);

            float newY = Utils::remap(slOffset,
                0.0f, 1.0f, pos.y + kScale.y / 2, pos.y + scale.y - kScale.y / 2);
            kPos.x = pos.x;
            kPos.y =  newY - kScale.y / 2;

            /* Horizontal available space needs to decrease & current offset in px */
            data.shrinkBy.x = scale.x;
            data.offsetPx.y = sl->getSlideCurrentValue();
        }
        else if (sl->getLayout().type == utils::Layout::Type::HORIZONTAL)
        {
            /* Scrollbar positioning */
            pos.y = nPos.y + pScale.y - scale.y - layout.border.bot;
            pos.x = nPos.x + layout.border.left;
            scale.x = pScale.x - (bothSlOn ? scale.y : 0)
                - (layout.border.left + layout.border.right);
            scale.y = ch->getLayout().scale.y;

            /* Knob positioning */
            AbstractNodePtr knob = sl->getChildren()[0]; // Always exists
            auto& kScale = knob->getTransform().scale;
            auto& kPos = knob->getTransform().pos;
            float slOffset = sl->getOffsetPerc();

            int32_t slOverflowSize = sl->getSlideTo();
            kScale.x = scale.x - slOverflowSize;
            kScale.x = std::max(scale.y, kScale.x);
            kScale.y = scale.y;

            float newX = Utils::remap(slOffset,
                0.0f, 1.0f, pos.x + kScale.x / 2, pos.x + scale.x - kScale.x / 2);
            kPos.y = pos.y;
            kPos.x = newX - kScale.x / 2;

            /* Vertical available space needs to decrease & current offset in px */
            data.shrinkBy.y = scale.y;
            data.offsetPx.x = sl->getSlideCurrentValue();
        }
    }

    /* No need to have an offset for RecycleList or TreeView as they calculate that offset separately. */
    if (parent->getType() == AbstractNode::NodeType::TREEVIEW)
    {
        data.offsetPx = {0, 0};
    }
    return data;
}

bool BasicLayoutEngine::processTreeView(const AbstractNodePtr& node)
{
    const TreeViewPtr tvPtr = Utils::as<TreeView>(node);
    if (!tvPtr)
    {
        log_.errorLn("Could not cast to TreeView: %s", node->getCName());
        return false;
    }

    TreeView::Internals& internalsRef = tvPtr->getInternalsRef();
    const int32_t rowSize = tvPtr->getRowSize();
    const glm::vec3 nScale = node->getTransform().scale;
    utils::Layout::TBLR itemMargin = tvPtr->getItemMargin();

    int32_t rowSizeAndMargin = rowSize + itemMargin.top + itemMargin.bot;
    int32_t maxDisplayAmt = nScale.y / rowSizeAndMargin + 1;
    internalsRef.topOfListIdx = tvPtr->getVBar().lock()->getSlideCurrentValue() / rowSizeAndMargin;
    internalsRef.visibleNodes = maxDisplayAmt + 1;
    if (internalsRef.isDirty || internalsRef.topOfListIdx != internalsRef.oldTopOfListIdx
        || internalsRef.oldVisibleNodes != internalsRef.visibleNodes)
    {

        internalsRef.isDirty = false;
        tvPtr->onLayoutDirtyPost();
    }

    internalsRef.oldTopOfListIdx = internalsRef.topOfListIdx;
    internalsRef.oldVisibleNodes = internalsRef.visibleNodes;
    internalsRef.lastScaleY = nScale.y;
    internalsRef.lastScaleX = nScale.x;

    //TODO: 20 magic number
    const int32_t hBarActiveSize = tvPtr->isScrollBarActive(utils::Layout::Type::HORIZONTAL) ? 20 : 0;
    const int32_t vBarActiveSize = tvPtr->isScrollBarActive(utils::Layout::Type::VERTICAL) ? 20 : 0;

    //TODO: 250 & 60 magic number
    internalsRef.overflow.x = (250+60*internalsRef.maxDepth_) - nScale.x + vBarActiveSize;
    internalsRef.overflow.y = internalsRef.flatTreeElements * rowSizeAndMargin - nScale.y + hBarActiveSize;
    internalsRef.overflow.x = std::max(0, internalsRef.overflow.x);
    internalsRef.overflow.y = std::max(0, internalsRef.overflow.y);

    tvPtr->updateOverflow(internalsRef.overflow);

    internalsRef.oldTopOfListIdx = internalsRef.topOfListIdx;
    internalsRef.oldVisibleNodes = internalsRef.visibleNodes;
    internalsRef.lastScaleY = nScale.y;
    internalsRef.lastScaleX = nScale.x;

    int32_t maxX{0};
    auto& children = node->getChildren();
    uint32_t size = children.size();
    for (uint32_t i = 0; i < size; i++)
    {
        if (children[i]->getType() == AbstractNode::NodeType::SCROLL) { continue; }
        children[i]->getTransform().pos.y -= (int32_t)tvPtr->getVBar().lock()->getSlideCurrentValue() % rowSizeAndMargin;
        children[i]->getTransform().pos.x -= tvPtr->getHBar().lock()->getSlideCurrentValue();
        maxX = std::max(maxX, (int32_t)children[i]->getTransform().scale.x);
    }

    return false;
}

void BasicLayoutEngine::processSlider(const AbstractNodePtr& node)
{
    /* Assuming parent node is Slider node, it will always have a SliderKnob child. */
    SliderPtr sliderPtr = Utils::as<Slider>(node);
    SliderKnobPtr knobPtr = Utils::as<SliderKnob>(node->getChildren()[0]);

    if (!sliderPtr || !knobPtr)
    {
        log_.errorLn("Couldn't cast to Slider or SliderKnob for %s", node->getCName());
        return;
    }

    const auto& nLayout = node->getLayout();
    const auto& nPos = sliderPtr->getTransform().pos;
    const auto& pScale = sliderPtr->getTransform().scale;
    auto& kPos = knobPtr->getTransform().pos;
    auto& kScale = knobPtr->getTransform().scale;

    /* Knob positioning */
    float sliderOffset = sliderPtr->getOffsetPerc();
    if (sliderPtr->getLayout().type == utils::Layout::Type::HORIZONTAL)
    {
        float newX = Utils::remap(sliderOffset,
            0.0f, 1.0f, nPos.x + kScale.x / 2, nPos.x + pScale.x - kScale.x / 2);
        kPos.y = nPos.y + nLayout.border.top;
        kPos.x = newX - kScale.x / 2;
    }
    else if (sliderPtr->getLayout().type == utils::Layout::Type::VERTICAL)
    {
        float newY = Utils::remap(1.0f - sliderOffset,
            0.0f, 1.0f, nPos.y + kScale.y / 2, nPos.y + pScale.y - kScale.y / 2);
        kPos.x = nPos.x + nLayout.border.left;
        kPos.y = newY - kScale.y / 2 + nLayout.border.top;
    }
}

void BasicLayoutEngine::processFloatingBox(const AbstractNodePtr& node)
{
    FloatingBoxPtr floatingBoxPtr = Utils::as<FloatingBox>(node);
    if (!floatingBoxPtr)
    {
        log_.errorLn("Couldn't cast to FloatingBox for %s", node->getCName());
        return;
    }

    const auto& preferredPos = floatingBoxPtr->getPreferredPos();
    auto& pos = floatingBoxPtr->getTransform().pos;
    pos = {preferredPos.x, preferredPos.y, pos.z};

    node->getTransform().pos.x = pos.x;
    node->getTransform().pos.y = pos.y;
    node->getChildren()[0]->getTransform().pos.x = pos.x;
    node->getChildren()[0]->getTransform().pos.y = pos.y;
}

void BasicLayoutEngine::processBoxDivider(const glm::vec2& pScale, const AbstractNodePVec& children)
{
    glm::vec2 newParentScale = pScale - getTotalChildrenAbsScale(children);
    /* See if there's any MIN not satisfied */
    glm::vec2 unsatMin = getBoxDivUnsatisfiedMinimumTotal(children, newParentScale);

    /* If there are MINs not satisfied then try to spread the overflow to the rest of the nodes that can take it
       and if there's still something remaining it means the mins cannot be satisfied with current layout. */
    for (auto& ch : children)
    {
        if (ch->getType() == AbstractNode::NodeType::BOX_DIVIDER_SEP) { continue; }
        trySpreadBoxDivUnsatMinimum(ch, newParentScale, unsatMin);
    }

    /* Adjust scale of boxes inside the divers to reflect what the user does (box separator dragging).
       Keep also boxes from going smaller than the required minimum scale. */
    for (uint32_t i = 0; i < children.size() - 1; i++)
    {
        auto& ch = children[i];
        if (ch->getType() == AbstractNode::NodeType::BOX_DIVIDER_SEP)
        {   
            /* Only handle active separator (the one the user is currently dragging) */
            BoxDividerSepPtr sep = Utils::as<BoxDividerSep>(ch);
            if (!sep->getIsActiveSeparator()) { continue; }

            auto firstBox = Utils::as<Box>(sep->getFirstBox());
            auto secondBox = Utils::as<Box>(sep->getSecondBox());

            if (sep->getLayout().type == utils::Layout::Type::HORIZONTAL)
            {
                float newL = firstBox->getLayout().tempScale.x / newParentScale.x;
                float newR = secondBox->getLayout().tempScale.x / newParentScale.x;

                if (newL + firstBox->getLayout().scale.x >= firstBox->getLayout().minScale.x / newParentScale.x &&
                    newR + secondBox->getLayout().scale.x >= secondBox->getLayout().minScale.x / newParentScale.x)
                {
                    firstBox->getLayout().scale.x += newL;
                    secondBox->getLayout().scale.x += newR;
                }

                firstBox->getLayout().tempScale.x = 0;
                secondBox->getLayout().tempScale.x = 0;
            }
            else if (sep->getLayout().type == utils::Layout::Type::VERTICAL)
            {
                float newT = firstBox->getLayout().tempScale.y / pScale.y;
                float newB = secondBox->getLayout().tempScale.y / pScale.y;

                if (newT + firstBox->getLayout().scale.y >= firstBox->getLayout().minScale.y / newParentScale.y &&
                    newB + secondBox->getLayout().scale.y >= secondBox->getLayout().minScale.y / newParentScale.y)
                {
                    firstBox->getLayout().scale.y += newT;
                    secondBox->getLayout().scale.y += newB;
                }

                firstBox->getLayout().tempScale.y = 0;
                secondBox->getLayout().tempScale.y = 0;
            }
        }
    }
}

void BasicLayoutEngine::processDropdown(const AbstractNodePtr& node)
{
    /* Don't compute anything if the dropdown isn't even visible. */
    DropdownPtr dd = Utils::as<Dropdown>(node);
    if (!dd->isDropdownOpen()) { return; }
    
    auto& children = node->getChildren();
    if (!children.size()) { return; }

    /* Figure out how big the elements are on the vertical axis in order to set a size for the dropdown. */
    float boxContElementsRollingY{0};
    float maxX{0};
    auto& container = children[0];
    for (const auto& ch : container->getChildren())
    {
        const auto& chLayout = ch->getLayout();
        boxContElementsRollingY += chLayout.scale.y + chLayout.margin.top + chLayout.margin.bot;
        maxX = std::max(maxX, chLayout.scale.x + chLayout.margin.left + chLayout.margin.right);
    }

    /* Compute and sets it's scale (for the dropdown items container). */
    auto& chZeroLayout = container->getLayout();
    chZeroLayout.scale = {
        maxX + chZeroLayout.padding.left + chZeroLayout.padding.right +
            chZeroLayout.border.left + chZeroLayout.border.right +
            chZeroLayout.margin.left + chZeroLayout.margin.right,
        boxContElementsRollingY + chZeroLayout.padding.top + chZeroLayout.padding.bot +
            chZeroLayout.border.top + chZeroLayout.border.bot
    };
    auto& contBoxScale = container->getTransform().scale;
    contBoxScale.x = chZeroLayout.scale.x;
    contBoxScale.y = chZeroLayout.scale.y;

    /* Try to find a location to fit the dropdown in relationship to the parent aka try to see if it can
        be positioned to the bottom, right, top, left. */
    auto& nShrink = node->getLayout().shrink;
    auto& nPos = node->getTransform().pos;
    auto& nScale = node->getTransform().scale;
    auto& contBoxPos = container->getTransform().pos;

    /* Try to fit in the user preferred position relative to the parent as long as the dropdown would not
        exit the screen. */
    const auto& frameState = node->getState();
    Dropdown::Expand dir = dd->getExpandDirection();

    const bool topCond = nPos.y - contBoxScale.y >= 0;
    const bool botCond = frameState->frameSize.y > nPos.y + nScale.y + contBoxScale.y;
    const bool leftCond = nPos.x - contBoxScale.x >= 0;
    const bool rightCond = frameState->frameSize.x > nPos.x + contBoxScale.x;
    switch (dir)
    {
        case Dropdown::Expand::LEFT:
            if (leftCond)
            {
                contBoxPos.x = nPos.x - contBoxScale.x;
                contBoxPos.y = nPos.y;
            }
            else if (rightCond)
            {
                contBoxPos.x = nPos.x + nScale.x;
                contBoxPos.y = nPos.y;
            }
            break;
        case Dropdown::Expand::RIGHT:
            if (rightCond)
            {
                contBoxPos.x = nPos.x + nScale.x;
                contBoxPos.y = nPos.y;
                break;
            }
            else if (leftCond)
            {
                contBoxPos.x = nPos.x - contBoxScale.x;
                contBoxPos.y = nPos.y;
            }
            break;
        case Dropdown::Expand::TOP:
            if (topCond)
            {
                contBoxPos.y = nPos.y - contBoxScale.y - nShrink.y;
                if (rightCond)
                {
                    contBoxPos.x = nPos.x - nShrink.x;
                }
                else if (leftCond)
                {
                    contBoxPos.x = nPos.x + nScale.x - contBoxScale.x;
                }
                break;
            }
            break;
        case Dropdown::Expand::BOTTOM:
            if (botCond)
            {
                contBoxPos.y = nPos.y + nScale.y + nShrink.y;
                if (rightCond)
                {
                    contBoxPos.x = nPos.x - nShrink.x;
                }
                else if (leftCond)
                {
                    contBoxPos.x = nPos.x + nScale.x - contBoxScale.x;
                }
            }
            break;
        default:
            log_.warnLn("Unknown expand direction: ENUM(%d)", static_cast<uint8_t>(dir));
    }
}

void BasicLayoutEngine::processGridLayout(const glm::vec2& pScale, const AbstractNodePtr& parent)
{
    const AbstractNodePVec& children = parent->getChildren();
    utils::Layout::GridDistribRC& gridDistribRC = parent->getLayout().gridDistrib;

    /* Calculate total frac/abs */
    glm::ivec2 totalAbs{0, 0};
    glm::ivec2 totalFrac{0, 0};
    for (const auto& distribData : gridDistribRC.cols)
    {
        if (distribData.type == utils::Layout::GridDistrib::ABS)
        {
            totalAbs.x += distribData.value;
        }
        else if (distribData.type == utils::Layout::GridDistrib::FRAC)
        {
            totalFrac.x += distribData.value;
        }
    }

    for (const auto& distribData : gridDistribRC.rows)
    {
        if (distribData.type == utils::Layout::GridDistrib::ABS)
        {
            totalAbs.y += distribData.value;
        }
        else if (distribData.type == utils::Layout::GridDistrib::FRAC)
        {
            totalFrac.y += distribData.value;
        }
    }

    float wPerFrac = (pScale.x - totalAbs.x) / totalFrac.x;
    float hPerFrac = (pScale.y - totalAbs.y) / totalFrac.y;

    float rollingX{0};
    for (auto& distribData : gridDistribRC.cols)
    {
        distribData.computedStart = rollingX;

        if (distribData.type == utils::Layout::GridDistrib::Type::FRAC)
        {
            rollingX += distribData.value * wPerFrac;
        }
        else if (distribData.type == utils::Layout::GridDistrib::Type::ABS)
        {
            rollingX += distribData.value;
        }
    }

    float rollingY{0};
    for (auto& distribData : gridDistribRC.rows)
    {
        distribData.computedStart = rollingY;

        if (distribData.type == utils::Layout::GridDistrib::Type::FRAC)
        {
            rollingY += distribData.value * hPerFrac;
        }
        else if (distribData.type == utils::Layout::GridDistrib::Type::ABS)
        {
            rollingY += distribData.value;
        }
    }

    int32_t colsCount = gridDistribRC.cols.size();
    int32_t rowsCount = gridDistribRC.rows.size();
    const auto& pPadding = parent->getLayout().padding;
    const auto& pPos = parent->getTransform().pos;
    for (auto ch : children)
    {
        IGNORE_SCROLLBAR;
        IGNORE_FLOATING_BOX;

        auto& pos = ch->getTransform().pos;
        auto& scale = ch->getTransform().scale;
        const auto& chLayout = ch->getLayout();
        auto& gridStart = ch->getLayout().gridStartRC;

        /* Nodes positioning */
        if ((gridStart.col >= 0 && gridStart.col < colsCount) &&
            (gridStart.row >= 0 && gridStart.row < rowsCount))
        {
            pos.x = pPos.x + gridDistribRC.cols[gridStart.col].computedStart + pPadding.left + chLayout.margin.left;
            pos.y = pPos.y + gridDistribRC.rows[gridStart.row].computedStart + pPadding.top + chLayout.margin.top;
        }
        else
        {
            // TODO: Out of bounds indexing, should notify user somehow
            log_.infoLn("Out of bounds gridStart+Span. Shorting.");
            return;
        }

        /* Nodes scaling */
        glm::vec2 leftover{0, 0};
        float startX = gridDistribRC.cols[gridStart.col].computedStart + chLayout.margin.left;
        float startY = gridDistribRC.rows[gridStart.row].computedStart + chLayout.margin.top;
        float endX = gridStart.col + 1 < colsCount ? gridDistribRC.cols[gridStart.col + 1].computedStart : pScale.x;
        float endY = gridStart.row + 1 < rowsCount ? gridDistribRC.rows[gridStart.row + 1].computedStart : pScale.y;
        endX -= chLayout.margin.right;
        endY -= chLayout.margin.bot;
        if (chLayout.scaleType.x == utils::Layout::ScaleType::PX)
        {
            scale.x = chLayout.scale.x;
            leftover.x = (endX - startX) - scale.x;
        }
        else if (chLayout.scaleType.x == utils::Layout::ScaleType::REL)
        {
            scale.x = endX - startX;
            leftover.x = scale.x;
            scale.x *= chLayout.scale.x;
            leftover.x -= scale.x;
        }

        if (chLayout.scaleType.y == utils::Layout::ScaleType::PX)
        {
            scale.y = chLayout.scale.y;
            leftover.y = (endY - startY) - scale.y;
        }
        else if (chLayout.scaleType.y == utils::Layout::ScaleType::REL)
        {
            scale.y = endY - startY;
            leftover.y = scale.y;
            scale.y *= chLayout.scale.y;
            leftover.y -= scale.y;
        }

        /* Self Alignment */
        switch (chLayout.alignSelf)
        {
            IGNORE_LR_ALIGN
            IGNORE_TB_ALIGN
            case utils::Layout::CENTER:
                pos.x += leftover.x * 0.5f;
                pos.y += leftover.y * 0.5f;
                break;
            case utils::Layout::TOP_LEFT:
                break; /* Nothing to do */
            case utils::Layout::TOP_RIGHT:
                pos.x += leftover.x;
                break;
            case utils::Layout::CENTER_LEFT:
                pos.y += leftover.y * 0.5f;
                break;
            case utils::Layout::CENTER_RIGHT:
                pos.x += leftover.x;
                pos.y += leftover.y * 0.5f;
                break;
            case utils::Layout::CENTER_TOP:
                pos.x += leftover.x * 0.5f;
                break;
            case utils::Layout::CENTER_BOTTOM:
                pos.x += leftover.x * 0.5f;
                pos.y += leftover.y;
                break;
            case utils::Layout::BOTTOM_LEFT:
                pos.y += leftover.y;
                break;
            case utils::Layout::BOTTOM_RIGHT:
                pos.x += leftover.x;
                pos.y += leftover.y;
                break;
            default:
                log_.warnLn("Unrecognized grid alignSelf value: ENUM(%d)",
                    static_cast<uint8_t>(chLayout.alignSelf));
        }
    }
}

void BasicLayoutEngine::processHVLayout(const AbstractNodePtr& node, const glm::vec2 nScale)
{
    const auto& children = node->getChildren();
    const utils::Layout& layout = node->getLayout();

    /* Compute real (not planned) total scale of children */
    glm::vec2 totalChildSize = computeTotalRealNodesScale(children);

    /* Compute ZERO relative position of objects PASS */
    float startX{0};
    float startY{0};
    int32_t startIdx{0};
    int32_t endIdx{0};
    if (layout.type == utils::Layout::Type::HORIZONTAL)
    {
        /* Dealing with layout spacing */
        const glm::vec2 spacing = computeSpacing(node, nScale.x, totalChildSize.x);
        startX += spacing.y; /* additionalPush */

        float rollingX = startX;
        float maxY{0};
        for (auto& ch : children)
        {
            /* Already calculated, skip */
            IGNORE_SCROLLBAR;
            IGNORE_FLOATING_BOX;

            const utils::Layout& chLayout = ch->getLayout();
            auto& pos = ch->getTransform().pos;
            auto& scale = ch->getTransform().scale;
            const float lrMargin = chLayout.margin.left + chLayout.margin.right;
            if (layout.allowWrap)
            {
                if (rollingX + scale.x + lrMargin > nScale.x)
                {
                    startX = 0;
                    startY += maxY;
                    rollingX = 0;

                    /* Align Self (2nd zero positioning) */
                    resolveAlignSelf(children, startIdx, endIdx, maxY, utils::Layout::Type::HORIZONTAL);

                    maxY = 0;
                    startIdx = endIdx;
                }
                rollingX += scale.x + lrMargin;
            }
            maxY = std::max(maxY, scale.y + chLayout.margin.top + chLayout.margin.bot);
            endIdx++;

            /* 1st zero positioning */
            pos.x = startX + chLayout.margin.left;
            pos.y = startY + chLayout.margin.top;
            startX += scale.x + spacing.x + lrMargin;
        }

        /* Align Self (2nd zero positioning) */
        resolveAlignSelf(children, startIdx, endIdx, maxY, utils::Layout::Type::HORIZONTAL);
    }
    else if (layout.type == utils::Layout::Type::VERTICAL)
    {
        /* Dealing with layout spacing */
        const glm::vec2 spacing = computeSpacing(node, nScale.y, totalChildSize.y);
        startY += spacing.y; /* additionalPush */

        float rollingY = startY;
        float maxX{0};
        for (auto& ch : children)
        {
            /* Already calculated, skip */
            IGNORE_SCROLLBAR;
            IGNORE_FLOATING_BOX;

            const utils::Layout& chLayout = ch->getLayout();
            auto& pos = ch->getTransform().pos;
            auto& scale = ch->getTransform().scale;
            const float tbMargin = chLayout.margin.top + chLayout.margin.bot;
            if (layout.allowWrap)
            {
                if (rollingY + scale.y + tbMargin > nScale.y)
                {
                    startX += maxX;
                    startY = 0;
                    rollingY = 0;

                    /* Align Self (2nd zero positioning) */
                    resolveAlignSelf(children, startIdx, endIdx, maxX, utils::Layout::Type::VERTICAL);

                    maxX = 0;
                    startIdx = endIdx;
                }
                rollingY += scale.y + tbMargin;
            }
            maxX = std::max(maxX, scale.x + chLayout.margin.left + chLayout.margin.right);
            endIdx++;

            /* 1st zero positioning */
            pos.x = startX + chLayout.margin.left;
            pos.y = startY + chLayout.margin.top;
            startY += scale.y + spacing.x + tbMargin;
        }
        /* Align Self (2nd zero positioning) */
        resolveAlignSelf(children, startIdx, endIdx, maxX, utils::Layout::Type::VERTICAL);
    }
}

#undef IGNORE_SCROLLBAR
#undef IGNORE_FLOATING_BOX
#undef IGNORE_GRID_ALIGN
#undef IGNORE_LR_ALIGN
#undef IGNORE_TB_ALIGN
} // namespace msgui

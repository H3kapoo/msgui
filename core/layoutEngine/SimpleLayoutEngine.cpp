#include "SimpleLayoutEngine.hpp"

#include "core/node/AbstractNode.hpp"
#include "core/node/Box.hpp"
#include "core/node/Slider.hpp"
#include "core/node/utils/BoxDividerSep.hpp"
#include "core/node/utils/LayoutData.hpp"
#include "core/node/utils/ScrollBar.hpp"
#include "core/node/utils/SliderKnob.hpp"
#include <cmath>

namespace msgui
{
#define IGNORE_GRID_ALIGN\
    case Layout::TOP_LEFT:\
    case Layout::TOP_RIGHT:\
    case Layout::CENTER_LEFT:\
    case Layout::CENTER_RIGHT:\
    case Layout::BOTTOM_LEFT:\
    case Layout::BOTTOM_RIGHT:\
        break;\

#define IGNORE_LR_ALIGN\
    case Layout::LEFT:\
    case Layout::RIGHT:\
        break;\

#define IGNORE_TB_ALIGN\
    case Layout::TOP:\
    case Layout::BOTTOM:\
        break;\

#define IGNORE_SCROLLBAR\
    if (ch->getType() == AbstractNode::NodeType::SCROLL ||\
        ch->getType() == AbstractNode::NodeType::SCROLL_KNOB)\
    { continue; }\


glm::vec2 SimpleLayoutEngine::process(const AbstractNodePtr& parent)
{
    const AbstractNodePVec& children = parent->getChildren();
    if (children.empty()) { return {0, 0}; }

    const Layout& layout = parent->getLayout();
    if (layout.type == Layout::Type::GRID)
    {
        processGridLayout(parent);
        return {0, 0};
    }

    // Compute SCROLLBARS position PASS
    ScrollBarsData scrollNodeData = processScrollbars(parent);

    // Useless to compute further if parent is a scroll node (knob computed already)
    if (parent->getType() == AbstractNode::NodeType::SCROLL) { return {0, 0}; }

    auto& pPos = parent->getTransform().pos;
    glm::vec3 pScale = parent->getTransform().scale;
    pScale.x -= scrollNodeData.shrinkBy.x + layout.padding.left + layout.padding.right
        + layout.border.left + layout.border.right;
    pScale.y -= scrollNodeData.shrinkBy.y + layout.padding.top + layout.padding.bot
        + layout.border.bot + layout.border.top;

    // TODO: This needs to be combined as much as possible with computeNodeScale
    // Compute box divider shit
    if (parent->getType() == AbstractNode::NodeType::BOX_DIVIDER)
    {
        processBoxDivider(pScale, children);
    }
    else
    {
        // Compute node scale (if needed)
        computeNodeScale(pScale, children);
    }

    // Compute sliders; no overflow is gonna be generated
    if (parent->getType() == AbstractNode::NodeType::SLIDER)
    {
        processSlider(parent);
        return {0, 0};
    }

    // Compute total scale of children
    glm::vec2 totalChildSize{0, 0};
    for (auto& ch : children)
    {
        IGNORE_SCROLLBAR

        const Layout& chLayout = ch->getLayout();
        totalChildSize.x += ch->getTransform().scale.x + (chLayout.margin.left + chLayout.margin.right);
        totalChildSize.y += ch->getTransform().scale.y + (chLayout.margin.top + chLayout.margin.bot);
    }

    // Compute ZERO relative position of objects PASS
    float startX{0};
    float startY{0};
    int32_t startIdx{0};
    int32_t endIdx{0};
    if (layout.type == Layout::Type::HORIZONTAL)
    {
        // Dealing with layout Spacing
        float spacing{0};
        switch (layout.spacing)
        {
            case Layout::TIGHT:
                // Do nothing
                break;
            case Layout::EVEN_WITH_NO_START_GAP:
                spacing = (pScale.x - totalChildSize.x) / (children.size() - 1);
                spacing = std::max(0.0f, spacing);
                break;
            case Layout::EVEN_WITH_START_GAP:
                spacing = (pScale.x - totalChildSize.x) / (children.size());
                spacing = std::max(0.0f, spacing);
                startX += spacing * 0.5f;
                break;
            default:
                log_.warnLn("Unrecognized Spacing value: ENUM(%d)",
                    static_cast<uint8_t>(layout.spacing));
        }

        float rollingX = startX;
        float maxY{0};
        for (auto& ch : children)
        {
            // Already calculated, skip
            IGNORE_SCROLLBAR

            const Layout& chLayout = ch->getLayout();
            auto& pos = ch->getTransform().pos;
            auto& scale = ch->getTransform().scale;
            if (layout.allowWrap)
            {
                if (rollingX + scale.x + chLayout.margin.left + chLayout.margin.right > pScale.x)
                {
                    startX = 0;
                    startY += maxY;
                    rollingX = 0;

                    // Align Self (2nd zero positioning)
                    resolveAlignSelf(children, startIdx, endIdx, maxY, Layout::Type::HORIZONTAL);

                    maxY = 0;
                    startIdx = endIdx;
                }
                rollingX += scale.x + chLayout.margin.left + chLayout.margin.right;
            }
            maxY = std::max(maxY, scale.y + chLayout.margin.top + chLayout.margin.bot);
            endIdx++;
            // 1st zero positioning
            pos.x = startX + chLayout.margin.left;
            pos.y = startY + chLayout.margin.top;
            startX += scale.x + spacing + (chLayout.margin.left + chLayout.margin.right);
        }

        // Align Self (2nd zero positioning)
        resolveAlignSelf(children, startIdx, endIdx, maxY, Layout::Type::HORIZONTAL);
    }
    else if (layout.type == Layout::Type::VERTICAL)
    {
        // Dealing with layout Spacing
        float spacing{0};
        switch (layout.spacing)
        {
            case Layout::TIGHT:
                // Do nothing
                break;
            case Layout::EVEN_WITH_NO_START_GAP:
                //TODO: Divide by zero in case we have just a child!
                spacing = (pScale.y - totalChildSize.y) / (children.size() - 1);
                spacing = std::max(0.0f, spacing);
                break;
            case Layout::EVEN_WITH_START_GAP:
                spacing = (pScale.y - totalChildSize.y) / (children.size());
                spacing = std::max(0.0f, spacing);
                startY += spacing * 0.5f;
                break;
            default:
                log_.warnLn("Unrecognized Spacing value: ENUM(%d)",
                    static_cast<uint8_t>(layout.spacing));
        }

        float rollingY = startY;
        float maxX{0};
        for (auto& ch : children)
        {
            // Already calculated, skip
            IGNORE_SCROLLBAR

            const Layout& chLayout = ch->getLayout();
            auto& pos = ch->getTransform().pos;
            auto& scale = ch->getTransform().scale;
            if (layout.allowWrap)
            {
                if (rollingY + scale.y + chLayout.margin.top + chLayout.margin.bot > pScale.y)
                {
                    startX += maxX;
                    startY = 0;
                    rollingY = 0;

                    // Align Self (2nd zero positioning)
                    resolveAlignSelf(children, startIdx, endIdx, maxX, Layout::Type::VERTICAL);

                    maxX = 0;
                    startIdx = endIdx;
                }
                rollingY += scale.y + chLayout.margin.top + chLayout.margin.bot;
            }
            maxX = std::max(maxX, scale.x + chLayout.margin.left + chLayout.margin.right);
            endIdx++;
            // 1st zero positioning
            pos.x = startX + chLayout.margin.left;
            pos.y = startY + chLayout.margin.top;
            startY += scale.y + spacing + (chLayout.margin.top + chLayout.margin.bot);
        }
        // Align Self (2nd zero positioning)
        resolveAlignSelf(children, startIdx, endIdx, maxX, Layout::Type::VERTICAL);
    }

    // Compute children overflow PASS
    const glm::vec2 computedOverflow = computeOverflow(pScale, children);

    // Apply SCROLLBAR offsets + any group offseting from ZERO PASS
    //TODO: Maybe RecycleList will not come through here?
    for (auto& ch : children)
    {
        // Already calculated, skip
        IGNORE_SCROLLBAR

        auto& pos = ch->getTransform().pos;
        pos.x += -scrollNodeData.offsetPx.x + pPos.x + layout.padding.left + layout.border.left;
        pos.y += -scrollNodeData.offsetPx.y + pPos.y + layout.padding.top + layout.border.top;
        // AlignChild
        // Negative overflow means we still have X amount of pixels until the parent is full on that axis
        // We can leverage this to position elements top, left, right, bot, center.
        if (computedOverflow.x < 0)
        {
            switch (layout.alignChild.x)
            {
                IGNORE_GRID_ALIGN
                IGNORE_TB_ALIGN
                case Layout::LEFT:
                    // Do nothing
                    break;
                case Layout::CENTER:
                    pos.x += -computedOverflow.x * 0.5f;
                    break;
                case Layout::RIGHT:
                    pos.x += -computedOverflow.x;
                    break;
                default:
                    log_.warnLn("Unrecognized alignChildX value: ENUM(%d)",
                        static_cast<uint8_t>(layout.alignChild.x));
            }
        }

        if (computedOverflow.y < 0)
        {
            switch (layout.alignChild.y)
            {
                IGNORE_GRID_ALIGN
                IGNORE_LR_ALIGN
                case Layout::TOP:
                    // Do nothing
                    break;
                case Layout::CENTER:
                    pos.y += -computedOverflow.y * 0.5f;
                    break;
                case Layout::BOTTOM:
                    pos.y += -computedOverflow.y;
                    break;
                default:
                    log_.warnLn("Unrecognized alignChildY value: ENUM(%d)",
                        static_cast<uint8_t>(layout.alignChild.y));
            }
        }
    }

    return computedOverflow;
}

void SimpleLayoutEngine::computeNodeScale(const glm::vec2& pScale, const AbstractNodePVec& children)
{
    auto getChildrenAbsTotal = [this](const AbstractNodePVec& children2) -> glm::vec2
    {
        glm::vec2 totalAbsChildSize{0};
        for (auto& ch : children2)
        {
            IGNORE_SCROLLBAR

            const Layout& chLayout = ch->getLayout();
            if (chLayout.scaleType.x == Layout::ScaleType::ABS)
            {
                totalAbsChildSize.x += chLayout.scale.x;
            }

            if (chLayout.scaleType.y == Layout::ScaleType::ABS)
            {
                totalAbsChildSize.y += chLayout.scale.y;
            }
        }
        return totalAbsChildSize;
    };

    glm::vec2 newParentScale = pScale - getChildrenAbsTotal(children);
    for (auto& ch : children)
    {
        // Shall not be taken into consideration as these are calculated differently
        IGNORE_SCROLLBAR

        const Layout& chLayout = ch->getLayout();
        if (chLayout.scaleType.x == Layout::ScaleType::REL)
        {
            auto& scale = ch->getTransform().scale;
            scale.x = newParentScale.x * chLayout.scale.x;
            scale.x -= chLayout.margin.left + chLayout.margin.right;
        }

        if (chLayout.scaleType.y == Layout::ScaleType::REL)
        {
            auto& scale = ch->getTransform().scale;
            scale.y = newParentScale.y * chLayout.scale.y;
            scale.y -= chLayout.margin.top + chLayout.margin.bot;
        }

        if (chLayout.scaleType.x == Layout::ScaleType::ABS)
        {
            auto& scale = ch->getTransform().scale;
            scale.x = chLayout.scale.x;
        }

        if (chLayout.scaleType.y == Layout::ScaleType::ABS)
        {
            auto& scale = ch->getTransform().scale;
            scale.y = chLayout.scale.y;
        }
    }
}

void SimpleLayoutEngine::resolveAlignSelf(const AbstractNodePVec& children, const uint32_t idxStart,
    const uint32_t idxEnd, const int32_t max, const Layout::Type type)
{
    for (uint32_t i = idxStart; i < idxEnd; i++)
    {
        const Layout& chLayout = children[i]->getLayout();
        auto& pos = children[i]->getTransform().pos;
        auto& scale = children[i]->getTransform().scale;

        if (type == Layout::Type::HORIZONTAL)
        {
            switch (chLayout.alignSelf)
            {
                IGNORE_GRID_ALIGN
                IGNORE_LR_ALIGN
                case Layout::TOP:
                    // Do nothing
                    break;
                case Layout::CENTER:
                    pos.y += (max - scale.y - chLayout.margin.bot) * 0.5f;
                    break;
                case Layout::BOTTOM:
                    pos.y += max - scale.y - chLayout.margin.bot;
                    break;
                default:
                    log_.warnLn("Unrecognized horizontal alignSelf value: ENUM(%d)",
                        static_cast<uint8_t>(chLayout.alignSelf));
            }
        }
        else if (type == Layout::Type::VERTICAL)
        {
            switch (chLayout.alignSelf)
            {
                IGNORE_GRID_ALIGN
                IGNORE_TB_ALIGN
                case Layout::LEFT:
                    // Do nothing
                    break;
                case Layout::CENTER:
                    pos.x += (max - scale.x - chLayout.margin.right) * 0.5f;
                    break;
                case Layout::RIGHT:
                    pos.x += max - scale.x - chLayout.margin.right;
                    break;
                default:
                    log_.warnLn("Unrecognized vertical alignSelf value: ENUM(%d)",
                        static_cast<uint8_t>(chLayout.alignSelf));
            }
        }
    }
}

glm::vec2 SimpleLayoutEngine::computeOverflow(const glm::vec2& pScale, const AbstractNodePVec& children)
{
    glm::vec2 currentScale{0, 0};
    for (auto& ch : children)
    {
        // Shall not be taken into consideration for overflow
        if (ch->getType() == AbstractNode::NodeType::SCROLL ||
            ch->getType() == AbstractNode::NodeType::SCROLL_KNOB)
        { continue; }

        const Layout& chLayout = ch->getLayout();
        auto scale = ch->getTransform().scale;
        auto pos = ch->getTransform().pos;
        scale.x += chLayout.margin.right;
        scale.y += chLayout.margin.bot;
        currentScale.x = std::max(currentScale.x, pos.x + scale.x);
        currentScale.y = std::max(currentScale.y, pos.y + scale.y);
    }

    return {currentScale.x - pScale.x, currentScale.y - pScale.y};
}

SimpleLayoutEngine::ScrollBarsData SimpleLayoutEngine::processScrollbars(const AbstractNodePtr& parent)
{
    const AbstractNodePVec& children = parent->getChildren();
    if (children.empty())
    {
        return ScrollBarsData{};
    }

    const Layout& layout = parent->getLayout();

    // Return by how much should the parent "shrink" to fit scrollbars
    ScrollBarsData data;

    bool bothSbOn{false};
    if (parent->getType() == AbstractNode::NodeType::BOX)
    {
        Box* castBox = static_cast<Box*>(parent.get());
        if (castBox->isScrollBarActive(ScrollBar::Orientation::NONE))
        {
            return ScrollBarsData{};
        }

        if (castBox->isScrollBarActive(ScrollBar::Orientation::ALL))
        {
            bothSbOn = true;
        }
    }

    auto& pPos = parent->getTransform().pos;
    auto& pScale = parent->getTransform().scale;

    // Dealing with scrollbars, if any
    for (auto& ch : children)
    {   
        // Ignore non-Scrollbar elements
        if (ch->getType() != AbstractNode::NodeType::SCROLL) { continue; }

        ScrollBar* sb = static_cast<ScrollBar*>(ch.get());
        if (!sb)
        {
            log_.errorLn("Failed to cast to ScrollBar!");
        }

        auto& pos = ch->getTransform().pos;
        auto& scale = ch->getTransform().scale;

        if (sb->getOrientation() == ScrollBar::Orientation::VERTICAL)
        {
            // Scrollbar positioning
            pos.x = pPos.x + pScale.x - scale.x - layout.border.right;
            pos.y = pPos.y + layout.border.top;
            scale.y = pScale.y - (bothSbOn ? scale.x : 0)
             - (layout.border.top + layout.border.bot);

            // Knob positioning
            AbstractNodePtr knob = sb->getChildren()[0]; // Always exists
            auto& kScale = knob->getTransform().scale;
            auto& kPos = knob->getTransform().pos;
            float sbOffset = sb->getKnobOffset();

            int32_t sbOverflowSize = sb->getOverflowSize();
            kScale.x = scale.x;
            kScale.y = scale.y - sbOverflowSize;
            kScale.y = std::max(scale.x, kScale.y);

            float newY = Utils::remap(sbOffset,
                0.0f, 1.0f, pos.y + kScale.y / 2, pos.y + scale.y - kScale.y / 2);
            kPos.x = pos.x;
            kPos.y = newY - kScale.y / 2;

            // Horizontal available space needs to decrease & current offset in px
            data.shrinkBy.x = scale.x;
            data.offsetPx.y = sb->geOverflowOffset();
        }
        else if (sb->getOrientation() == ScrollBar::Orientation::HORIZONTAL)
        {
            // Scrollbar positioning
            pos.y = pPos.y + pScale.y - scale.y - layout.border.bot;
            pos.x = pPos.x + layout.border.left;
            scale.x = pScale.x - (bothSbOn ? scale.y : 0)
                - (layout.border.left + layout.border.right);

            // Knob positioning
            AbstractNodePtr knob = sb->getChildren()[0]; // Always exists
            auto& kScale = knob->getTransform().scale;
            auto& kPos = knob->getTransform().pos;
            float sbOffset = sb->getKnobOffset();

            int32_t sbOverflowSize = sb->getOverflowSize();
            kScale.x = scale.x - sbOverflowSize;
            kScale.x = std::max(scale.y, kScale.x);
            kScale.y = scale.y;

            float newX = Utils::remap(sbOffset,
                0.0f, 1.0f, pos.x + kScale.x / 2, pos.x + scale.x - kScale.x / 2);
            kPos.y = pos.y;
            kPos.x = newX - kScale.x / 2;

            // Vertical available space needs to decrease & current offset in px
            data.shrinkBy.y = scale.y;
            data.offsetPx.x = sb->geOverflowOffset();
        }
    }

    return data;
}

void SimpleLayoutEngine::processSlider(const AbstractNodePtr& parent)
{
    // Assuming parent node is Slider node, it will always have a SliderKnob child.
    Slider* sliderRawPtr = static_cast<Slider*>(parent.get());
    SliderKnob* knobRawPtr = static_cast<SliderKnob*>(parent->getChildren()[0].get());

    if (!sliderRawPtr || !knobRawPtr)
    {
        log_.errorLn("Couldn't cast to Slider or SliderKnob for %s", parent->getCName());
        return;
    }

    auto& pPos = sliderRawPtr->getTransform().pos;
    auto& pScale = sliderRawPtr->getTransform().scale;
    auto& kPos = knobRawPtr->getTransform().pos;
    auto& kScale = knobRawPtr->getTransform().scale;

    // Knob positioning
    float sliderOffset = sliderRawPtr->getOffsetPerc();
    if (sliderRawPtr->getLayout().type == Layout::Type::HORIZONTAL)
    {
        float newX = Utils::remap(sliderOffset,
            0.0f, 1.0f, pPos.x + kScale.x / 2, pPos.x + pScale.x - kScale.x / 2);
        kPos.y = pPos.y;
        kPos.x = newX - kScale.x / 2;
    }
    else if (sliderRawPtr->getLayout().type == Layout::Type::VERTICAL)
    {
        float newY = Utils::remap(sliderOffset,
            0.0f, 1.0f, pPos.y + kScale.y / 2, pPos.y + pScale.y - kScale.y / 2);
        kPos.x = pPos.x;
        kPos.y = newY - kScale.y / 2;
    }
}

void SimpleLayoutEngine::processBoxDivider(const glm::vec2& pScale, const AbstractNodePVec& children)
{
    auto getChildrenAbsTotal = [this](const AbstractNodePVec& children2) -> glm::vec2
    {
        glm::vec2 totalAbsChildSize{0};
        for (auto& ch : children2)
        {
            IGNORE_SCROLLBAR

            const Layout& chLayout = ch->getLayout();
            if (chLayout.scaleType.x == Layout::ScaleType::ABS)
            {
                totalAbsChildSize.x += chLayout.scale.x;
            }

            if (chLayout.scaleType.y == Layout::ScaleType::ABS)
            {
                totalAbsChildSize.y += chLayout.scale.y;
            }
        }
        return totalAbsChildSize;
    };

    auto getUnsatMin = [this](const AbstractNodePVec& children2, const glm::vec2 newParentScale) -> glm::vec2
    {
        glm::vec2 runningMinOverflow{0, 0};
        for (auto& ch : children2)
        {
            if (ch->getType() == AbstractNode::NodeType::BOX_DIVIDER_SEP) { continue; }

            Layout& chLayout = ch->getLayout();
            float newMinX = Utils::remap(chLayout.minScale.x, 0, newParentScale.x, 0.0f, 1.0f);
            if (newMinX - chLayout.scale.x > 0)
            {
                runningMinOverflow.x += newMinX - chLayout.scale.x;
            }
            chLayout.scale.x = std::max(chLayout.scale.x, newMinX);

            float newMinY = Utils::remap(chLayout.minScale.y, 0, newParentScale.y, 0.0f, 1.0f);
            if (newMinY - chLayout.scale.y > 0)
            {
                runningMinOverflow.y += newMinY - chLayout.scale.y;
            }
            chLayout.scale.y = std::max(chLayout.scale.y, newMinY);
        }
        return runningMinOverflow;
    };

    auto trySpreadUnsatMin = [this](Layout& chLayout, const glm::vec2& newParentScale, glm::vec2& unsatMin) -> void
    {
        if (unsatMin.x > 0.01f)
        {
            float newMinX = Utils::remap(chLayout.minScale.x, 0, newParentScale.x, 0.0f, 1.0f);
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
            float newMinY = Utils::remap(chLayout.minScale.y, 0, newParentScale.y, 0.0f, 1.0f);
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
    };

    glm::vec2 newParentScale = pScale - getChildrenAbsTotal(children);
    // see if there's any MIN not satisfied
    glm::vec2 unsatMin = getUnsatMin(children, newParentScale);

    // there are MINs not satisfied; try to spread the overflow to the rest of the nodes that can take it
    // if there's still something in runningX it means the mins cannot be satisfied with current layout;
    // error out, notify user, idk
    for (auto& ch : children)
    {
        if (ch->getType() == AbstractNode::NodeType::BOX_DIVIDER_SEP) { continue; }

        Layout& chLayout = ch->getLayout();
        trySpreadUnsatMin(chLayout, newParentScale, unsatMin);
    }

    // Keep mins in check
    // Adjust scale of boxes inside the divers to reflect what the user does (box separator dragging).
    // Keep also boxes from going smaller than the required minimum scale.
    for (uint32_t i = 0; i < children.size() - 1; i++)
    {
        auto& ch = children[i];
        if (ch->getType() == AbstractNode::NodeType::BOX_DIVIDER_SEP)
        {   
            // Only handle active separator (the one the user is currently dragging)
            BoxDividerSep* sep = static_cast<BoxDividerSep*>(ch.get());
            if (!sep->getIsActiveSeparator()) { continue; }

            auto firstBox = static_cast<Box*>(sep->getFirstBox().get());
            auto secondBox = static_cast<Box*>(sep->getSecondBox().get());

            if (sep->getLayout().type == Layout::Type::HORIZONTAL)
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
            else if (sep->getLayout().type == Layout::Type::VERTICAL)
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

    // Normal scaling; maybe can be combined with the default scaling?
    for (auto& ch : children)
    {
        const Layout& chLayout = ch->getLayout();
        if (chLayout.scaleType.x == Layout::ScaleType::REL)
        {
            auto& scale = ch->getTransform().scale;
            scale.x = newParentScale.x * chLayout.scale.x;
            scale.x -= chLayout.margin.left + chLayout.margin.right;
            scale.x = std::round(scale.x);
        }

        if (chLayout.scaleType.y == Layout::ScaleType::REL)
        {
            auto& scale = ch->getTransform().scale;
            scale.y = newParentScale.y * chLayout.scale.y;
            scale.y -= chLayout.margin.top + chLayout.margin.bot;
            scale.y = std::round(scale.y);
        }

        if (chLayout.scaleType.x == Layout::ScaleType::ABS)
        {
            auto& scale = ch->getTransform().scale;
            scale.x = chLayout.scale.x;
        }

        if (chLayout.scaleType.y == Layout::ScaleType::ABS)
        {
            auto& scale = ch->getTransform().scale;
            scale.y = chLayout.scale.y;
        }
    }
}

void SimpleLayoutEngine::processGridLayout(const AbstractNodePtr& parent)
{
    const AbstractNodePVec& children = parent->getChildren();
    const glm::vec2 pScale = parent->getTransform().scale;
    Layout::DistribRC& gridDistribRC = parent->getLayout().gridDist;

    // Calculate total frac/abs
    glm::ivec2 totalAbs{0, 0};
    glm::ivec2 totalFrac{0, 0};
    for (const auto& distribData : gridDistribRC.cols)
    {
        if (distribData.type == Layout::Distrib::ABS)
        {
            totalAbs.x += distribData.value;
        }
        else if (distribData.type == Layout::Distrib::FRAC)
        {
            totalFrac.x += distribData.value;
        }
    }

    for (const auto& distribData : gridDistribRC.rows)
    {
        if (distribData.type == Layout::Distrib::ABS)
        {
            totalAbs.y += distribData.value;
        }
        else if (distribData.type == Layout::Distrib::FRAC)
        {
            totalFrac.y += distribData.value;
        }
    }

    float wPerFrac = (pScale.x - totalAbs.x) / totalFrac.x;
    float hPerFrac = (pScale.y - totalAbs.y) / totalFrac.y;
    // log_.debugLn("wPerFract %f", wPerFrac);
    // log_.debugLn("Frac %d Abs %d", totalFrac.y, totalAbs.y);

    float startX{0};
    for (auto& distribData : gridDistribRC.cols)
    {
        distribData.computedStart = startX;

        if (distribData.type == Layout::Distrib::Type::FRAC)
        {
            startX += distribData.value * wPerFrac;
        }
        else if (distribData.type == Layout::Distrib::Type::ABS)
        {
            startX += distribData.value;
        }
    }

    float startY{0};
    for (auto& distribData : gridDistribRC.rows)
    {
        distribData.computedStart = startY;

        if (distribData.type == Layout::Distrib::Type::FRAC)
        {
            startY += distribData.value * hPerFrac;
        }
        else if (distribData.type == Layout::Distrib::Type::ABS)
        {
            startY += distribData.value;
        }
    }

    int32_t colsCount = gridDistribRC.cols.size();
    int32_t rowsCount = gridDistribRC.rows.size();
    for (auto ch : children)
    {
        IGNORE_SCROLLBAR
        auto& pos = ch->getTransform().pos;
        auto& scale = ch->getTransform().scale;
        auto& gridStart = ch->getLayout().gridStartRC;
        // pos.y = hPerFrac * gridStart.y;

        // TODO: gridStart.xy bound check
        if ((gridStart.col >= 0 || gridStart.col < colsCount) &&
            (gridStart.row >= 0 || gridStart.row < rowsCount))
        {
            pos.x = gridDistribRC.cols[gridStart.col].computedStart;
            pos.y = gridDistribRC.rows[gridStart.row].computedStart;
        }
        
        float endX = gridStart.col + 1 < colsCount ? gridDistribRC.cols[gridStart.col + 1].computedStart : pScale.x;
        float endY = gridStart.row + 1 < rowsCount ? gridDistribRC.rows[gridStart.row + 1].computedStart : pScale.y;
        scale.x = endX - gridDistribRC.cols[gridStart.col].computedStart;
        scale.y = endY - gridDistribRC.rows[gridStart.row].computedStart;
    }
}

#undef IGNORE_GRID_ALIGN
#undef IGNORE_LR_ALIGN
#undef IGNORE_TB_ALIGN
} // namespace msgui
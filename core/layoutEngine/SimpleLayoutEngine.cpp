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

glm::vec2 SimpleLayoutEngine::process(const AbstractNodePtr& parent)
{
    const AbstractNodePVec& children = parent->getChildren();

    if (children.empty()) { return {0, 0}; }

    const Layout* layout = static_cast<Layout*>(parent->getProps());
    if (!layout)
    {
        log_.errorLn("Whoops no layout %s", parent->getCName());
        return {0, 0};
    }

    // Compute SCROLLBARS position PASS
    ScrollBarsData scrollNodeData = processScrollbars(parent);

    // Useless to compute further if parent is a scroll node (knob computed already)
    if (parent->getType() == AbstractNode::NodeType::SCROLL) { return {0, 0}; }

    auto& pPos = parent->getTransform().pos;
    glm::vec3 pScale = parent->getTransform().scale;
    pScale.x -= scrollNodeData.shrinkBy.x + layout->padding.value.left + layout->padding.value.right
        + layout->border.value.left + layout->border.value.right;
    pScale.y -= scrollNodeData.shrinkBy.y + layout->padding.value.top + layout->padding.value.bot
        + layout->border.value.bot + layout->border.value.top;
    
    // TEMPORARY HERE:
    // Compute box divider shit
    if (parent->getType() == AbstractNode::NodeType::BOX_DIVIDER)
    {
        glm::vec3 totalAbsChildSize{0};
        for (auto& ch : children)
        {
            if (ch->getType() == AbstractNode::NodeType::SCROLL ||
                ch->getType() == AbstractNode::NodeType::SCROLL_KNOB)
            { continue; }

            const Layout* chLayout = static_cast<Layout*>(ch->getProps());
            if (!chLayout)
            {
                log_.errorLn("Whoops no layout %s", ch->getCName());
                return {0, 0};
            }

            if (chLayout->scaleType.value.x == Layout::ScaleType::ABS)
            {
                totalAbsChildSize.x += chLayout->scale.value.x;
            }

            if (chLayout->scaleType.value.y == Layout::ScaleType::ABS)
            {
                totalAbsChildSize.y += chLayout->scale.value.y;
            }
        }
        processBoxDivider(pScale - totalAbsChildSize, children);
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
        if (ch->getType() == AbstractNode::NodeType::SCROLL ||
            ch->getType() == AbstractNode::NodeType::SCROLL_KNOB)
        { continue; }

        const Layout* chLayout = static_cast<Layout*>(ch->getProps());
        if (!chLayout)
        {
            log_.errorLn("Whoops no layout %s", ch->getCName());
            return {0, 0};
        }

        totalChildSize.x += ch->getTransform().scale.x + (chLayout->margin.value.left + chLayout->margin.value.right);
        totalChildSize.y += ch->getTransform().scale.y + (chLayout->margin.value.top + chLayout->margin.value.bot);
    }

    // Compute ZERO relative position of objects PASS
    float startX{0};
    float startY{0};
    int32_t startIdx{0};
    int32_t endIdx{0};
    if (layout->type == Layout::Type::HORIZONTAL)
    {
        // Dealing with layout Spacing
        float spacing{0};
        switch (layout->spacing)
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
                    static_cast<uint8_t>(layout->spacing));
        }

        float rollingX = startX;
        float maxY{0};
        for (auto& ch : children)
        {
            // Already calculated, skip
            if (ch->getType() == AbstractNode::NodeType::SCROLL ||
                ch->getType() == AbstractNode::NodeType::SCROLL_KNOB)
            { continue; }

            const Layout* chLayout = static_cast<Layout*>(ch->getProps());
            if (!chLayout)
            {
                log_.errorLn("Whoops no layout %s", ch->getCName());
                return {0, 0};
            }

            auto& pos = ch->getTransform().pos;
            auto& scale = ch->getTransform().scale;
            if (layout->allowWrap)
            {
                if (rollingX + scale.x + chLayout->margin.value.left + chLayout->margin.value.right > pScale.x)
                {
                    startX = 0;
                    startY += maxY;
                    rollingX = 0;

                    // Align Self (2nd zero positioning)
                    resolveAlignSelf(children, startIdx, endIdx, maxY, Layout::Type::HORIZONTAL);

                    maxY = 0;
                    startIdx = endIdx;
                }
                rollingX += scale.x + chLayout->margin.value.left + chLayout->margin.value.right;
            }
            maxY = std::max(maxY, scale.y + chLayout->margin.value.top + chLayout->margin.value.bot);
            endIdx++;
            // 1st zero positioning
            pos.x = startX + chLayout->margin.value.left;
            pos.y = startY + chLayout->margin.value.top;
            startX += scale.x + spacing + (chLayout->margin.value.left + chLayout->margin.value.right);
        }

        // Align Self (2nd zero positioning)
        resolveAlignSelf(children, startIdx, endIdx, maxY, Layout::Type::HORIZONTAL);
    }
    else if (layout->type == Layout::Type::VERTICAL)
    {
        // Dealing with layout Spacing
        float spacing{0};
        switch (layout->spacing)
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
                    static_cast<uint8_t>(layout->spacing));
        }

        float rollingY = startY;
        float maxX{0};
        for (auto& ch : children)
        {
            // Already calculated, skip
            if (ch->getType() == AbstractNode::NodeType::SCROLL ||
                ch->getType() == AbstractNode::NodeType::SCROLL_KNOB)
            { continue; }

            const Layout* chLayout = static_cast<Layout*>(ch->getProps());
            if (!chLayout)
            {
                log_.errorLn("Whoops no layout %s", ch->getCName());
                return {0, 0};
            }

            auto& pos = ch->getTransform().pos;
            auto& scale = ch->getTransform().scale;
            if (layout->allowWrap)
            {
                if (rollingY + scale.y + chLayout->margin.value.top + chLayout->margin.value.bot > pScale.y)
                {
                    startX += maxX;
                    startY = 0;
                    rollingY = 0;

                    // Align Self (2nd zero positioning)
                    resolveAlignSelf(children, startIdx, endIdx, maxX, Layout::Type::VERTICAL);

                    maxX = 0;
                    startIdx = endIdx;
                }
                rollingY += scale.y + chLayout->margin.value.top + chLayout->margin.value.bot;
            }
            maxX = std::max(maxX, scale.x + chLayout->margin.value.left + chLayout->margin.value.right);
            endIdx++;
            // 1st zero positioning
            pos.x = startX + chLayout->margin.value.left;
            pos.y = startY + chLayout->margin.value.top;
            startY += scale.y + spacing + (chLayout->margin.value.top + chLayout->margin.value.bot);
        }
        // Align Self (2nd zero positioning)
        resolveAlignSelf(children, startIdx, endIdx, maxX, Layout::Type::VERTICAL);
    }

    // Compute children overflow PASS
    const glm::vec2 computedOverflow = computeOverflow(pScale, children);

    // Apply SCROLLBAR offsets + any group offseting from ZERO PASS
    for (auto& ch : children)
    {
        // Already calculated, skip
        if (ch->getType() == AbstractNode::NodeType::SCROLL ||
            ch->getType() == AbstractNode::NodeType::SCROLL_KNOB)
        { continue; }

        auto& pos = ch->getTransform().pos;
        pos.x += -scrollNodeData.offsetPx.x + pPos.x + layout->padding.value.left + layout->border.value.left;
        pos.y += -scrollNodeData.offsetPx.y + pPos.y + layout->padding.value.top + layout->border.value.top;

        // AlignChild
        // Negative overflow means we still have X amount of pixels until the parent is full on that axis
        // We can leverage this to position elements top, left, right, bot, center.
        if (computedOverflow.x < 0)
        {
            switch (layout->alignChildX)
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
                        static_cast<uint8_t>(layout->alignChildX));
            }
        }

        if (computedOverflow.y < 0)
        {
            switch (layout->alignChildY)
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
                        static_cast<uint8_t>(layout->alignChildY));
            }
        }
    }

    return computedOverflow;
}

void SimpleLayoutEngine::computeNodeScale(const glm::vec2& pScale, const AbstractNodePVec& children)
{
    for (auto& ch : children)
    {
        // Shall not be taken into consideration as these are calculated differently
        if (ch->getType() == AbstractNode::NodeType::SCROLL ||
            ch->getType() == AbstractNode::NodeType::SCROLL_KNOB)
        { continue; }

        const Layout* chLayout = static_cast<Layout*>(ch->getProps());
        if (!chLayout)
        {
            log_.errorLn("Whoops no layout %s", ch->getCName());
            return;
        }

        if (chLayout->scaleType.value.x == Layout::ScaleType::REL)
        {
            auto& scale = ch->getTransform().scale;
            scale.x = pScale.x * chLayout->scale.value.x;
            scale.x -= chLayout->margin.value.left + chLayout->margin.value.right;
        }

        if (chLayout->scaleType.value.y == Layout::ScaleType::REL)
        {
            auto& scale = ch->getTransform().scale;
            scale.y = pScale.y * chLayout->scale.value.y;
            scale.y -= chLayout->margin.value.top + chLayout->margin.value.bot;
        }

        if (chLayout->scaleType.value.x == Layout::ScaleType::ABS)
        {
            auto& scale = ch->getTransform().scale;
            scale.x = chLayout->scale.value.x;
        }

        if (chLayout->scaleType.value.y == Layout::ScaleType::ABS)
        {
            auto& scale = ch->getTransform().scale;
            scale.y = chLayout->scale.value.y;
        }
    }
}

void SimpleLayoutEngine::resolveAlignSelf(const AbstractNodePVec& children, const uint32_t idxStart,
    const uint32_t idxEnd, const int32_t max, const Layout::Type type)
{
    for (uint32_t i = idxStart; i < idxEnd; i++)
    {
        const Layout* chLayout = static_cast<Layout*>(children[i]->getProps());
        if (!chLayout)
        {
            log_.errorLn("Whoops no layout %s", children[i]->getCName());
            return;
        }

        auto& pos = children[i]->getTransform().pos;
        auto& scale = children[i]->getTransform().scale;

        if (type == Layout::Type::HORIZONTAL)
        {
            switch (chLayout->alignSelf)
            {
                IGNORE_GRID_ALIGN
                IGNORE_LR_ALIGN
                case Layout::TOP:
                    // Do nothing
                    break;
                case Layout::CENTER:
                    pos.y += (max - scale.y - chLayout->margin.value.bot) * 0.5f;
                    break;
                case Layout::BOTTOM:
                    pos.y += max - scale.y - chLayout->margin.value.bot;
                    break;
                default:
                    log_.warnLn("Unrecognized horizontal alignSelf value: ENUM(%d)",
                        static_cast<uint8_t>(chLayout->alignSelf));
            }
        }
        else if (type == Layout::Type::VERTICAL)
        {
            switch (chLayout->alignSelf)
            {
                IGNORE_GRID_ALIGN
                IGNORE_TB_ALIGN
                case Layout::LEFT:
                    // Do nothing
                    break;
                case Layout::CENTER:
                    pos.x += (max - scale.x - chLayout->margin.value.right) * 0.5f;
                    break;
                case Layout::RIGHT:
                    pos.x += max - scale.x - chLayout->margin.value.right;
                    break;
                default:
                    log_.warnLn("Unrecognized vertical alignSelf value: ENUM(%d)",
                        static_cast<uint8_t>(chLayout->alignSelf));
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

        const Layout* chLayout = static_cast<Layout*>(ch->getProps());
        if (!chLayout)
        {
            log_.errorLn("Whoops no layout %s", ch->getCName());
            return {0, 0};
        }

        auto scale = ch->getTransform().scale;
        auto pos = ch->getTransform().pos;
        scale.x += chLayout->margin.value.right;
        scale.y += chLayout->margin.value.bot;
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

    const Layout* layout = static_cast<Layout*>(parent->getProps());
    if (!layout)
    {
        log_.errorLn("Whoops no layout %s", parent->getCName());
        return ScrollBarsData{};
    }

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
            pos.x = pPos.x + pScale.x - scale.x - layout->border.value.right;
            pos.y = pPos.y + layout->border.value.top;
            scale.y = pScale.y - (bothSbOn ? scale.x : 0)
             - (layout->border.value.top + layout->border.value.bot);

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
            pos.y = pPos.y + pScale.y - scale.y - layout->border.value.bot;
            pos.x = pPos.x + layout->border.value.left;
            scale.x = pScale.x - (bothSbOn ? scale.y : 0)
                - (layout->border.value.left + layout->border.value.right);

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
    const Slider* sliderRawPtr = static_cast<Slider*>(parent.get());
    SliderKnob* knobRawPtr = static_cast<SliderKnob*>(parent->getChildren()[0].get());

    if (!sliderRawPtr || !knobRawPtr)
    {
        log_.errorLn("Couldn't cast to Slider or SliderKnob for %s", parent->getCName());
        return;
    }

    const Layout* layout = static_cast<Layout*>(parent->getProps());
    if (!layout)
    {
        log_.errorLn("Whoops no layout %s", parent->getCName());
        return;
    }

    auto& pPos = sliderRawPtr->getTransform().pos;
    auto& pScale = sliderRawPtr->getTransform().scale;
    auto& kPos = knobRawPtr->getTransform().pos;
    auto& kScale = knobRawPtr->getTransform().scale;

    // Knob positioning
    float sliderOffset = sliderRawPtr->getOffsetPerc();
    if (sliderRawPtr->props.orientType == Layout::Type::HORIZONTAL)
    {
        float newX = Utils::remap(sliderOffset,
            0.0f, 1.0f, pPos.x + kScale.x / 2, pPos.x + pScale.x - kScale.x / 2);
        kPos.y = pPos.y;
        kPos.x = newX - kScale.x / 2;
    }
    else if (sliderRawPtr->props.orientType == Layout::Type::VERTICAL)
    {
        float newY = Utils::remap(sliderOffset,
            0.0f, 1.0f, pPos.y + kScale.y / 2, pPos.y + pScale.y - kScale.y / 2);
        kPos.x = pPos.x;
        kPos.y = newY - kScale.y / 2;
    }
}

void SimpleLayoutEngine::processBoxDivider(const glm::vec2& pScale, const AbstractNodePVec& children)
{
    // for (auto& ch : children)
    // {
    //     if (ch->getType() == AbstractNode::NodeType::BOX_DIVIDER_SEP) { continue; }

    //     Layout* chLayout = static_cast<Layout*>(ch->getProps());
    //     if (!chLayout)
    //     {
    //         log_.errorLn("Whoops no layout %s", ch->getCName());
    //         return;
    //     }

    //     // 0     x     1
    //     // 0   300   1280
    //     if (chLayout->scale.value.x > 1.0f)
    //     {
    //         float scaleX = Utils::remap(chLayout->scale.value.x, 0, 1270, 0.0f, 1.0f);
    //         chLayout->scale.value.x = scaleX;
    //     }
    // }

    // see if there's any MIN not satisfied
    float runningMinOverflowX{0};
    for (auto& ch : children)
    {
        if (ch->getType() == AbstractNode::NodeType::BOX_DIVIDER_SEP) { continue; }

        Layout* chLayout = static_cast<Layout*>(ch->getProps());
        if (!chLayout)
        {
            log_.errorLn("Whoops no layout %s", ch->getCName());
            return;
        }

        if (chLayout->minScale.value.x - chLayout->scale.value.x > 0)
        {
            runningMinOverflowX += chLayout->minScale.value.x - chLayout->scale.value.x;
        }
        chLayout->scale.value.x = std::max(chLayout->scale.value.x, chLayout->minScale.value.x);
    }
    // log_.debugLn("runningX: %f", runningMinOverflowX);

    // there are MINs not satisfied; try to spread the overflow to the rest of the nodes
    // who can take it
    for (auto& ch : children)
    {
        if (runningMinOverflowX > 0)
        {
            if (ch->getType() == AbstractNode::NodeType::BOX_DIVIDER_SEP) { continue; }

            Layout* chLayout = static_cast<Layout*>(ch->getProps());
            if (!chLayout)
            {
                log_.errorLn("Whoops no layout %s", ch->getCName());
                return;
            }

            float distToMin = chLayout->scale.value.x - chLayout->minScale.value.x;
            if (distToMin > 0)
            {
                if (runningMinOverflowX - distToMin > 0)
                {
                    chLayout->scale.value.x -= distToMin;
                    runningMinOverflowX -= distToMin;
                }
                else
                {
                    chLayout->scale.value.x -= runningMinOverflowX;
                    runningMinOverflowX = 0;
                }
            }
        }
    }
    // log_.debugLn("runningX: %f", runningX);

    // if there's still something in runningX it means the mins cannot be satisfied with current layout;
    // error out, notify user, idk
    for (auto& ch : children)
    {
        Layout* chLayout = static_cast<Layout*>(ch->getProps());
        if (!chLayout)
        {
            log_.errorLn("Whoops no layout %s", ch->getCName());
            return;
        }

        if (chLayout->scaleType.value.x == Layout::ScaleType::REL)
        {
            auto& scale = ch->getTransform().scale;
            scale.x = pScale.x * chLayout->scale.value.x;
            scale.x -= chLayout->margin.value.left + chLayout->margin.value.right;
            log_.debugLn("x %f %s", chLayout->scale.value.x, ch->getCName());
        }

        if (chLayout->scaleType.value.y == Layout::ScaleType::REL)
        {
            auto& scale = ch->getTransform().scale;
            scale.y = pScale.y * chLayout->scale.value.y;
            scale.y -= chLayout->margin.value.top + chLayout->margin.value.bot;
        }

        if (chLayout->scaleType.value.x == Layout::ScaleType::ABS)
        {
            auto& scale = ch->getTransform().scale;
            scale.x = chLayout->scale.value.x;
            log_.debugLn("x %f %s", scale.x, ch->getCName());
        }

        if (chLayout->scaleType.value.y == Layout::ScaleType::ABS)
        {
            auto& scale = ch->getTransform().scale;
            scale.y = chLayout->scale.value.y;
        }
    }
}

#undef IGNORE_GRID_ALIGN
#undef IGNORE_LR_ALIGN
#undef IGNORE_TB_ALIGN
} // namespace msgui
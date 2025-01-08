#include "SimpleLayoutEngine.hpp"

#include "core/node/AbstractNode.hpp"
#include "core/node/Box.hpp"
#include "core/node/utils/LayoutData.hpp"
#include "core/node/utils/ScrollBar.hpp"

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

glm::ivec2 SimpleLayoutEngine::process(const AbstractNodePtr& parent)
{
    const AbstractNodePVec& children = parent->getChildren();

    if (children.empty()) { return {0, 0}; }

    Layout* layout = static_cast<Layout*>(parent->getProps());
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
    pScale.x -= scrollNodeData.shrinkBy.x;
    pScale.y -= scrollNodeData.shrinkBy.y;

    // Compute total scale of children
    glm::ivec2 totalChildSize{0, 0};
    for (auto& ch : children)
    {
        if (ch->getType() == AbstractNode::NodeType::SCROLL ||
            ch->getType() == AbstractNode::NodeType::SCROLL_KNOB)
        { continue; }

        totalChildSize.x += ch->getTransform().scale.x;
        totalChildSize.y += ch->getTransform().scale.y;
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

            auto& pos = ch->getTransform().pos;
            auto& scale = ch->getTransform().scale;
            if (layout->allowWrap)
            {
                if (rollingX + scale.x > pScale.x)
                {
                    startX = 0;
                    startY += maxY;
                    rollingX = 0;

                    // Align Self (2nd zero positioning)
                    resolveAlignSelf(children, startIdx, endIdx, maxY, Layout::Type::HORIZONTAL);

                    maxY = 0;
                    startIdx = endIdx;
                }
                rollingX += scale.x;
            }
            maxY = std::max(maxY, scale.y);
            endIdx++;
            // 1st zero positioning
            pos.x = startX;
            pos.y = startY;
            startX += scale.x + spacing;
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

            auto& pos = ch->getTransform().pos;
            auto& scale = ch->getTransform().scale;
            if (layout->allowWrap)
            {
                if (rollingY + scale.y > pScale.y)
                {
                    startX += maxX;
                    startY = 0;
                    rollingY = 0;

                    // Align Self (2nd zero positioning)
                    resolveAlignSelf(children, startIdx, endIdx, maxX, Layout::Type::VERTICAL);

                    maxX = 0;
                    startIdx = endIdx;
                }
                rollingY += scale.y;
            }
            maxX = std::max(maxX, scale.x);
            endIdx++;
            // 1st zero positioning
            pos.x = startX;
            pos.y = startY;
            startY += scale.y + spacing;
        }
        // Align Self (2nd zero positioning)
        resolveAlignSelf(children, startIdx, endIdx, maxX, Layout::Type::VERTICAL);
    }

    // Compute children overflow PASS
    const glm::ivec2 computedOverflow = computeOverflow(pScale, children);

    // Apply SCROLLBAR offsets + any group offseting from ZERO PASS
    for (auto& ch : children)
    {
        // Already calculated, skip
        if (ch->getType() == AbstractNode::NodeType::SCROLL ||
            ch->getType() == AbstractNode::NodeType::SCROLL_KNOB)
        { continue; }

        auto& pos = ch->getTransform().pos;
        pos.x += -scrollNodeData.offsetPx.x + pPos.x;
        pos.y += -scrollNodeData.offsetPx.y + pPos.y;

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

void SimpleLayoutEngine::resolveAlignSelf(const AbstractNodePVec& children, const uint32_t idxStart,
    const uint32_t idxEnd, const int32_t max, const Layout::Type type)
{
    for (uint32_t i = idxStart; i < idxEnd; i++)
    {
        Layout* chLayout = static_cast<Layout*>(children[i]->getProps());
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
                    pos.y += (max - scale.y) * 0.5f;
                    break;
                case Layout::BOTTOM:
                    pos.y += max - scale.y;
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
                    pos.x += (max - scale.x) * 0.5f;
                    break;
                case Layout::RIGHT:
                    pos.x += max - scale.x;
                    break;
                default:
                    log_.warnLn("Unrecognized vertical alignSelf value: ENUM(%d)",
                        static_cast<uint8_t>(chLayout->alignSelf));
            }
        }
    }
}

glm::ivec2 SimpleLayoutEngine::computeOverflow(const glm::ivec2& pScale, const AbstractNodePVec& children)
{
    glm::ivec2 currentScale{0, 0};
    for (auto& ch : children)
    {
        // Shall not be taken into consideration for overflow
        if (ch->getType() == AbstractNode::NodeType::SCROLL ||
            ch->getType() == AbstractNode::NodeType::SCROLL_KNOB)
        { continue; }

        const auto& scale = ch->getTransform().scale;
        const auto& pos = ch->getTransform().pos;
        currentScale.x = std::max(currentScale.x ,(int32_t)(pos.x + scale.x));
        currentScale.y = std::max(currentScale.y ,(int32_t)(pos.y + scale.y));
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

    // Return by how much should the parent "shrink" to fit scrollbars
    ScrollBarsData data;

    bool bothSbOn{false};
    if (parent->getType() == AbstractNode::NodeType::BOX)
    {
        Box* castBox = static_cast<Box*>(parent.get());
        if (castBox->isScrollBarActive(ScrollBar::Orientation::ALL))
        {
            bothSbOn = true;
        }
    }

    auto& pPos = parent->getTransform().pos;
    auto& pScale = parent->getTransform().scale;

    // glm::vec2 sbSize = {20, pScale.y};
    const float hardcodedSize = 20;
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
            pos.x = pPos.x + pScale.x - hardcodedSize;
            pos.y = pPos.y;
            scale.x = hardcodedSize;
            scale.y = pScale.y - (bothSbOn ? hardcodedSize : 0);

            // Knob positioning
            AbstractNodePtr knob = sb->getChildren()[0]; // Always exists
            float sbOffset = sb->getKnobOffset();
            auto& kPos = knob->getTransform().pos;
            auto& kScale = knob->getTransform().scale;

            float newY = Utils::remap(sbOffset,
                0.0f, 1.0f, pos.y + hardcodedSize / 2, pos.y + scale.y - hardcodedSize / 2);
            kScale.x = hardcodedSize;
            kScale.y = hardcodedSize;
            kPos.x = pos.x;
            kPos.y = newY - hardcodedSize / 2;

            // Horizontal available space needs to decrease & current offset in px
            data.shrinkBy.x = hardcodedSize;
            data.offsetPx.y = sb->geOverflowOffset();
        }
        else if (sb->getOrientation() == ScrollBar::Orientation::HORIZONTAL)
        {
            // Scrollbar positioning
            pos.y = pPos.y + pScale.y - hardcodedSize;
            pos.x = pPos.x;
            scale.y = hardcodedSize;
            scale.x = pScale.x - (bothSbOn ? hardcodedSize : 0);

            // Knob positioning
            AbstractNodePtr knob = sb->getChildren()[0]; // Always exists
            auto& kPos = knob->getTransform().pos;
            auto& kScale = knob->getTransform().scale;
            float sbOffset = sb->getKnobOffset();

            float newX = Utils::remap(sbOffset,
                0.0f, 1.0f, pos.x + hardcodedSize / 2, pos.x + scale.x - hardcodedSize / 2);
            kScale.y = hardcodedSize;
            kScale.x = hardcodedSize;
            kPos.y = pos.y;
            kPos.x = newX - hardcodedSize / 2;

            // Vertical available space needs to decrease & current offset in px
            data.shrinkBy.y = hardcodedSize;
            data.offsetPx.x = sb->geOverflowOffset();
        }
    }

    return data;
}
#undef IGNORE_GRID_ALIGN
#undef IGNORE_LR_ALIGN
#undef IGNORE_TB_ALIGN
} // namespace msgui
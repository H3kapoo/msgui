#include "SimpleLayoutEngine.hpp"

#include "core/node/AbstractNode.hpp"
#include "core/node/Box.hpp"
#include "core/node/utils/LayoutData.hpp"
#include "core/node/utils/ScrollBar.hpp"

namespace msgui
{
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

    // Compute ZERO relative position of objects PASS
    int32_t startX = 0;
    int32_t startY = 0;
    int32_t maxY{0};
    int32_t startIdx{0};
    int32_t endIdx{0};
    if (layout->type == Layout::Type::HORIZONTAL)
    {
        int32_t rollingX = startX;
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
                    alignSelfHorizontal(children, startIdx, endIdx, maxY);

                    maxY = 0;
                    startIdx = endIdx;
                }
                rollingX += scale.x;
            }
            maxY = std::max(maxY, (int32_t)scale.y);
            endIdx++;
            // 1st zero positioning
            pos.x = startX;
            pos.y = startY;
            startX += scale.x;
        }
    }
    else if (layout->type == Layout::Type::VERTICAL)
    {
        int32_t rollingY = startY;
        int32_t maxX{0};
        for (auto& ch : children)
        {
            // Already calculated, skip
            if (ch->getType() == AbstractNode::NodeType::SCROLL ||
                ch->getType() == AbstractNode::NodeType::SCROLL_KNOB)
            {
                continue;
            }

            auto& pos = ch->getTransform().pos;
            auto& scale = ch->getTransform().scale;
            if (layout->allowWrap)
            {
                if (rollingY + scale.y > pScale.y)
                {
                    startX += maxX;
                    startY = 0;
                    rollingY = 0;
                    maxX = 0;
                }
                rollingY += scale.y;
                maxX = std::max(maxX, (int32_t)scale.x);
            }

            pos.x = startX;
            pos.y = startY;
            startY += scale.y;
        }
    }

    // Align Self (2nd zero positioning)
    alignSelfHorizontal(children, startIdx, endIdx, maxY);

    // Compute children overflow PASS
    const glm::ivec2 computedOverflow = computeOverflow(pPos, pScale, children);

    // log_.debugLn("%s %d %d", parent->getCName(), bounds.x, bounds.y);
    // Apply SCROLLBAR offsets + any offseting from ZERO PASS
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
                    log_.warnLn("Unrecognized alignChildX value");
            }
        }

        if (computedOverflow.y < 0)
        {
            switch (layout->alignChildY)
            {
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
                    log_.warnLn("Unrecognized alignChildY value");
            }
        }
    }

    return computedOverflow;
}

void SimpleLayoutEngine::alignSelfHorizontal(const AbstractNodePVec& children, const uint32_t idxStart,
    const uint32_t idxEnd, const int32_t maxY)
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
        switch (chLayout->alignSelf)
        {
            case Layout::TOP:
                // Do nothing
                break;
            case Layout::CENTER:
                pos.y += (maxY - scale.y) * 0.5f;
                break;
            case Layout::BOTTOM:
                break;
            default:
                log_.warnLn("Unrecognized alignSelf value");
        }
    }
}

glm::ivec2 SimpleLayoutEngine::computeOverflow(const glm::ivec2& pPos, const glm::ivec2& pScale,
    const AbstractNodePVec& children)
{
    glm::ivec2 currentScale{0, 0};

    for (auto& ch : children)
    {
        // Shall not be taken into consideration for overflow
        if (ch->getType() == AbstractNode::NodeType::SCROLL ||
            ch->getType() == AbstractNode::NodeType::SCROLL_KNOB)
        {
            continue;
        }

        const auto& scale = ch->getTransform().scale;
        const auto& pos = ch->getTransform().pos;
        currentScale.x = std::max(currentScale.x ,(int32_t)(pos.x + scale.x));
        currentScale.y = std::max(currentScale.y ,(int32_t)(pos.y + scale.y));
        // currentScale.x = std::max(currentScale.x ,(int32_t)scale.x);
        // currentScale.y = std::max(currentScale.y ,(int32_t)scale.y);
    }

    // log_.debugLn("overflow x:%d", currentScale.x - (int32_t)pScale.x);
    // log_.debugLn("overflow y:%d", currentScale.y - (int32_t)pScale.y);
    return {currentScale.x - pScale.x, currentScale.y - pScale.y};
    // return {currentScale.x - (pScale.x + pPos.x), currentScale.y - (pScale.y + pPos.y)};
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
        if (ch->getType() != AbstractNode::NodeType::SCROLL)
        {
            continue;
        }

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
} // namespace msgui
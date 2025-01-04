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

    ScrollBarsData scrollNodeData = processScrollbars(parent);

    // Useless to compute further if parent is a scroll node (knob computed already)
    if (parent->getType() == AbstractNode::NodeType::SCROLL) { return {0, 0}; }

    auto& pPos = parent->getTransform().pos;
    glm::vec3 pScale = parent->getTransform().scale;
    pScale.x -= scrollNodeData.shrinkBy.x;
    pScale.y -= scrollNodeData.shrinkBy.y;

    // log_.debugLn("%s offsets y:%d -- %f", parent->getCName(), sbOffsets.y, sbOffsetsF.y);

    int32_t startX = pPos.x;
    int32_t startY = pPos.y;
    if (layout->orientation == Layout::Orientation::HORIZONTAL)
    {
        int32_t rollingX = startX;
        int32_t maxY{0};
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
                if (rollingX + scale.x > pScale.x)
                {
                    startX = 0;
                    startY += maxY;
                    rollingX = 0;
                    maxY = 0;
                }
                rollingX += scale.x;
                maxY = std::max(maxY, (int32_t)scale.y);
            }

            pos.x = startX;
            pos.y = startY;
            startX += scale.x;
        }
    }
    else if (layout->orientation == Layout::Orientation::VERTICAL)
    {
        int32_t rollingY = startY;
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
            // rollingY += scale.y;
            // if (rollingY > pScale.y)
            // {
            //     startY = 0;
            //     startX += scale.x;
            //     rollingY = pPos.y;
            // }

            pos.y = startY;
            pos.x = startX;
            startY += scale.y;
        }
    }

    const glm::ivec2 computedOverflow = computeOverflow(pPos, pScale, children);

    // Apply SB offsets (move nodes into scrollbar offseted position)
    // TODO: Maybe can be compressed somewhere else more efficiently?
    for (auto& ch : children)
    {
        // Already calculated, skip
        if (ch->getType() == AbstractNode::NodeType::SCROLL ||
            ch->getType() == AbstractNode::NodeType::SCROLL_KNOB)
        {
            continue;
        }

        auto& pos = ch->getTransform().pos;
        pos.x -= scrollNodeData.offsetPx.x;
        pos.y -= scrollNodeData.offsetPx.y;
    }
    return computedOverflow;
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
    }

    // log_.debugLn("overflow x:%d", currentScale.x - (int32_t)pScale.x);
    // log_.debugLn("overflow y:%d", currentScale.y - (int32_t)pScale.y);
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
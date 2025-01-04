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
    if (children.empty())
    {
        return {0, 0};
    }

    Layout* layout = static_cast<Layout*>(parent->getProps());
    if (!layout)
    {
        log_.errorLn("Whoops no layout %s", parent->getCName());
        return {0, 0};
    }

    glm::vec3 sbSize = processScrollbars(parent);

    // Useless to compute further if parent is a scroll node
    if (parent->getType() == AbstractNode::NodeType::SCROLL)
    {
        return {0, 0};
    }

    auto& pPos = parent->getTransform().pos;
    const auto pScale = parent->getTransform().scale - sbSize;

    // log_.debugLn("On it boss %s %d %ld", parent->getName().c_str(), parent->getId(), children.size());

    // Get sb overflow val
    glm::ivec2 sbOffsets{0, 0};
    glm::vec2 sbOffsetsF{0, 0};
    for (auto& ch : children)
    {
        if (ch->getType() != AbstractNode::NodeType::SCROLL)
        {
            continue;
        }

        ScrollBar* sb = static_cast<ScrollBar*>(ch.get());
        if (!sb)
        {
            log_.errorLn("Could not cast to ScrollBar: %s", ch->getCName());
            return {0, 0};
        }

        if (sb->getOrientation() == ScrollBar::Orientation::HORIZONTAL)
        {
            sbOffsets.x = sb->geOverflowOffset();
            sbOffsetsF.x = sb->getKnobOffset();
        }
        else if (sb->getOrientation() == ScrollBar::Orientation::VERTICAL)
        {
            sbOffsets.y = sb->geOverflowOffset();
            sbOffsetsF.y = sb->getKnobOffset();
        }
    }

    // log_.debugLn("%s overflowX: %d offsets x:%f", parent->getCName(), over.x, pScale.x * sbOffsets.x);
    // log_.debugLn("%s offsets x:%d -- %f", parent->getCName(), sbOffsets.x, sbOffsetsF.x);
    // log_.debugLn("%s offsets y:%d -- %f", parent->getCName(), sbOffsets.y, sbOffsetsF.y);


    int32_t startX = pPos.x;// - sbOffsets.x;
    int32_t startY = pPos.y;
    if (layout->orientation == Layout::Orientation::HORIZONTAL)
    {
        int32_t rollingX = startX;
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
            // rollingX += scale.x;
            // if (rollingX > pScale.x)
            // {
            //     startX = 0;
            //     startY += scale.y;
            //     rollingX = pPos.x;
            // }

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

    // Useless to compute overflow for a SCROLL node's children
    if (parent->getType() == AbstractNode::NodeType::SCROLL)
    {
        return {0, 0};
    }

    const glm::ivec2 computedOverflow = computeOverflow(pPos, pScale, children);

    // Apply SB offsets
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
        pos.x -= sbOffsets.x;
        pos.y -= sbOffsets.y;
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

glm::vec3 SimpleLayoutEngine::processScrollbars(const AbstractNodePtr& parent)
{
    const AbstractNodePVec& children = parent->getChildren();
    if (children.empty())
    {
        return {0, 0, 0};
    }

    // Return by how much should the parent "shrink" to fit scrollbars
    glm::vec3 returnedSizes{0};
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

            // Horizonal available space needs to decrease
            returnedSizes.x = hardcodedSize;
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

            // float newX = Utils::remap(sbOffset,
            //     0.0f, 1.0f, pos.x + hardcodedSize, pos.x + scale.x);

            float newX = Utils::remap(sbOffset,
                0.0f, 1.0f, pos.x + hardcodedSize / 2, pos.x + scale.x - hardcodedSize / 2);
            kScale.y = hardcodedSize;
            kScale.x = hardcodedSize;
            kPos.y = pos.y;
            kPos.x = newX - hardcodedSize / 2;

            // Vertical available space needs to decrease
            returnedSizes.y = hardcodedSize;
        }
    }

    return returnedSizes;
}
} // namespace msgui
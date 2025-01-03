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

    glm::vec3 sbSize = processScrollbars(parent);

    auto& pPos = parent->getTransform().pos;
    const auto pScale = parent->getTransform().scale - sbSize;

    // log_.infoLn("On it boss %s %d %ld", parent->getName().c_str(), parent->getId(), children.size());

    int32_t startX = pPos.x;
    int32_t startY = pPos.y;

    Layout* layout = static_cast<Layout*>(parent->getProps());
    if (!layout)
    {
        log_.errorLn("Whoops no layout %s", parent->getCName());
        return {0, 0};
    }

    glm::ivec2 computedOverflow{0, 0};
    glm::ivec2 currentScale{0, 0};
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
            currentScale.x += scale.x;

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

    // Ignore negative values

    computedOverflow.x = currentScale.x - pScale.x;
    // log_.infoLn("overflow %d", computedOverflow.x);
    return computedOverflow;
}

glm::vec3 SimpleLayoutEngine::processScrollbars(const AbstractNodePtr& parent)
{
    const AbstractNodePVec& children = parent->getChildren();
    if (children.empty())
    {
        return {0, 0, 0};
    }

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

    glm::vec3 returnedSizes{0};

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
            float sbOffset = sb->getOffset();
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
            float sbOffset = sb->getOffset();
            auto& kPos = knob->getTransform().pos;
            auto& kScale = knob->getTransform().scale;

            float newX = Utils::remap(sbOffset,
                0.0f, 1.0f, pos.x +  hardcodedSize / 2, pos.x + scale.x - hardcodedSize / 2);
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
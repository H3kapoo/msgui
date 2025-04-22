#pragma once

#include "msgui/layoutEngine/ILayoutEngine.hpp"
#include "msgui/Logger.hpp"
#include "msgui/node/AbstractNode.hpp"

namespace msgui
{
class CustomLayoutEngine : public ILayoutEngine
{
public:
    CustomLayoutEngine() = default;

    /**
        Process the layout for the current node.

        @param node Node on which the layour calculations will be performed
     */
    Result<glm::vec2> process(const AbstractNodePtr& node) override;

private:
    struct ScrollContribution
    {
        glm::ivec2 offset{0, 0};
        glm::ivec2 barScale{0, 0};
    };

    /* Common */
    Result<Void> computeSubNodesScale(const AbstractNodePtr& node, const ScrollContribution& sc);
    Result<Void> computeSubNodesPosition(const AbstractNodePtr& node, const ScrollContribution& sc);
    Result<glm::vec2> computeFitScale(const AbstractNodePtr& node);
    Result<Void> alignSubNodes(const AbstractNodePtr& node, const glm::vec2 computedOverflow);
    Result<Void> selfAlignSubNodeSlice(const AbstractNodePtr& node, const glm::vec2 maximum,
        const uint32_t startIdx, const uint32_t endIdx);
    void applyOverflowAndScrollOffsets(const AbstractNodePtr& node, const glm::vec2 overflow,
        const ScrollContribution& sc);
    glm::vec2 computeOverflow(const AbstractNodePtr& node, const ScrollContribution& sc);
    glm::vec2 computeNodeFreeSpace(const AbstractNodePtr& node, const ScrollContribution& sc);
    glm::vec2 computeNodeInnerStartOffsets(const AbstractNodePtr& node);
    glm::vec2 computeNoteOccupiedSpace(const AbstractNodePtr& node);

    /* Scrollbars */
    Result<ScrollContribution> computeScrollNodeContribution(const AbstractNodePtr& node);
    void handlerSliderNode(const AbstractNodePtr& node);


private:
    Logger log_{"CustomLayoutEngine"};
};
} // namespace msgui
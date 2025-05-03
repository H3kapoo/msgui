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
    Result<Void> process(const AbstractNodePtr& node) override;

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
    Result<Void> computeGridLayout(const AbstractNodePtr& node);
    Result<Void> updateGridFracPart(const AbstractNodePtr& node);
    Result<Void> selfAlignGridSubNode(const AbstractNodePtr& node, const glm::vec2& cellSize);

    /* Scrollbars */
    Result<ScrollContribution> computeScrollNodeContribution(const AbstractNodePtr& node);
    void handleSliderNode(const AbstractNodePtr& node);

    /* BoxDivider */
    Result<Void> handleBoxDividerNode(const AbstractNodePtr& node);
    Result<Void> computeBoxDividerSubNodesScale(const AbstractNodePtr& node,
        const glm::vec2 usableNodeSpace);
    Result<Void> computeBoxDividerSubNodesPos(const AbstractNodePtr& node);
    Result<Void> tryToSatisfyMinMaxBoxDividerValues(const AbstractNodePtr& node,
        const glm::vec2 usableNodeSpace);
    
    /* Dropdown */
    Result<Void> handleDropdown(const AbstractNodePtr& node);

    /* Helpers */
    void resolveCumulativeError(const AbstractNodePtr& node, const glm::vec2 totalInt,
        const glm::vec2 totalFloat);

private:
    Logger log_{"CustomLayoutEngine"};
};
} // namespace msgui
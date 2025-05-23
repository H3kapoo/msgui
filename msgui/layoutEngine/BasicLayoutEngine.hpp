#pragma once

#include "msgui/layoutEngine/ILayoutEngine.hpp"
#include "msgui/Logger.hpp"
#include "msgui/node/AbstractNode.hpp"
#include "msgui/layoutEngine/utils/LayoutData.hpp"

namespace msgui
{
/* Class responsible for positioning and scaling objects as user defined.
    Known bugs:
        - allowWrap + non-Tight spacing + small space so that scrollbar appears will
            make elements go one on top another bit by bit. This is because we compute total
            children scale instead of per-wrapped-row scale.
*/
class BasicLayoutEngine : public ILayoutEngine
{
private:
    struct ScrollBarsData
    {
        glm::ivec2 shrinkBy{0, 0};
        glm::ivec2 offsetPx{0, 0};
    };

public:
    BasicLayoutEngine() = default;

    /**
        Process the layour for the current node.

        @param node Node on which the layour calculations will be performed
     */
    glm::vec2 process(const AbstractNodePtr& node) override;

private:
    glm::vec2 getPaddedAndBorderedNodeScale(const AbstractNodePtr& node);
    glm::vec2 getTotalChildrenAbsScale(const AbstractNodePVec& children);
    glm::vec2 getBoxDivUnsatisfiedMinimumTotal(const AbstractNodePVec& children, const glm::vec2 nScale);
    void trySpreadBoxDivUnsatMinimum(const AbstractNodePtr& chNode, const glm::vec2 nScale, glm::vec2& unsatMin);
    glm::vec2 computeTotalRealNodesScale(const AbstractNodePVec& children);
    glm::vec2 computeSpacing(const AbstractNodePtr& node, const float nScale, const float totalChildSize);
    void computeNodeScale(const glm::vec2& nScale, const AbstractNodePtr& node);
    void resolveAlignSelf(const AbstractNodePVec& children, const uint32_t idxStart, const uint32_t idxEnd,
        const int32_t max, const utils::Layout::Type type);
    glm::vec2 computeOverflow(const glm::vec2& nScale, const AbstractNodePVec& children);
    void applyFinalOffsets(const AbstractNodePtr& node, const glm::vec2 overflow,
        const ScrollBarsData& scrollNodeData);
    ScrollBarsData processScrollbars(const AbstractNodePtr& node);
    bool processTreeView(const AbstractNodePtr& node);
    void processSlider(const AbstractNodePtr& node);
    void processFloatingBox(const AbstractNodePtr& node);
    void processBoxDivider(const glm::vec2& nScale, const AbstractNodePVec& children);
    void processDropdown(const AbstractNodePtr& node);
    void processGridLayout(const glm::vec2& nScale, const AbstractNodePtr& node);
    void processHVLayout(const AbstractNodePtr& node, const glm::vec2 nScale);

private:
    Logger log_{"SimpleLayoutEngine"};
};
} // namespace msgui
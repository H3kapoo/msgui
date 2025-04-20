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
    Process the layour for the current node.

        @param node Node on which the layour calculations will be performed
     */
    Result<glm::vec2> process(const AbstractNodePtr& node) override;

private:
    Result<Void> computeSubNodesScale(const AbstractNodePtr& node);
    Result<Void> computeSubNodesPosition(const AbstractNodePtr& node);
    Result<glm::vec2> computeFitScale(const AbstractNodePtr& node);
    Result<Void> alignSubNodes(const AbstractNodePtr& node, const glm::vec2 computedOverflow);
    Result<Void> selfAlignSubNodeSlice(const AbstractNodePtr& node, const glm::vec2 maximum,
        const uint32_t startIdx, const uint32_t endIdx);
    glm::vec2 computeOverflow(const AbstractNodePtr& node);

private:
    Logger log_{"CustomLayoutEngine"};
};
} // namespace msgui
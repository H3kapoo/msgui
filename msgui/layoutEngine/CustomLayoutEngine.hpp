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

private:
    Logger log_{"CustomLayoutEngine"};
};
} // namespace msgui
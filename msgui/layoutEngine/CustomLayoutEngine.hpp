#pragma once

#include "msgui/layoutEngine/ILayoutEngine.hpp"
#include "msgui/Logger.hpp"
#include "msgui/node/AbstractNode.hpp"
#include "msgui/layoutEngine/utils/LayoutData.hpp"

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
    glm::vec2 process(const AbstractNodePtr& node) override;

private:
    void computeSubNodesScale(const AbstractNodePtr& node);
    void computeSubNodesPosition(const AbstractNodePtr& node);
    glm::vec2 computeFitScale(const AbstractNodePtr& node);

private:
    Logger log_{"CustomLayoutEngine"};
};
} // namespace msgui
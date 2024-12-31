#pragma once

#include "ILayoutEngine.hpp"
#include "core/Logger.hpp"
#include "core/node/AbstractNode.hpp"

namespace msgui
{
class SimpleLayoutEngine : public ILayoutEngine
{
public:
    SimpleLayoutEngine() = default;

    // Overrides
    void process(const AbstractNodePtr& parent) override;

private:
    // Normal
    glm::vec3 processScrollbars(const AbstractNodePtr& parent);

private:
    Logger log_{"SimpleLayoutEngine"};
};
} // namespace msgui
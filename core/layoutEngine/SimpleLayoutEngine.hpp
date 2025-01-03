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

    glm::ivec2 process(const AbstractNodePtr& parent) override;

private:
    glm::vec3 processScrollbars(const AbstractNodePtr& parent);

private:
    Logger log_{"SimpleLayoutEngine"};
};
} // namespace msgui
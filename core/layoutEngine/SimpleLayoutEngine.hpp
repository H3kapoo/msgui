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

    // Normal
    void process(const AbstractNodePtr& parent) override;

private:
    Logger log_{"SimpleLayoutEngine"};
};
} // namespace msgui
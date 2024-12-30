#pragma once

#include <memory>

#include "core/node/AbstractNode.hpp"

namespace msgui
{
class ILayoutEngine
{
public:
    // Normal
    virtual void process(const AbstractNodePtr& parent) = 0;
};

using ILayoutEnginePtr = std::shared_ptr<ILayoutEngine>;
} // namespace msgui
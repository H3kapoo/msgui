#pragma once

#include <memory>

#include "msgui/node/AbstractNode.hpp"

namespace msgui
{
class ILayoutEngine
{
public:
    virtual glm::vec2 process(const AbstractNodePtr& node) = 0;
};

using ILayoutEnginePtr = std::shared_ptr<ILayoutEngine>;
} // namespace msgui
#pragma once

#include <memory>

#include "msgui/node/AbstractNode.hpp"

namespace msgui
{
class ILayoutEngine
{
public:
    template<typename ResultType>
    struct Result
    {
        std::string error{};
        ResultType value{}; // do we still need it?
    };

    struct Void{};

public:
    virtual Result<Void> process(const AbstractNodePtr& node) = 0;
};

using ILayoutEnginePtr = std::shared_ptr<ILayoutEngine>;
} // namespace msgui
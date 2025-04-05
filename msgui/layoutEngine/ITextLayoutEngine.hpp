#pragma once

#include <memory>

#include "msgui/renderer/Types.hpp"

namespace msgui::layoutengine
{

class ITextLayoutEngine
{
public:
    virtual void process(renderer::TextData& data, const bool forceAllDirty) = 0;
};
using ITextLayoutEnginePtr = std::shared_ptr<ITextLayoutEngine>;
} // namespace msgui::layoutengine
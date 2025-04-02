#pragma once

#include <memory>

#include "msgui/renderer/text/Types.hpp"

namespace msgui::layoutengine::text
{
using namespace renderer::text;

class ITextLayoutEngine
{
public:
    virtual void process(TextData& data, const bool forceAllDirty) = 0;
};
using ITextLayoutEnginePtr = std::shared_ptr<ITextLayoutEngine>;
} // namespace msgui::layoutengine::text
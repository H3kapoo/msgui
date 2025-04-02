#pragma once

#include "msgui/Logger.hpp"
#include "msgui/layoutEngine/text/ITextLayoutEngine.hpp"
#include "msgui/renderer/text/Types.hpp"

namespace msgui::layoutengine::text
{
class SimpleTextLayoutEngine : public ITextLayoutEngine
{
public:
    virtual void process(TextData& data, const bool forceAllDirty) override;

private:
    Logger log_{"SimpleTextLayoutEngine"};
};
using SimpleTextLayoutEnginePtr = std::shared_ptr<SimpleTextLayoutEngine>;
} // namespace msgui::layoutengine::text


#pragma once

#include "msgui/layoutEngine/ITextLayoutEngine.hpp"
#include "msgui/Logger.hpp"
#include "msgui/renderer/Types.hpp"

namespace msgui::layoutengine
{
class BasicTextLayoutEngine : public ITextLayoutEngine
{
public:
    virtual void process(renderer::TextData& data, const bool forceAllDirty) override;

private:
    bool shouldSkipData(const renderer::TextData& data) const;
    glm::ivec2 computeTextLengthAndHeight(const renderer::TextData& data) const;

private:
    Logger log_{"SimpleTextLayoutEngine"};
};
using SimpleTextLayoutEnginePtr = std::shared_ptr<BasicTextLayoutEngine>;
} // namespace msgui::layoutengine


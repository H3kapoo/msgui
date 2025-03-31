#pragma once

#include "msgui/layoutEngine/text/ITextLayoutEngine.hpp"

namespace msgui::layoutengine::text
{
class SimpleTextLayoutEngine : public ITextLayoutEngine
{
public:
    virtual void process() override;
};
} // namespace msgui::layoutengine::text


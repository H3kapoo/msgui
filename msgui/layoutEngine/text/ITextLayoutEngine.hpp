#pragma once

namespace msgui::layoutengine::text
{
class ITextLayoutEngine
{
public:
    virtual void process() = 0;
};
} // namespace msgui::layoutengine::text
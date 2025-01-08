#pragma once

#include "ILayoutEngine.hpp"
#include "core/Logger.hpp"
#include "core/node/AbstractNode.hpp"
#include "core/node/utils/LayoutData.hpp"

namespace msgui
{
class SimpleLayoutEngine : public ILayoutEngine
{
private:
    struct ScrollBarsData
    {
        glm::ivec2 shrinkBy{0, 0};
        glm::ivec2 offsetPx{0, 0};
    };

public:
    SimpleLayoutEngine() = default;

    glm::ivec2 process(const AbstractNodePtr& parent) override;

private:
    void resolveAlignSelf(const AbstractNodePVec& children, const uint32_t idxStart, const uint32_t idxEnd,
        const int32_t max, const Layout::Type type);
    glm::ivec2 computeOverflow(const glm::ivec2& pScale, const AbstractNodePVec& children);
    ScrollBarsData processScrollbars(const AbstractNodePtr& parent);

private:
    Logger log_{"SimpleLayoutEngine"};
};
} // namespace msgui
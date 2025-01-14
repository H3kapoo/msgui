#pragma once

#include "core/node/AbstractNode.hpp"
#include "core/Listeners.hpp"
#include "core/node/Box.hpp"
#include "core/node/FrameState.hpp"
#include "core/node/utils/LayoutData.hpp"

namespace msgui
{
class BoxDividerSep : public AbstractNode
{
public:
    struct Props
    {
        Layout layout; // Do not change position
        glm::vec4 color{1.0f};
        glm::vec4 borderColor{1.0f};
    };

public:
    BoxDividerSep(const std::string& name, const BoxPtr& firstBox, const BoxPtr& secondBox);

    void setShaderAttributes() override;
    void* getProps() override;

private:
    void onMouseButtonNotify() override;
    void onMouseDragNotify() override;

    void setupReloadables();

    // BoxDividerSeps shall not have user added children
    // void append();
    // void appendMany();
    // void remove();
    // void removeMany();

public:
    Listeners listeners;
    Props props;

    BoxPtr firstBox_{nullptr};
    BoxPtr secondBox_{nullptr};
    bool activeNow_{false};
private:
    // BoxPtr firstBox_{nullptr};
    // BoxPtr secondBox_{nullptr};
};
using BoxDividerSepPtr = std::shared_ptr<BoxDividerSep>;
} // namespace msgui
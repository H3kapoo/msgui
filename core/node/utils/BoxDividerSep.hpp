#pragma once

#include "core/node/AbstractNode.hpp"
#include "core/Listeners.hpp"
#include "core/node/Box.hpp"
#include "core/node/FrameState.hpp"

namespace msgui
{
class BoxDividerSep : public AbstractNode
{
struct Props;
public:
    BoxDividerSep(const std::string& name, const BoxPtr& firstBox, const BoxPtr& secondBox);

    void setShaderAttributes() override;

    Props& setColor(const glm::vec4 color);
    Props& setBorderColor(const glm::vec4 color);

    bool getIsActiveSeparator();
    BoxPtr getFirstBox();
    BoxPtr getSecondBox();

private:
    void onMouseButtonNotify() override;
    void onMouseDragNotify() override;

    void setupLayoutReloadables();

public:
    Listeners listeners;
    bool activeNow_{false};

private:
    struct Props
    {
        glm::vec4 color{1.0f};
        glm::vec4 borderColor{1.0f};
        bool isActiveSeparator{false};
    };
    Props props;
    BoxPtr firstBox_{nullptr};
    BoxPtr secondBox_{nullptr};
};
using BoxDividerSepPtr = std::shared_ptr<BoxDividerSep>;
} // namespace msgui
#pragma once

#include "msgui/node/AbstractNode.hpp"
#include "msgui/node/Box.hpp"
#include "msgui/node/FrameState.hpp"
#include "msgui/events/LMBClick.hpp"
#include "msgui/events/LMBDrag.hpp"
#include "msgui/events/LMBRelease.hpp"
#include "msgui/events/LMBReleaseNotHovered.hpp"

namespace msgui
{
class BoxDividerSep : public AbstractNode
{
public:
    BoxDividerSep(const std::string& name, const BoxPtr& firstBox, const BoxPtr& secondBox);

    void setShaderAttributes() override;

    BoxDividerSep& setColor(const glm::vec4 color);
    BoxDividerSep& setBorderColor(const glm::vec4 color);

    bool getIsActiveSeparator();
    BoxPtr getFirstBox();
    BoxPtr getSecondBox();

private:
    BoxDividerSep(const BoxDividerSep&) = delete;
    BoxDividerSep(BoxDividerSep&&) = delete;
    BoxDividerSep& operator=(const BoxDividerSep&) = delete;
    BoxDividerSep& operator=(BoxDividerSep&&) = delete;

    void onMouseClick(const events::LMBClick& evt);
    void onMouseRelease(const events::LMBRelease& evt);
    void onMouseReleaseNotHovered(const events::LMBReleaseNotHovered& evt);
    void onMouseDrag(const events::LMBDrag& evt);

    void setupLayoutReloadables();

private:
    bool activeNow_{false};
    glm::vec4 color_{1.0f};
    glm::vec4 borderColor_{1.0f};
    bool isActiveSeparator_{false};
    BoxPtr firstBox_{nullptr};
    BoxPtr secondBox_{nullptr};
};
using BoxDividerSepPtr = std::shared_ptr<BoxDividerSep>;
using BoxDividerSepWPtr = std::weak_ptr<BoxDividerSep>;
} // namespace msgui
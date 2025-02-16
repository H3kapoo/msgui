#pragma once

#include "msgui/node/AbstractNode.hpp"
#include "msgui/node/Box.hpp"
#include "msgui/node/FrameState.hpp"
#include "msgui/nodeEvent/LMBClick.hpp"
#include "msgui/nodeEvent/LMBDrag.hpp"
#include "msgui/nodeEvent/LMBRelease.hpp"

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

    void onMouseClick(const nodeevent::LMBClick& evt);
    void onMouseRelease(const nodeevent::LMBRelease& evt);
    void onMouseDrag(const nodeevent::LMBDrag& evt);

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
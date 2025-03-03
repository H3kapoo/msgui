#pragma once

#include "msgui/node/AbstractNode.hpp"
#include "msgui/node/Box.hpp"
#include "msgui/nodeEvent/LMBRelease.hpp"

namespace msgui
{
/* A floating container that can be spawned at any coordinate. */
class FloatingBox : public AbstractNode
{
public:
    FloatingBox(const std::string& name);

    FloatingBox& setPreferredPosition(const glm::vec2& pos);

    glm::vec2 getPreferredPos() const;
    BoxWPtr getContainer();

    private:
    FloatingBox(const FloatingBox&) = delete;
    FloatingBox(FloatingBox&&) = delete;
    FloatingBox& operator=(const FloatingBox&) = delete;
    FloatingBox& operator=(FloatingBox&&) = delete;

    void setShaderAttributes() override;

    void onMouseRelease(const nodeevent::LMBRelease& evt);
private:
    glm::vec4 color_{1.0f};
    glm::vec4 borderColor_{1.0f};
    glm::vec2 preferredPos_{0, 0};

    BoxPtr box_{nullptr};
};

using FloatingBoxPtr = std::shared_ptr<FloatingBox>;
using FloatingBoxWPtr = std::weak_ptr<FloatingBox>;
} // namespace msgui
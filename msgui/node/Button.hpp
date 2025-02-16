#pragma once

#include "AbstractNode.hpp"
#include "msgui/Texture.hpp"
#include "msgui/nodeEvent/LMBClick.hpp"
#include "msgui/nodeEvent/LMBRelease.hpp"

namespace msgui
{
class Button : public AbstractNode
{
public:
    Button(const std::string& name);

    Button& setColor(const glm::vec4& color);
    Button& setPressedColor(const glm::vec4& color);
    Button& setBorderColor(const glm::vec4& color);
    Button& setTexture(const std::string texturePath);
    Button& setEnabled(const bool value);

    glm::vec4 getColor() const;
    glm::vec4 getBorderColor() const;
    std::string getTexturePath() const;

    ABSTRACT_NODE_ALLOW_APPEND_REMOVE
private:
    Button(const Button&) = delete;
    Button(Button&&) = delete;
    Button& operator=(const Button&) = delete;
    Button& operator=(Button&&) = delete;

    void setShaderAttributes() override;
    void onMouseClick(const nodeevent::LMBClick& evt);
    void onMouseRelease(const nodeevent::LMBRelease& evt);
    void setupLayoutReloadables();

private:
    glm::vec4 color_{1.0f};
    glm::vec4 currentColor_{1.0f};
    glm::vec4 pressedColor_{1.0f};
    glm::vec4 borderColor_{1.0f};
    glm::vec4 disabledColor_{1.0f};
    std::string texturePath_;
    TexturePtr btnTex_;
    bool isEnabled_{true};
    int32_t shrinkFactor{2};
};
using ButtonPtr = std::shared_ptr<Button>;
using ButtonWPtr = std::weak_ptr<Button>;
} // namespace msgui
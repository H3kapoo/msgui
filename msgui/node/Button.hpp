#pragma once

#include "AbstractNode.hpp"
#include "msgui/events/LMBClick.hpp"
#include "msgui/events/LMBRelease.hpp"
#include "msgui/events/LMBReleaseNotHovered.hpp"
#include "msgui/events/MouseEnter.hpp"
#include "msgui/events/MouseExit.hpp"
#include "msgui/node/Image.hpp"
#include "msgui/node/TextLabel.hpp"

namespace msgui
{
/* Node representing the common button. */
class Button : public AbstractNode
{
public:
    Button(const std::string& name);

    Button& setColor(const glm::vec4& color);
    Button& setPressedColor(const glm::vec4& color);
    Button& setBorderColor(const glm::vec4& color);
    Button& setTexture(const std::string texturePath);
    Button& setEnabled(const bool value);
    Button& setText(const std::string& text);
    Button& setImagePath(const std::string& path);

    glm::vec4 getColor() const;
    glm::vec4 getBorderColor() const;
    std::string getText() const;
    std::string getImagePath() const;
    TextLabelWPtr getTextLabel();
    ImageWPtr getImage();

private:
    /* Can't be copied or moved. */
    Button(const Button&) = delete;
    Button(Button&&) = delete;
    Button& operator=(const Button&) = delete;
    Button& operator=(Button&&) = delete;

    void setShaderAttributes() override;
    void setupLayoutReloadables();
    void onMouseClick(const events::LMBClick& evt);
    void onMouseRelease(const events::LMBRelease& evt);
    void onMouseReleaseNotHovered(const events::LMBReleaseNotHovered& evt);
    void onMouseEnter(const events::MouseEnter& evt);
    void onMouseExit(const events::MouseExit& evt);

private:
    glm::vec4 color_{1.0f};
    glm::vec4 currentColor_{1.0f};
    glm::vec4 pressedColor_{1.0f};
    glm::vec4 borderColor_{1.0f};
    glm::vec4 disabledColor_{1.0f};

    bool isEnabled_{true};

    TextLabelPtr textLabel_{nullptr};
    ImagePtr image_{nullptr};
};
using ButtonPtr = std::shared_ptr<Button>;
using ButtonWPtr = std::weak_ptr<Button>;
} // namespace msgui
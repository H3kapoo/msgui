#pragma once

#include "AbstractNode.hpp"
#include "core/Texture.hpp"
#include "core/nodeEvent/FocusLost.hpp"
#include "core/nodeEvent/LMBClick.hpp"
#include "core/nodeEvent/LMBRelease.hpp"

namespace msgui
{
class Button : public AbstractNode
{
public:
    Button(const std::string& name);

    Button& setColor(const glm::vec4& color);
    Button& setBorderColor(const glm::vec4& color);
    Button& setTexture(const std::string texturePath);
    Button& setEnabled(const bool value);

    glm::vec4 getColor() const;
    glm::vec4 getBorderColor() const;
    std::string getTexturePath() const;

private:
    Button(const Button&) = delete;
    Button(Button&&) = delete;
    Button& operator=(const Button&) = delete;
    Button& operator=(Button&&) = delete;

    void setShaderAttributes() override;

    void onMouseClick(const nodeevent::LMBClick& evt);
    void onMouseRelease(const nodeevent::LMBRelease& evt);
    void onFocusLost(const nodeevent::FocusLost& evt);

    void setupLayoutReloadables();

private:
    glm::vec4 color_{1.0f};
    glm::vec4 borderColor_{1.0f};
    glm::vec4 disabledColor_{1.0f};
    std::string texturePath_;
    TexturePtr btnTex_;
    bool isEnabled_{true};
};
using ButtonPtr = std::shared_ptr<Button>;
} // namespace msgui
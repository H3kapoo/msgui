#pragma once

#include "msgui/node/AbstractNode.hpp"
#include "msgui/Texture.hpp"
#include "msgui/renderer/Types.hpp"

namespace msgui
{

/* Node used to display text. */
class TextLabel : public AbstractNode
{
public:
    TextLabel(const std::string& name);
    ~TextLabel();

    TextLabel& setColor(const glm::vec4& text);
    TextLabel& setText(const std::string& text);
    TextLabel& setFont(const std::string fontPath);
    TextLabel& setFontSize(const int32_t fontSize);

    glm::vec4 getColor() const;
    std::string getText() const;
    std::string getFont() const;
    int32_t getFontSize() const;

private:
    void setShaderAttributes() override;

    void setupLayoutReloadables();

private:
    glm::vec4 color_{1.0f};
    glm::vec4 borderColor_{1.0f};

    renderer::MaybeTextDataIt textData_{std::nullopt};

    TexturePtr btnTex_{nullptr};
    uint32_t id_{0};
};
using TextLabelPtr = std::shared_ptr<TextLabel>;
using TextLabelWPtr = std::weak_ptr<TextLabel>;
} // namespace msgui
#pragma once

#include "AbstractNode.hpp"
#include "core/Listeners.hpp"
#include "core/Texture.hpp"

namespace msgui
{
class Button : public AbstractNode
{
public:
    Button(const std::string& name);

    Button& setColor(const glm::vec4& color);
    Button& setBorderColor(const glm::vec4& color);
    Button& setTexture(const std::string texturePath);

    glm::vec4 getColor() const;
    glm::vec4 getBorderColor() const;
    std::string getTexturePath() const;
    Listeners& getListeners();

private:
    void setShaderAttributes() override;
    void onMouseButtonNotify() override;

    void setupLayoutReloadables();

private:
    glm::vec4 color_{1.0f};
    glm::vec4 borderColor_{1.0f};
    std::string texturePath_;
    TexturePtr btnTex_;
    Listeners listeners_;
};
using ButtonPtr = std::shared_ptr<Button>;
} // namespace msgui
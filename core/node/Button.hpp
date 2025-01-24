#pragma once

#include "AbstractNode.hpp"
#include "core/Listeners.hpp"
#include "core/Texture.hpp"
#include "core/Utils.hpp"

namespace msgui
{
class Button : public AbstractNode
{
struct Props;
public:

public:
    Button(const std::string& name);

    Props& setColor(const glm::vec4& color);
    Props& setBorderColor(const glm::vec4& color);
    Props& setTexture(const std::string texturePath);

    glm::vec4 getColor() const;
    glm::vec4 getBorderColor() const;
    std::string getTexturePath() const;

private:
    void setShaderAttributes() override;
    void onMouseButtonNotify() override;

    void setupLayoutReloadables();

public:
    Listeners listeners;

private:
    struct Props
    {
        glm::vec4 color{1.0f};
        glm::vec4 borderColor{1.0f};
        std::string texturePath;
    };
    Props props;
    TexturePtr btnTex_;
};
using ButtonPtr = std::shared_ptr<Button>;
} // namespace msgui
#pragma once

#include "AbstractNode.hpp"
#include "msgui/Listeners.hpp"
#include "msgui/Texture.hpp"

namespace msgui
{
class Image : public AbstractNode
{
public:
    Image(const std::string& name);

    Image& setTint(const glm::vec4& color);
    Image& setImage(const std::string& imagePath);

    glm::vec4 getTint() const;
    std::string getImagePath() const;
    Listeners& getListeners();

private:
    void setShaderAttributes() override;

    void setupLayoutReloadables();

private:
    glm::vec4 color_{1.0f};
    glm::vec4 borderColor_{1.0f};
    std::string imagePath_;
    TexturePtr btnTex_{nullptr};
    Listeners listeners_;
};
using ImagePtr = std::shared_ptr<Image>;
} // namespace msgui
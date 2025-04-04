#pragma once

#include "msgui/node/AbstractNode.hpp"
#include "msgui/Texture.hpp"

namespace msgui
{
/* Node used to display an image. */
class Image : public AbstractNode
{
public:
    Image(const std::string& name);

    Image& setTint(const glm::vec4& color);
    Image& setImage(const std::string& imagePath);

    glm::vec4 getTint() const;
    std::string getImagePath() const;

private:
    void setShaderAttributes() override;

    void setupLayoutReloadables();

private:
    glm::vec4 color_{1.0f};
    glm::vec4 borderColor_{1.0f};
    std::string imagePath_;
    TexturePtr btnTex_{nullptr};
    uint32_t id_{0};
};
using ImagePtr = std::shared_ptr<Image>;
using ImageWPtr = std::weak_ptr<Image>;
} // namespace msgui
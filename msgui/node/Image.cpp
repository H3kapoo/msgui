#include "Image.hpp"

#include <GLFW/glfw3.h>

#include "msgui/loaders/MeshLoader.hpp"
#include "msgui/loaders/ShaderLoader.hpp"
#include "msgui/loaders/TextureLoader.hpp"
#include "msgui/Utils.hpp"
#include "msgui/node/FrameState.hpp"

namespace msgui
{
Image::Image(const std::string& name) : AbstractNode(name, NodeType::COMMON)
{
    setShader(loaders::ShaderLoader::loadShader("assets/shader/basicTex.glsl"));
    setMesh(loaders::MeshLoader::loadQuad());
    log_ = ("Image(" + name + ")");

    /* Defaults */
    color_ = Utils::hexToVec4("#f9f8f7ff");

    layout_.setNewScale({100_px, 100_px});
}

void Image::setShaderAttributes()
{
    transform_.computeModelMatrix();
    auto shader = getShader();
    int32_t texId = btnTex_ ? btnTex_->getId() : 0;

    shader->setMat4f("uModelMat", transform_.modelMatrix);
    shader->setVec4f("uColor", color_);
    shader->setVec4f("uBorderColor", borderColor_);
    shader->setVec4f("uBorderSize", layout_.border);
    shader->setVec4f("uBorderRadii", layout_.borderRadius);
    shader->setVec2f("uResolution", glm::vec2{transform_.scale.x, transform_.scale.y});
    shader->setInt("uUseTexture", texId);
    shader->setTexture2D("uTexture", GL_TEXTURE0, texId);
}

Image& Image::setTint(const glm::vec4& color)
{
    color_ = color;
    REQUEST_NEW_FRAME;
    return *this;
}

Image& Image::setImage(const std::string& imagePath)
{
    imagePath_ = imagePath;
    btnTex_ = loaders::TextureLoader::loadTexture(imagePath_);
    REQUEST_NEW_FRAME;
    return *this;
}

std::string Image::getImagePath() const { return imagePath_; }
} // msgui
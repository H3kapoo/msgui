#include "Image.hpp"

#include <GLFW/glfw3.h>
#include <chrono>
#include <thread>

#include "core/MeshLoader.hpp"
#include "core/ShaderLoader.hpp"
#include "core/TextureLoader.hpp"
#include "core/Utils.hpp"
#include "core/node/FrameState.hpp"

namespace msgui
{
Image::Image(const std::string& name) : AbstractNode(name, NodeType::COMMON)
{
    setShader(ShaderLoader::loadShader("assets/shader/basicTex.glsl"));
    setMesh(MeshLoader::loadQuad());
    log_ = ("Image(" + name + ")");

    setupLayoutReloadables();

    color_ = Utils::hexToVec4("#ffffffff");
    // color_ = Utils::hexToVec4("#bbbbbbff");
    // borderColor_ = Utils::hexToVec4("#55bbbbff");
}

void Image::setShaderAttributes()
{
    transform_.computeModelMatrix();
    auto shader = getShader();
    int32_t texId = btnTex_ ? btnTex_->getId() : 0;

    // log_.debugLn("Texid %d", texId);
    shader->setMat4f("uModelMat", transform_.modelMatrix);
    shader->setVec4f("uColor", color_);
    shader->setTexture2D("uTexture", GL_TEXTURE0, texId);
    // shader->setVec4f("uBorderColor", borderColor_);
    // shader->setVec4f("uBorderSize", layout_.border);
    // shader->setVec4f("uBorderRadii", layout_.borderRadius);
    // shader->setVec2f("uResolution", glm::vec2{transform_.scale.x, transform_.scale.y});
}

void Image::setupLayoutReloadables()
{
    auto updateCb = [this ](){ MAKE_LAYOUT_DIRTY_AND_REQUEST_NEW_FRAME };

    layout_.onAlignSelfChange = updateCb;
    layout_.onMarginChange = updateCb;
    layout_.onBorderChange = updateCb;
    layout_.onScaleTypeChange = updateCb;
    layout_.onScaleChange = updateCb;
}

// Image& Image::setColor(const glm::vec4& color)
// {
//     color_ = color;
//     return *this;
// }

// Image& Image::setBorderColor(const glm::vec4& color)
// {
//     borderColor_ = color;
//     return *this;
// }

Image& Image::setImage(const std::string& imagePath)
{
    imagePath_ = imagePath;
    btnTex_ = TextureLoader::loadTexture(imagePath_);
    MAKE_LAYOUT_DIRTY_AND_REQUEST_NEW_FRAME
    return *this;
}

// glm::vec4 Image::getColor() const { return color_; }

// glm::vec4 Image::getBorderColor() const { return borderColor_; }

std::string Image::getImagePath() const { return imagePath_; }

Listeners& Image::getListeners() { return listeners_; }
} // msgui
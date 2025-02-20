#include "Image.hpp"

#include <GLFW/glfw3.h>

#include "msgui/MeshLoader.hpp"
#include "msgui/ShaderLoader.hpp"
#include "msgui/TextureLoader.hpp"
#include "msgui/Utils.hpp"
#include "msgui/node/FrameState.hpp"

namespace msgui
{
Image::Image(const std::string& name) : AbstractNode(name, NodeType::COMMON)
{
    setShader(ShaderLoader::loadShader("assets/shader/basicTex.glsl"));
    setMesh(MeshLoader::loadQuad());
    log_ = ("Image(" + name + ")");

    setupLayoutReloadables();

    /* Defaults */
    color_ = Utils::hexToVec4("#F9F8F7");

    layout_.setBorder({1});
    layout_.setBorderRadius({4});
    layout_.setScale({70, 34});
}

void Image::setShaderAttributes()
{
    transform_.computeModelMatrix();
    auto shader = getShader();
    int32_t texId = btnTex_ ? btnTex_->getId() : 0;

    shader->setMat4f("uModelMat", transform_.modelMatrix);
    shader->setVec4f("uColor", color_);
    shader->setInt("uUseTexture", texId);
    shader->setTexture2D("uTexture", GL_TEXTURE0, texId);
}

void Image::setupLayoutReloadables()
{
    auto updateCb = [this](){ MAKE_LAYOUT_DIRTY_AND_REQUEST_NEW_FRAME };

    /* Layout will auto recalculate and new frame will be requested on layout data changes. */
    layout_.onMarginChange = updateCb;
    layout_.onPaddingChange = updateCb;
    layout_.onBorderChange = updateCb;
    layout_.onBorderRadiusChange = updateCb;
    layout_.onAlignSelfChange = updateCb;
    layout_.onScaleTypeChange = updateCb;
    layout_.onGridStartRCChange = updateCb;
    layout_.onGridSpanRCChange = updateCb;
    layout_.onScaleChange = updateCb;
    layout_.onMinScaleChange = updateCb;
    layout_.onMaxScaleChange = updateCb;
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
    btnTex_ = TextureLoader::loadTexture(imagePath_);
    REQUEST_NEW_FRAME;
    return *this;
}

std::string Image::getImagePath() const { return imagePath_; }
} // msgui
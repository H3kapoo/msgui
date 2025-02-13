#include "Image.hpp"

#include <GLFW/glfw3.h>

#include "core/MeshLoader.hpp"
#include "core/ShaderLoader.hpp"
#include "core/TextureLoader.hpp"
#include "core/Utils.hpp"
#include "core/node/FrameState.hpp"
#include "core/nodeEvent/LMBClick.hpp"

namespace msgui
{
Image::Image(const std::string& name) : AbstractNode(name, NodeType::COMMON)
{
    setShader(ShaderLoader::loadShader("assets/shader/basicTex.glsl"));
    setMesh(MeshLoader::loadQuad());
    log_ = ("Image(" + name + ")");

    setupLayoutReloadables();

    color_ = Utils::hexToVec4("#ffffffff");
}

void Image::setShaderAttributes()
{
    transform_.computeModelMatrix();
    auto shader = getShader();
    int32_t texId = btnTex_ ? btnTex_->getId() : 0;

    shader->setMat4f("uModelMat", transform_.modelMatrix);
    shader->setVec4f("uColor", color_);
    shader->setTexture2D("uTexture", GL_TEXTURE0, texId);
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

Image& Image::setTint(const glm::vec4& color)
{
    color_ = color;
    MAKE_LAYOUT_DIRTY_AND_REQUEST_NEW_FRAME
    return *this;
}

Image& Image::setImage(const std::string& imagePath)
{
    imagePath_ = imagePath;
    btnTex_ = TextureLoader::loadTexture(imagePath_);
    MAKE_LAYOUT_DIRTY_AND_REQUEST_NEW_FRAME
    return *this;
}

std::string Image::getImagePath() const { return imagePath_; }

Listeners& Image::getListeners() { return listeners_; }
} // msgui
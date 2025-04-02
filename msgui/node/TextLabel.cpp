#include "TextLabel.hpp"

#include <GLFW/glfw3.h>

#include "msgui/MeshLoader.hpp"
#include "msgui/ShaderLoader.hpp"
#include "msgui/Utils.hpp"
#include "msgui/node/FrameState.hpp"
#include "msgui/renderer/text/TextBufferStore.hpp"
#include "msgui/renderer/text/Types.hpp"

namespace msgui
{
TextLabel::TextLabel(const std::string& name) : AbstractNode(name, NodeType::COMMON)
{
    setShader(ShaderLoader::loadShader("assets/shader/sdfRect.glsl"));
    setMesh(MeshLoader::loadQuad());
    log_ = ("TextLabel(" + name + ")");

    setupLayoutReloadables();

    /* Defaults */
    color_ = Utils::hexToVec4("#ad0f0f");

    layout_.setScale({100, 100});
    // color_ = Utils::hexToVec4("#ff00ffff");
}

void TextLabel::setShaderAttributes()
{
    transform_.computeModelMatrix();
    auto shader = getShader();

    shader->setMat4f("uModelMat", transform_.modelMatrix);
    shader->setVec4f("uColor", color_);
    shader->setVec4f("uBorderColor", borderColor_);
    shader->setVec4f("uBorderSize", layout_.border);
    shader->setVec4f("uBorderRadii", layout_.borderRadius);
    shader->setVec2f("uResolution", glm::vec2{transform_.scale.x, transform_.scale.y});
}

void TextLabel::setupLayoutReloadables()
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

TextLabel& TextLabel::setText(const std::string& text)
{
    if (!textData_) { textData_ = TextBufferStore::get().newLocation(); }

    textData_.value()->transformPtr = &transform_;
    textData_.value()->text = std::move(text);
    textData_.value()->isDirty = true;

    if (auto state = getState())
    {
        state->layoutPassActions |= ELayoutPass::EVERYTHING_TEXT;
    }

    REQUEST_NEW_FRAME;
    return *this;
}

std::string TextLabel::getText() const { return textData_ ? textData_.value()->text : ""; }
} // msgui
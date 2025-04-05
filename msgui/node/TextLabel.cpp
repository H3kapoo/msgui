#include "TextLabel.hpp"

#include <GLFW/glfw3.h>

#include "msgui/MeshLoader.hpp"
#include "msgui/ShaderLoader.hpp"
#include "msgui/Utils.hpp"
#include "msgui/loaders/FontLoader.hpp"
#include "msgui/node/FrameState.hpp"
#include "msgui/renderer/text/TextBufferStore.hpp"
#include "msgui/renderer/text/Types.hpp"

namespace msgui
{
using namespace loaders;

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

TextLabel::~TextLabel()
{
    if (!textData_) { return; }

    TextBufferStore::get().remove(textData_);
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

TextLabel& TextLabel::setColor(const glm::vec4& color)
{
    color_ = color;
    REQUEST_NEW_FRAME;

    return *this;
}

TextLabel& TextLabel::setText(const std::string& text)
{
    if (!textData_)
    {
        textData_ = TextBufferStore::get().newLocation();
        textData_.value()->transformPtr = &transform_;
        textData_.value()->fontData = FontLoader::get().loadFont(DEFAULT_FONT_PATH, DEFAULT_FONT_SIZE);
    }

    textData_.value()->text = std::move(text);
    textData_.value()->isDirty = true;

    // MAKE_TEXT_LAYOUT_DIRTY;
    REQUEST_NEW_FRAME;
    return *this;
}

TextLabel& TextLabel::setFont(const std::string fontPath)
{
    if (!textData_) { return *this; }
    if (textData_.value()->fontData->fontPath == fontPath) { return *this; }

    const int32_t previousFontSize = textData_.value()->fontData->fontSize;
    const auto loadedFont = FontLoader::get().loadFont(fontPath, previousFontSize);
    if (!loadedFont->texId) { return *this; }

    textData_.value()->fontData = loadedFont;
    textData_.value()->isDirty = true;

    // MAKE_TEXT_LAYOUT_DIRTY;
    REQUEST_NEW_FRAME;
    return *this;
}

TextLabel& TextLabel::setFontSize(const int32_t fontSize)
{
    if (!textData_) { return *this; }
    if (textData_.value()->fontData->fontSize == fontSize) { return *this; }

    const std::string previousFontPath = textData_.value()->fontData->fontPath;
    const auto loadedFont = FontLoader::get().loadFont(previousFontPath, fontSize);
    if (!loadedFont->texId) { return *this; }

    textData_.value()->fontData = loadedFont;
    textData_.value()->isDirty = true;

    // MAKE_TEXT_LAYOUT_DIRTY;
    REQUEST_NEW_FRAME;
    return *this;
}

glm::vec4 TextLabel::getColor() const { return color_; }

std::string TextLabel::getText() const { return textData_ ? textData_.value()->text : ""; }

std::string TextLabel::getFont() const { return textData_ ? textData_.value()->fontData->fontPath : "?"; }

int32_t TextLabel::getFontSize() const { return textData_ ? textData_.value()->fontData->fontSize : -1; }
} // msgui
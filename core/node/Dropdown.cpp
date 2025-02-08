#include "Dropdown.hpp"

#include <GLFW/glfw3.h>

#include "core/MeshLoader.hpp"
#include "core/ShaderLoader.hpp"
#include "core/Utils.hpp"
#include "core/node/FrameState.hpp"
#include "core/node/utils/ScrollBar.hpp"

namespace msgui
{
Dropdown::Dropdown(const std::string& name) : AbstractNode(name, NodeType::DROPDOWN)
{
    setShader(ShaderLoader::loadShader("assets/shader/sdfRect.glsl"));
    setMesh(MeshLoader::loadQuad());
    log_ = ("Dropdown(" + name + ")");

    setupLayoutReloadables();

    color_ = Utils::hexToVec4("#ffffffff");

    // container_ = Utils::make<Box>("BoxIn");
    container_ = Utils::make<Box>("BoxIn");
    container_->setColor(Utils::hexToVec4("#48ff00ff"));
    container_->getLayout()
        .setType(Layout::Type::VERTICAL)
        .setAllowOverflow({false, true})
        .setScaleType(Layout::ScaleType::ABS)
        .setScale({100, 100})
        .setPadding(Layout::TBLR{0, 0, 2, 2})
        // .setMargin({10, 10, 60, 10})
        ;

    container_->getListeners().setOnMouseButtonLeftClick([this]()
    {
        log_.debugLn("clicked box");
    });
}

void Dropdown::setShaderAttributes()
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

void Dropdown::onMouseButtonNotify()
{   
    const auto& state = getState();
    int32_t clicked = state->mouseButtonState[GLFW_MOUSE_BUTTON_LEFT];
    log_.debugLn("state %d", clicked);
    if (!clicked && this == state->hoveredNodePtr.get())
    {
        dropdownOpen_ = !dropdownOpen_;
    }

    if (!dropdownOpen_ && getChildren().size())
    {
        // if (container_->isScrollBarActive(ScrollBar::Orientation::VERTICAL))
        // {
        //     container_->getScrollBar(ScrollBar::Orientation::VERTICAL)->setScrollCurrentValue(0);
        // }
        remove(container_->getId());
        container_.reset();
    }
    else if (dropdownOpen_ && !getChildren().size())
    {
        append(container_);
    }
}

void Dropdown::addMenuItem(const AbstractNodePtr& nodeItem)
{
    container_->append(nodeItem);
}

void Dropdown::setupLayoutReloadables()
{
    auto updateCb = [this ](){ MAKE_LAYOUT_DIRTY_AND_REQUEST_NEW_FRAME };

    layout_.onAlignSelfChange = updateCb;
    layout_.onMarginChange = updateCb;
    layout_.onBorderChange = updateCb;
    layout_.onScaleTypeChange = updateCb;
    layout_.onScaleChange = updateCb;
}

Dropdown& Dropdown::setColor(const glm::vec4& color)
{
    color_ = color;
    MAKE_LAYOUT_DIRTY_AND_REQUEST_NEW_FRAME
    return *this;
}

Dropdown& Dropdown::setBorderColor(const glm::vec4& color)
{
    color_ = color;
    MAKE_LAYOUT_DIRTY_AND_REQUEST_NEW_FRAME
    return *this;
}

Listeners& Dropdown::getListeners() { return listeners_; }
} // msgui
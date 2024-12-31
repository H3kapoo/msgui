#include "ScrollBar.hpp"

namespace msgui
{
ScrollBar::ScrollBar(const ScrollBar::Orientation orientation)
    : AbstractNode(MeshLoader::loadQuad(), ShaderLoader::load("assets/shader/basic.glsl"),
        "TestBar1", NodeType::SCROLL)
    , orientation_(orientation)
{
    knob_ = std::make_shared<ScrollBarKnob>();
    // temporary hardcoded sizes
    knob_->getTransform().setScale({20, 20, 1});
    append(knob_);
}

// ---- Overrides ---- //
void* ScrollBar::getProps()
{
    return nullptr;
}

// ---- Getters ---- //
float ScrollBar::getOffset()
{
    return offset_;
}

ScrollBar::Orientation ScrollBar::getOrientation()
{
    return orientation_;
}

// ---- Overrides Private ---- //
void ScrollBar::setShaderAttributes()
{
    transform_.computeModelMatrix();
    shader_->setVec4f("uColor", color_);
    shader_->setMat4f("uModelMat", transform_.modelMatrix);
}

// ---- Virtual Event Listeners ---- //
void ScrollBar::onMouseButtonNotify()
{
    if (state_->mouseButtonState[GLFW_MOUSE_BUTTON_LEFT])
    {
        return;
    }

    // offset: 0.........1
    // size:  top_xy........bottom_xy

    if (orientation_ == Orientation::VERTICAL)
    {
        offset_ = Utils::remap(state_->mouseY,
            transform_.pos.y + 10, transform_.pos.y + transform_.scale.y - 10, 0.0f, 1.0f);
    }
    else if (orientation_ == Orientation::HORIZONTAL)
    {
        offset_ = Utils::remap(state_->mouseX,
            transform_.pos.x + 10, transform_.pos.x + transform_.scale.x - 10, 0.0f, 1.0f);
    }
    log_.infoLn("clicked %f", offset_);

    state_->isLayoutDirty = true;
}

} // namespace msgui
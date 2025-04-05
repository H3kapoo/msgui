#include "ScrollBarKnob.hpp"

#include "msgui/loaders/MeshLoader.hpp"
#include "msgui/loaders/ShaderLoader.hpp"
#include "msgui/node/AbstractNode.hpp"
#include "msgui/layoutEngine/utils/LayoutData.hpp"
#include "msgui/node/utils/ScrollBar.hpp"

namespace msgui
{
ScrollBarKnob::ScrollBarKnob() : AbstractNode("ScrollBarKnob", NodeType::SCROLL_KNOB)
{
    setShader(loaders::ShaderLoader::loadShader("assets/shader/sdfRect.glsl"));
    setMesh(loaders::MeshLoader::loadQuad());

    getLayout().border = utils::Layout::TBLR{4};
    getLayout().borderRadius = utils::Layout::TBLR{8};

    getEvents().listen<events::LMBClick, events::InputChannel>(
        std::bind(&ScrollBarKnob::onMouseClick, this, std::placeholders::_1));
    getEvents().listen<events::LMBDrag, events::InputChannel>(
        std::bind(&ScrollBarKnob::onMouseDrag, this, std::placeholders::_1));
}

void ScrollBarKnob::setShaderAttributes()
{
    transform_.computeModelMatrix();
    auto shader = getShader();
    shader->setMat4f("uModelMat", transform_.modelMatrix);
    shader->setVec4f("uColor", color_);
    shader->setVec4f("uBorderColor", borderColor_);
    shader->setVec4f("uBorderSize", getLayout().border);
    shader->setVec4f("uBorderRadii", getLayout().borderRadius);
    shader->setVec2f("uResolution", glm::vec2{transform_.scale.x, transform_.scale.y});
}

void ScrollBarKnob::onMouseClick(const events::LMBClick&)
{
    /* Pass-through to parent */
    AbstractNodePtr sbParent = parent_.lock();
    if (!sbParent) { return; }

    ScrollBar* sbParentRaw = static_cast<ScrollBar*>(sbParent.get());
    if (!sbParentRaw) { return; }

    events::LMBClick evt;
    sbParentRaw->getEvents().notifyEvent<events::LMBClick, events::InternalChannel>(evt);
}

void ScrollBarKnob::onMouseDrag(const events::LMBDrag& evtIn)
{
    /* Pass-through to parent */
    AbstractNodePtr sbParent = parent_.lock();
    if (!sbParent) { return; }

    ScrollBar* sbParentRaw = static_cast<ScrollBar*>(sbParent.get());
    if (!sbParentRaw) { return; }

    events::LMBDrag evt(evtIn);
    sbParentRaw->getEvents().notifyEvent<events::LMBDrag, events::InternalChannel>(evt);
}

ScrollBarKnob& ScrollBarKnob::setColor(const glm::vec4& color)
{
    color_ = color;
    return *this;
}

ScrollBarKnob& ScrollBarKnob::setBorderColor(const glm::vec4& color)
{
    borderColor_ = color;
    return *this;
}

glm::vec4 ScrollBarKnob::getColor() const { return color_; }

glm::vec4 ScrollBarKnob::getBorderColor() const { return borderColor_; }
} // msgui
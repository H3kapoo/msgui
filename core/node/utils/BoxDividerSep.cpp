#include "BoxDividerSep.hpp"

#include "core/MeshLoader.hpp"
#include "core/ShaderLoader.hpp"
#include "core/node/AbstractNode.hpp"
#include "core/node/Box.hpp"
#include "core/node/FrameState.hpp"

namespace msgui
{
BoxDividerSep::BoxDividerSep(const std::string& name, const BoxPtr& firstBox, const BoxPtr& secondBox)
    : AbstractNode(MeshLoader::loadQuad(), ShaderLoader::load("assets/shader/sdfRect.glsl"),
        name, NodeType::BOX_DIVIDER_SEP)
    , firstBox_(firstBox)
    , secondBox_(secondBox)
{
    log_ = ("BoxDividerSep(" + name + ")");

    //TODO: Box divider should not be "active" with < 2 boxes
    setupReloadables();

    props.color = Utils::hexToVec4("#52161bff");
    props.layout.type = Layout::Type::HORIZONTAL;
    props.layout.scaleType = {Layout::ScaleType::ABS, Layout::ScaleType::REL};
    props.layout.scale = {10, 1.0f};
}

void BoxDividerSep::setShaderAttributes()
{
    transform_.computeModelMatrix();
    shader_->setMat4f("uModelMat", transform_.modelMatrix);
    shader_->setVec4f("uColor", props.color);
    shader_->setVec4f("uBorderColor", props.borderColor);
    shader_->setVec4f("uBorderSize", props.layout.border.value);
    shader_->setVec4f("uBorderRadii", props.layout.borderRadius.value);
    shader_->setVec2f("uResolution", glm::vec2{transform_.scale.x, transform_.scale.y});
}

void* BoxDividerSep::getProps() { return (void*)&props; }

void BoxDividerSep::onMouseButtonNotify() {}

void BoxDividerSep::onMouseDragNotify()
{
    float diff = state_->mouseX - state_->lastMouseX;

    Layout* left = static_cast<Layout*>(firstBox_->getProps());
    Layout* right = static_cast<Layout*>(secondBox_->getProps());

    // Temp is used here as we don't want to modify the original scale supplied by the user
    // Maybe there's a better way to do it..later.
    left->tempScale.x += diff;
    right->tempScale.x -= diff;
    activeNow_ = true;

    MAKE_LAYOUT_DIRTY
}

void BoxDividerSep::setupReloadables()
{
    auto updateCb = [this ](){ MAKE_LAYOUT_DIRTY_AND_REQUEST_NEW_FRAME };

    props.layout.alignSelf.onReload = updateCb;
    props.layout.margin.onReload = updateCb;
    props.layout.border.onReload = updateCb;
    props.layout.scaleType.onReload = updateCb;
    props.layout.scale.onReload = updateCb;
}
} // msgui
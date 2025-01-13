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
    // if (state_->mouseX > transform_.pos.x + transform_.scale.x ||
    // state_->mouseX < transform_.pos.x)
    // {
    //     return;
    // }

    //Note: this func will be called more times than processLayout()
    // so thats why doing the proportional calculations there appeared
    // "slow" changing. 
    float incBy = (state_->lastMouseX - state_->mouseX) / getParent().lock()->getTransform().scale.x;

    Layout* left = static_cast<Layout*>(firstBox_->getProps());
    Layout* right = static_cast<Layout*>(secondBox_->getProps());

    // if ((right->scale.value.x + incBy >= right->minScale.value.x) &&
    //     (left->scale.value.x - incBy >= left->minScale.value.x))
    {
        // log_.debugLn("here");
        left->scale.value.x -= incBy;
        right->scale.value.x += incBy;
    }

    // MAKE_LAYOUT_DIRTY_AND_REQUEST_NEW_FRAME;
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
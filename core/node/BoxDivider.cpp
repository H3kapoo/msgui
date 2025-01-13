#include "BoxDivider.hpp"

#include "core/MeshLoader.hpp"
#include "core/ShaderLoader.hpp"
#include "core/node/AbstractNode.hpp"
#include "core/node/Box.hpp"
#include "core/node/utils/BoxDividerSep.hpp"

namespace msgui
{
BoxDivider::BoxDivider(const std::string& name)
    : AbstractNode(MeshLoader::loadQuad(), ShaderLoader::load("assets/shader/sdfRect.glsl"), name, NodeType::BOX_DIVIDER)
{
    log_ = ("BoxDivider(" + name + ")");

    //TODO: Box divider should not be "active" with < 2 boxes
    setupReloadables();

    // props.color = Utils::hexToVec4("#bbbbbbff");
    // props.borderColor = Utils::hexToVec4("#55bbbbff");
    // props.layout.border = Layout::TBLR{5, 2, 5, 2};
    // props.layout.border = Layout::TBLR{2, 5, 2, 5};
}

void BoxDivider::append(const BoxPtr& box)
{
    AbstractNode::append(box);
}

void BoxDivider::appendMany(const std::initializer_list<BoxPtr>& boxes)
{   
    std::vector<BoxPtr> v{boxes}; // to be redone
    int32_t end = v.size();
    for (int32_t i = 0; i < end; i++)
    {
        AbstractNode::append(v[i]);

        if (i == 0 || i != end-1)
        {
            AbstractNode::append(
                std::make_shared<BoxDividerSep>("BoxDividerSep" + std::to_string(i),
                    v[i], v[i+1]));
        }
    }
}

void BoxDivider::setShaderAttributes()
{
    transform_.computeModelMatrix();
    shader_->setMat4f("uModelMat", transform_.modelMatrix);
    shader_->setVec4f("uColor", props.color);
    shader_->setVec4f("uBorderColor", props.borderColor);
    shader_->setVec4f("uBorderSize", props.layout.border.value);
    shader_->setVec4f("uBorderRadii", props.layout.borderRadius.value);
    shader_->setVec2f("uResolution", glm::vec2{transform_.scale.x, transform_.scale.y});
}

void* BoxDivider::getProps()
{
    return (void*)&props;
}

void BoxDivider::onMouseButtonNotify()
{
    // User custom behavior
    // listeners.callOnMouseButton(
    //     state_->lastMouseButtonTriggeredIdx,
    //     state_->mouseButtonState[state_->lastMouseButtonTriggeredIdx],
    //     state_->mouseX,
    //     state_->mouseY);
}

void BoxDivider::setupReloadables()
{
    auto updateCb = [this ](){ MAKE_LAYOUT_DIRTY_AND_REQUEST_NEW_FRAME };

    props.layout.alignSelf.onReload = updateCb;
    props.layout.margin.onReload = updateCb;
    props.layout.border.onReload = updateCb;
    props.layout.scaleType.onReload = updateCb;
    props.layout.scale.onReload = updateCb;
}
} // msgui
#include "BoxDivider.hpp"

#include "core/MeshLoader.hpp"
#include "core/ShaderLoader.hpp"
#include "core/node/AbstractNode.hpp"
#include "core/node/Box.hpp"
#include "core/node/FrameState.hpp"
#include "core/node/utils/BoxDividerSep.hpp"
#include <iterator>

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

void BoxDivider::appendBoxContainers(const std::initializer_list<BoxPtr>& boxes)
{
    // Minimum of 2 containers needed
    // Order is BOX SEP BOX SEP BOX.. always ending with a box.
    int32_t size = boxes.size();
    for (int32_t i = 0; i < size; i++)
    {
        auto thisBoxIt = std::next(boxes.begin(), i);
        AbstractNode::append(*thisBoxIt);

        (*thisBoxIt)->props.layout
            .setScaleType({Layout::ScaleType::REL, Layout::ScaleType::REL});

        if (i == 0 || i != size - 1)
        {
            auto nextBoxIt = std::next(boxes.begin(), i + 1);

            // Each separator holds ref to current and next box in order to modify their relative
            // scale accoding to mouse movement from user.
            // Unfortunatelly we cannot parent them directly to Sep node as we need ref to both current
            // and previous node for each sep.. and current node can be the prev of the next sep.
            // We cannot parent nodes to 2 nodes.
            BoxDividerSepPtr sep = std::make_shared<BoxDividerSep>(
                "BoxDividerSep" + std::to_string(i),
                *thisBoxIt,
                *nextBoxIt);
            AbstractNode::append(sep);
        }
    }
    props.layout._onTypeChange();
}

void BoxDivider::setShaderAttributes()
{
    transform_.computeModelMatrix();
    shader_->setMat4f("uModelMat", transform_.modelMatrix);
    shader_->setVec4f("uColor", props.color);
    shader_->setVec4f("uBorderColor", props.borderColor);
    shader_->setVec4f("uBorderSize", props.layout.border);
    shader_->setVec4f("uBorderRadii", props.layout.borderRadius);
    shader_->setVec2f("uResolution", glm::vec2{transform_.scale.x, transform_.scale.y});
}

void* BoxDivider::getProps() { return (void*)&props; }

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
    props.layout._onTypeChange = [this]()
    {
        for (auto& ch : getChildren())
        {
            if (ch->getType() == AbstractNode::NodeType::BOX_DIVIDER_SEP)
            {
                auto sep = static_cast<BoxDividerSep*>(ch.get());
                sep->props.layout.setType(props.layout.type);
            }
        }

        MAKE_LAYOUT_DIRTY
    };

    auto updateCb = [this ](){ MAKE_LAYOUT_DIRTY_AND_REQUEST_NEW_FRAME };

    props.layout._onAlignSelfChange = updateCb;
    props.layout._onMarginChange = updateCb;
    props.layout._onBorderChange = updateCb;
    props.layout._onScaleTypeChange = updateCb;
    props.layout._onScaleChange = updateCb;
}
} // msgui
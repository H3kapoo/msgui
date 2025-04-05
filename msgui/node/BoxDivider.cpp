#include "BoxDivider.hpp"

#include "msgui/loaders/MeshLoader.hpp"
#include "msgui/loaders/ShaderLoader.hpp"
#include "msgui/node/AbstractNode.hpp"
#include "msgui/node/Box.hpp"
#include "msgui/node/FrameState.hpp"
#include "msgui/node/utils/BoxDividerSep.hpp"
#include <memory>

namespace msgui
{
BoxDivider::BoxDivider(const std::string& name) : AbstractNode(name, NodeType::BOX_DIVIDER)
{
    setShader(loaders::ShaderLoader::loadShader("assets/shader/sdfRect.glsl"));
    setMesh(loaders::MeshLoader::loadQuad());
    log_ = ("BoxDivider(" + name + ")");

    //TODO: Box divider should not be "active" with < 2 boxes
    setupLayoutReloadables();
}

void BoxDivider::createSlots(uint32_t slotCount, std::vector<float> initialPercSize)
{
    //TODO: When this gets called again (although it shouldn't for now) reset all children.
    std::vector<BoxPtr> boxes;
    for (uint32_t i = 0; i < slotCount; i++)
    {
        auto ref = boxes.emplace_back(std::make_shared<Box>("Box" + std::to_string(i)));
        if (layout_.type == utils::Layout::Type::HORIZONTAL)
        {
            ref->getLayout().scale = {initialPercSize[i], 1.0f};
        }
        else if (layout_.type == utils::Layout::Type::VERTICAL)
        {
            ref->getLayout().scale = {1.0f, initialPercSize[i]};
        }
    }

    appendBoxContainers(boxes);
}

void BoxDivider::setShaderAttributes()
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

void BoxDivider::appendBoxContainers(const std::vector<BoxPtr>& boxes)
{
    // Minimum of 2 containers needed
    // Order is BOX SEP BOX SEP BOX.. always ending with a box.
    int32_t size = boxes.size();
    for (int32_t i = 0; i < size; i++)
    {
        auto thisBoxIt = std::next(boxes.begin(), i);
        AbstractNode::append(*thisBoxIt);

        (*thisBoxIt)->getLayout()
            .setScaleType({utils::Layout::ScaleType::REL, utils::Layout::ScaleType::REL});

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
            sep->getLayout().setType(layout_.type);
            AbstractNode::append(sep);
        }
    }
}

void BoxDivider::setupLayoutReloadables()
{
    layout_.onTypeChange = [this]()
    {
        for (auto& ch : getChildren())
        {
            if (ch->getType() == AbstractNode::NodeType::BOX_DIVIDER_SEP)
            {
                auto sep = Utils::as<BoxDividerSep>(ch);
                sep->getLayout().setType(layout_.type);
            }
            else
            {
                auto box = Utils::as<Box>(ch);
                std::swap(box->getLayout().scale.x, box->getLayout().scale.y);
            }
        }

        MAKE_LAYOUT_DIRTY
    };

    auto updateCb = [this ](){ MAKE_LAYOUT_DIRTY };

    layout_.onAlignSelfChange = updateCb;
    layout_.onMarginChange = updateCb;
    layout_.onBorderChange = updateCb;
    layout_.onScaleTypeChange = updateCb;
    layout_.onScaleChange = updateCb;
}

BoxWPtr BoxDivider::getSlot(uint32_t slotNumber)
{
    uint32_t idx = slotNumber * 2;
    if (idx < children_.size())
    {
        /* Guaranteed to be Box type */
        return Utils::ref<Box>(std::static_pointer_cast<Box>(children_[idx]));
    }
    return Utils::ref<Box>();
}

BoxDividerSepWPtr BoxDivider::getSepatator(uint32_t sepNumber)
{
    uint32_t idx = sepNumber * 2 + 1;
    if (idx < children_.size() - 1)
    {
        /* Guaranteed to be BoxDividerSep type */
        return Utils::ref<BoxDividerSep>(std::static_pointer_cast<BoxDividerSep>(children_[idx]));
    }
    return Utils::ref<BoxDividerSep>();
}

BoxDivider& BoxDivider::setColor(const glm::vec4& color)
{
    color_ = color;
    return *this;
}

BoxDivider& BoxDivider::setBorderColor(const glm::vec4& color)
{
    borderColor_ = color;
    return *this;
}

glm::vec4 BoxDivider::getColor() const { return color_; }

glm::vec4 BoxDivider::getBorderColor() const { return borderColor_; }
} // msgui
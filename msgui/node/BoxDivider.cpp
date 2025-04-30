#include "BoxDivider.hpp"

#include "msgui/loaders/MeshLoader.hpp"
#include "msgui/loaders/ShaderLoader.hpp"
#include "msgui/node/AbstractNode.hpp"
#include "msgui/node/Box.hpp"
#include "msgui/node/FrameState.hpp"
#include "msgui/node/utils/BoxDividerSep.hpp"

namespace msgui
{
BoxDivider::BoxDivider(const std::string& name) : AbstractNode(name, NodeType::BOX_DIVIDER)
{
    setShader(loaders::ShaderLoader::loadShader("assets/shader/sdfRect.glsl"));
    setMesh(loaders::MeshLoader::loadQuad());
    log_ = ("BoxDivider(" + name + ")");

    setupLayoutReloadables();
}

void BoxDivider::createSlots(const std::vector<Layout::Scale>& initialScale)
{
    if (getChildren().size())
    {
        log_.infoLn("Resetting slots to new ones..");
        removeAll();
    }

    std::vector<BoxPtr> boxes;
    for (uint32_t i = 0; i < initialScale.size(); i++)
    {
        auto ref = boxes.emplace_back(std::make_shared<Box>("Box" + std::to_string(i)));
        ref->setColor(Utils::randomRGB());
        if (layout_.type == utils::Layout::Type::HORIZONTAL)
        {
            ref->getLayout().setNewScale({initialScale[i], 0.9_rel});
        }
        else if (layout_.type == utils::Layout::Type::VERTICAL)
        {
            ref->getLayout().setNewScale({1.0_rel, initialScale[i]});
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
    /*
        Minimum of 2 containers needed.
        The order is always BOX SEP BOX SEP BOX ..
    */
    if (boxes.size() < 2) { return; }

    int32_t size = boxes.size();
    for (int32_t i = 0; i < size; i++)
    {
        auto thisBoxIt = std::next(boxes.begin(), i);
        append(*thisBoxIt);

        if (i == 0 || i != size - 1)
        {
            auto nextBoxIt = std::next(boxes.begin(), i + 1);

            /*
                Each separator holds a reference to the current and next box in order to modify their relative
                scale accoding to mouse movement from user.
                Unfortunatelly we cannot parent them directly to BoxDividerSep node as we need ref to both current
                and previous node for each separator and current box node can be the previous of the next separator.
                We cannot parent nodes to 2 nodes.
            */
            BoxDividerSepPtr sep = std::make_shared<BoxDividerSep>(
                "BoxDividerSep" + std::to_string(i),
                *thisBoxIt,
                *nextBoxIt);
            sep->getLayout().setType(layout_.type);
            append(sep);
        }
    }
}

void BoxDivider::setupLayoutReloadables()
{
    return;
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
}

BoxWPtr BoxDivider::getSlot(uint32_t slotNumber)
{
    uint32_t idx = slotNumber * 2;
    if (idx < children_.size())
    {
        /* Guaranteed to be Box type */
        return Utils::ref<Box>(Utils::as<Box>(children_[idx]));
    }
    return Utils::ref<Box>();
}

BoxDividerSepWPtr BoxDivider::getSepatator(uint32_t sepNumber)
{
    uint32_t idx = sepNumber * 2 + 1;
    if (idx < children_.size() - 1)
    {
        /* Guaranteed to be BoxDividerSep type */
        return Utils::ref<BoxDividerSep>(Utils::as<BoxDividerSep>(children_[idx]));
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
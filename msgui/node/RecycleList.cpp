#include "RecycleList.hpp"

#include "msgui/loaders//MeshLoader.hpp"
#include "msgui/loaders//ShaderLoader.hpp"
#include "msgui/node/AbstractNode.hpp"
#include "msgui/node/Button.hpp"
#include "msgui/node/FrameState.hpp"
#include "msgui/events/LMBItemRelease.hpp"

namespace msgui
{
RecycleList::RecycleList(const std::string& name) : Box(name)
{
    /* Defaults */
    log_ = Logger("RecycleList(" + name + ")");
    setType(AbstractNode::NodeType::RECYCLE_LIST);
    setShader(loaders::ShaderLoader::loadShader("assets/shader/sdfRect.glsl"));
    setMesh(loaders::MeshLoader::loadQuad());

    color_ = Utils::hexToVec4("#ffffffff");
    layout_.setAllowOverflow({true, true})
        .setType(Layout::Type::VERTICAL)
        .setNewScale({100_px, 100_px});
}

void RecycleList::addItem(const node::utils::ListItem& item)
{
    listItems_.emplace_back(item);
    internals_.isDirty = true;
    MAKE_LAYOUT_DIRTY;

    /* Small helper to trigger addition of a node the first time RecycleList starts. */
    if (internals_.visibleNodes == 0)
    {
        internals_.visibleNodes = 1;
        onLayoutDirtyPost();
    }
}

void RecycleList::removeItemIdx(const int32_t idx)
{
    if (idx < 0 || idx > (int32_t)listItems_.size() - 1) { return; }
    listItems_.erase(listItems_.begin() + idx);

    internals_.isDirty = true;
    MAKE_LAYOUT_DIRTY;
}

void RecycleList::removeItemsBy(const std::function<bool(const node::utils::ListItem&)> pred)
{
    if (std::erase_if(listItems_, pred))
    {
        internals_.isDirty = true;
        MAKE_LAYOUT_DIRTY;
    }
}

void RecycleList::setShaderAttributes()
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

void RecycleList::onLayoutDirtyPost()
{
    removeAll();

    internals_.elementsCount = listItems_.size();
    for (int32_t i = 0; i < internals_.visibleNodes; i++)
    {
        int32_t index = internals_.topOfListIdx + i;
        if (index >= internals_.elementsCount) { break; }

        // auto ref = std::make_shared<TextLabel>("TreeViewItem");
        auto ref = std::make_shared<Button>("Item");
        ref->setColor(listItems_[index].color)
            .setText(listItems_[index].text);

        ref->getLayout()
            .setMargin(itemMargin_)
            .setBorder(itemBorder_)
            .setNewScale(itemScale_);
        // ref->getLayout().margin.left += flattenedTreeBuffer[index]->depth*internals_.marginFactor_;

        append(ref);

        ref->getEvents().listen<events::LMBRelease>(
            [this, index](const auto&)
            {
                internals_.isDirty = true;
                MAKE_LAYOUT_DIRTY_AND_REQUEST_NEW_FRAME;

                events::LMBItemRelease evt{&listItems_[index]};
                getEvents().notifyEvent<events::LMBItemRelease>(evt);
            });
    }
}

RecycleList& RecycleList::setColor(const glm::vec4& color)
{
    color_ = color;
    REQUEST_NEW_FRAME;
    return *this;
}

RecycleList& RecycleList::setBorderColor(const glm::vec4& color)
{
    borderColor_ = color;
    REQUEST_NEW_FRAME;
    return *this;
}

RecycleList& RecycleList::setItemScale(const Layout::ScaleXY newScale)
{
    itemScale_ = newScale;
    internals_.isDirty = true;
    MAKE_LAYOUT_DIRTY_AND_REQUEST_NEW_FRAME;
    return *this;
}

RecycleList& RecycleList::setItemMargin(const utils::Layout::TBLR margin)
{
    itemMargin_ = margin;
    internals_.isDirty = true;
    MAKE_LAYOUT_DIRTY_AND_REQUEST_NEW_FRAME;
    return *this;
}

RecycleList& RecycleList::setItemBorder(const utils::Layout::TBLR border)
{
    itemBorder_ = border;
    internals_.isDirty = true;
    MAKE_LAYOUT_DIRTY_AND_REQUEST_NEW_FRAME;
    return *this;
}

RecycleList& RecycleList::setItemBorderRadius(const utils::Layout::TBLR borderRadius)
{
    itemBorderRadius_ = borderRadius;
    internals_.isDirty = true;
    MAKE_LAYOUT_DIRTY_AND_REQUEST_NEW_FRAME;
    return *this;
}

glm::vec4 RecycleList::getColor() const { return color_; }

glm::vec4 RecycleList::getBorderColor() const { return borderColor_; }

Layout::ScaleXY RecycleList::getItemScale() const { return itemScale_; }

Layout::TBLR RecycleList::getItemMargin() const { return itemMargin_; }

Layout::TBLR RecycleList::getItemBorder() const { return itemBorder_; }

Layout::TBLR RecycleList::getItemBorderRadius() const { return itemBorderRadius_; }

RecycleList::Internals& RecycleList::getInternalsRef() { return internals_; }
} // msgui
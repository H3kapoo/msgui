#include "TreeView.hpp"

#include <algorithm>
#include <ranges>
#include <stack>

#include "msgui/loaders/MeshLoader.hpp"
#include "msgui/loaders/ShaderLoader.hpp"
#include "msgui/node/AbstractNode.hpp"
#include "msgui/node/Button.hpp"
#include "msgui/node/FrameState.hpp"
#include "msgui/node/TextLabel.hpp"
#include "msgui/events/LMBRelease.hpp"
#include "msgui/events/LMBTreeItemRelease.hpp"

namespace msgui
{
TreeView::TreeView(const std::string& name) : Box(name)
{
    /* Defaults */
    log_ = Logger("TreeView(" + name + ")");
    setType(AbstractNode::NodeType::TREEVIEW);
    setShader(loaders::ShaderLoader::loadShader("assets/shader/sdfRect.glsl"));
    setMesh(loaders::MeshLoader::loadQuad());

    color_ = Utils::hexToVec4("#42056bff");
    layout_.setAllowOverflow({true, true})
        .setType(Layout::Type::VERTICAL)
        .setNewScale({100_px, 100_px});
}

void TreeView::addRootItem(const TreeItemPtr& tree)
{
    treeItems_.emplace_back(tree);

    refreshTree();
}

void TreeView::refreshTree()
{
    //TODO: On bigger lists this traversal shall be handled by a separate working thread as to not block the main one.

    /* Traverse the tree depth first so that at the end we get a contiguous array of open tree items
       that we can operate on an display to the user.
       Example & assume each node is opened:

       depth 0:            a        b
       depth 1:         c    d
       depth 2:      e

       Then the flattened buffer will be: a c d e b

       -- a
          -- c
          -- d
             -- e
       -- b
    */
    flattenedTreeBuffer.clear();

    std::stack<TreeItemPtr> traversalStack;
    for (auto& item : treeItems_ | std::views::reverse)
    {
        traversalStack.push(item);
    }

    internals_.maxDepth_ = 0;
    while (!traversalStack.empty())
    {
        TreeItemPtr currentTreeItem = traversalStack.top();
        traversalStack.pop();

        flattenedTreeBuffer.push_back(currentTreeItem);

        /* Reverse needed because otherwise children will be displayed in reverse order due to how
           stack traversal works.*/
        for (auto& item : currentTreeItem->subItems | std::views::reverse)
        {
            if (currentTreeItem->isOpen)
            {
                internals_.maxDepth_ = std::max(internals_.maxDepth_, item->depth);
                traversalStack.push(item);
            }
        }
    }
}

void TreeView::printFlatTreeView()
{
    for (const auto& item : flattenedTreeBuffer)
    {
        log_.raw("%*s- depth(%d) col(%.2f %.2f %.2f %.2f)\n", item->depth*4, "", item->depth,
            item->color.r, item->color.g, item->color.b, item->color.a);
    }
}

void TreeView::setShaderAttributes()
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

void TreeView::onLayoutDirtyPost()
{
    removeAll();

    internals_.elementsCount = flattenedTreeBuffer.size();
    for (int32_t i = 0; i < internals_.visibleNodes; i++)
    {
        int32_t index = internals_.topOfListIdx + i;
        if (index < internals_.elementsCount)
        {
            auto ref = std::make_shared<Button>("Item");
            ref->setColor(flattenedTreeBuffer[index]->color)
                .setText(flattenedTreeBuffer[index]->text);

            ref->getLayout()
                .setMargin(itemMargin_)
                .setBorder(itemBorder_)
                .setNewScale(itemScale_);
            ref->getLayout().margin.left += flattenedTreeBuffer[index]->depth*internals_.marginFactor_;
            append(ref);

            ref->getEvents().listen<events::LMBRelease>(
                [this, index](const auto&)
                {
                    flattenedTreeBuffer[index]->toggle();
                    refreshTree();
                    internals_.isDirty = true;
                    MAKE_LAYOUT_DIRTY_AND_REQUEST_NEW_FRAME;

                    events::LMBTreeItemRelease evt{flattenedTreeBuffer[index]};
                    getEvents().notifyEvent<events::LMBTreeItemRelease>(evt);
                });
        }
    }
}

TreeView& TreeView::setColor(const glm::vec4& color)
{
    color_ = color;
    REQUEST_NEW_FRAME;
    return *this;
}

TreeView& TreeView::setBorderColor(const glm::vec4& color)
{
    borderColor_ = color;
    REQUEST_NEW_FRAME;
    return *this;
}

TreeView& TreeView::setItemScale(const Layout::ScaleXY newScale)
{
    itemScale_ = newScale;
    internals_.isDirty = true;
    MAKE_LAYOUT_DIRTY_AND_REQUEST_NEW_FRAME;
    return *this;
}


TreeView& TreeView::setItemMargin(const utils::Layout::TBLR margin)
{
    itemMargin_ = margin;
    internals_.isDirty = true;
    MAKE_LAYOUT_DIRTY_AND_REQUEST_NEW_FRAME;
    return *this;
}

TreeView& TreeView::setItemBorder(const utils::Layout::TBLR border)
{
    itemBorder_ = border;
    internals_.isDirty = true;
    MAKE_LAYOUT_DIRTY_AND_REQUEST_NEW_FRAME;
    return *this;
}

TreeView& TreeView::setItemBorderRadius(const utils::Layout::TBLR borderRadius)
{
    itemBorderRadius_ = borderRadius;
    internals_.isDirty = true;
    MAKE_LAYOUT_DIRTY_AND_REQUEST_NEW_FRAME;
    return *this;
}

TreeView& TreeView::setMarginFactor(const uint32_t marginFactor)
{
    internals_.marginFactor_ = marginFactor;
    MAKE_LAYOUT_DIRTY_AND_REQUEST_NEW_FRAME;
    return *this;
}

glm::vec4 TreeView::getColor() const { return color_; }

glm::vec4 TreeView::getBorderColor() const { return borderColor_; }

Layout::ScaleXY TreeView::getItemScale() const { return itemScale_; }

utils::Layout::TBLR TreeView::getItemMargin() const { return itemMargin_; }

utils::Layout::TBLR TreeView::getItemBorder() const { return itemBorder_; }

utils::Layout::TBLR TreeView::getItemBorderRadius() const { return itemBorderRadius_; }

uint32_t TreeView::getMarginFactor() const { return internals_.marginFactor_; }

TreeView::Internals& TreeView::getInternalsRef() { return internals_; }
} // msgui
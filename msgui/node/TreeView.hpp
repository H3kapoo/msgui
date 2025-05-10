#pragma once

#include "msgui/node/AbstractNode.hpp"
#include "msgui/node/Box.hpp"
#include "msgui/node/utils/TreeItem.hpp"

namespace msgui
{

class CustomLayoutEngine;

/* Node used for efficiently handling lists with a large amount of entries. */
class TreeView : public Box
{
struct Internals;
public:
    TreeView(const std::string& name);

    /**
        Adds a new tree item to the internal buffer.

        @param tree Tree item to be added
    */
    void addRootItem(const TreeItemPtr& tree);

    /**
        Triggers refresh of the viewable tree in order to have the newest changes visible.
    */
    void refreshTree();

    /**
        Print tree structure of all nodes that are open and their children. Closed nodes will not show up.
    */
    void printFlatTreeView();

    /**
        Removes the item located at a specified index.

        @param idx Index of the item to be removed
    */
    void removeItemIdx(const int32_t idx);

    /**
        Removes all items that meet a certain predicate's criteria.

        @param pred Predicate to be satisfied
    */
    void removeItemsBy(const std::function<bool(const glm::vec4&)> pred);

    TreeView& setColor(const glm::vec4& color);
    TreeView& setBorderColor(const glm::vec4& color);
    TreeView& setItemScale(const Layout::ScaleXY scale);
    TreeView& setItemMargin(const utils::Layout::TBLR margin);
    TreeView& setItemBorder(const utils::Layout::TBLR border);
    TreeView& setItemBorderRadius(const utils::Layout::TBLR borderRadius);
    TreeView& setMarginFactor(const uint32_t marginFactor);
    
    glm::vec4 getColor() const;
    glm::vec4 getBorderColor() const;
    Layout::ScaleXY getItemScale() const;
    utils::Layout::TBLR getItemMargin() const;
    utils::Layout::TBLR getItemBorder() const;
    utils::Layout::TBLR getItemBorderRadius() const;
    uint32_t getMarginFactor() const;
    Internals& getInternalsRef();

private: // friend
    friend CustomLayoutEngine;
    friend WindowFrame;
    void onLayoutDirtyPost();

private:
    ABSTRACT_NODE_ALLOW_APPEND_REMOVE;

private:
    void setShaderAttributes() override;

private:
    glm::vec4 color_{1.0f};
    glm::vec4 borderColor_{1.0f};
    Layout::ScaleXY itemScale_{200_px, 20_px};
    utils::Layout::TBLR itemMargin_{0};
    utils::Layout::TBLR itemBorder_{0};
    utils::Layout::TBLR itemBorderRadius_{0};

    TreeItemPtrVec treeItems_;
    TreeItemPtrVec flattenedTreeBuffer; //TODO: Consider using a list instead

    struct Internals
    {
        bool isDirty{true};
        int32_t topOfListIdx{0};
        int32_t oldTopOfListIdx{-1};
        int32_t oldVisibleNodes{0};
        int32_t visibleNodes{0};
        int32_t maxDepth_{0};
        uint32_t marginFactor_{30};
        float lastScaleY{0};
        float lastScaleX{0};
        int32_t elementsCount{0};
        glm::ivec2 overflow{0, 0};
    };
    Internals internals_;
};

using TreeViewPtr = std::shared_ptr<TreeView>;
} // namespace msgui
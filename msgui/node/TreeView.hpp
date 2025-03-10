#pragma once

#include "msgui/layoutEngine/SimpleLayoutEngine.hpp"
#include "msgui/node/AbstractNode.hpp"
#include "msgui/node/Box.hpp"
#include "msgui/node/Slider.hpp"
#include "msgui/nodeEvent/Scroll.hpp"
#include <memory>

namespace msgui
{
struct TreeItem;
using TreeItemWPtr = std::weak_ptr<TreeItem>;
using TreeItemPtr = std::shared_ptr<TreeItem>;
using TreeItemPtrVec = std::vector<TreeItemPtr>;

struct TreeItem : std::enable_shared_from_this<TreeItem>
{
    void addItem(TreeItemPtr& item)
    {
        item->depth = depth + 1;
        items.emplace_back(item);
        item->parent = shared_from_this();
    }

    void open() { isOpen = true; }

    void close()
    {
        if (!isOpen) { return; }

        isOpen = false;
        for (auto& item : items)
        {
            item->close();
        }
    }

    void toggle() { isOpen ? close() : open(); }

    glm::vec4 color;
    float push;
    int32_t depth{0};
    bool isOpen{false};
    std::weak_ptr<TreeItem> parent;
    std::vector<std::shared_ptr<TreeItem>> items;
};

class SimpleLayoutEngine;

/* Node used for efficiently handling lists with a large amount of entries. */
class TreeView : public AbstractNode
{
public:
    TreeView(const std::string& name);

    /**
        Adds a new item to the list.

        @note @todo For now items are just colors, but when text rendering will be implemented, the items
                    will be able to display both text, colors and images.

        @param color Color of the item
    */
    void addItem(const glm::vec4& color);

    void addItem(const TreeItemPtr& tree);
    void recalc();
    void printTreeView();

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
    TreeView& setRowSize(const int32_t rowSize);
    TreeView& setItemMargin(const Layout::TBLR margin);
    TreeView& setItemBorder(const Layout::TBLR border);
    TreeView& setItemBorderRadius(const Layout::TBLR borderRadius);

    glm::vec4 getColor() const;
    glm::vec4 getBorderColor() const;
    int32_t getRowSize() const;
    Layout::TBLR getItemMargin() const;
    Layout::TBLR getItemBorder() const;
    Layout::TBLR getItemBorderRadius() const;
    BoxWPtr getContainer();

private: // friend
    friend SimpleLayoutEngine;
    friend WindowFrame;
    void onLayoutUpdateNotify();

private:
    void setShaderAttributes() override;

    void onSliderValueChanged(const nodeevent::Scroll& evt);

    void updateNodePositions();
    void setupLayoutReloadables();

private:
    glm::vec4 color_{1.0f};
    glm::vec4 borderColor_{1.0f};
    int32_t rowSize_{20};
    Layout::TBLR itemMargin_{0};
    Layout::TBLR itemBorder_{0};
    Layout::TBLR itemBorderRadius_{0};
    std::vector<TreeItem> listItems_;

    TreeItemPtrVec treeItems_;
    TreeItemPtrVec flatTreeItems_;

    BoxPtr boxCont_{nullptr};

    glm::ivec2 overflow{0, 0};
    bool listIsDirty_{true};
    int32_t oldTopOfList_{-1};
    int32_t oldVisibleNodes_{0};
    float lastScaleY_{0};
    float lastScaleX_{0};
};

using TreeViewPtr = std::shared_ptr<TreeView>;
} // namespace msgui
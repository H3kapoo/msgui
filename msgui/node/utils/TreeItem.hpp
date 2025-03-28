#pragma once

#include <memory>
#include <vector>
#include <glm/glm.hpp>

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
        subItems.emplace_back(item);
        item->parentItem = shared_from_this();
    }

    void toggle() { isOpen ? close() : open(); }

    void open() { isOpen = true; }

    void close()
    {
        if (!isOpen) { return; }

        isOpen = false;
        /* Also recursively close any child nodes. */
        for (auto& item : subItems) { item->close(); }
    }

    /* User set payload */
    glm::vec4 color;
    std::string stringInfo; //TODO: For when text gets implemented

    /* Internal management */
    int32_t depth{0};
    bool isOpen{false};
    TreeItemWPtr parentItem;
    TreeItemPtrVec subItems;
};
} // namespace msgui
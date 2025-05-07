#pragma once

#include "msgui/node/AbstractNode.hpp"
#include "msgui/node/Box.hpp"
#include "msgui/node/utils/ListItem.hpp"

namespace msgui
{
class CustomLayoutEngine;

/* Node used for efficiently handling lists with a large amount of entries. */
class RecycleList : public Box
{
struct Internals;



public:
    RecycleList(const std::string& name);

    /**
        Adds a new item to the list.

        @param color Color of the item
    */
    void addItem(const node::utils::ListItem& item);

    /**
        Removes the item located at a specified index.

        @param idx Index of the item to be removed
    */
    void removeItemIdx(const int32_t idx);

    /**
        Removes all items that meet a certain predicate's criteria.

        @param pred Predicate to be satisfied
    */
    void removeItemsBy(const std::function<bool(const node::utils::ListItem&)> pred);

    RecycleList& setColor(const glm::vec4& color);
    RecycleList& setBorderColor(const glm::vec4& color);
    RecycleList& setRowSize(const int32_t rowSize);
    RecycleList& setItemMargin(const utils::Layout::TBLR margin);
    RecycleList& setItemBorder(const utils::Layout::TBLR border);
    RecycleList& setItemBorderRadius(const utils::Layout::TBLR borderRadius);

    glm::vec4 getColor() const;
    glm::vec4 getBorderColor() const;
    int32_t getRowSize() const;
    utils::Layout::TBLR getItemMargin() const;
    utils::Layout::TBLR getItemBorder() const;
    utils::Layout::TBLR getItemBorderRadius() const;
    Internals& getInternalsRef();

private: // friend
    friend CustomLayoutEngine;
    friend WindowFrame;
    void onLayoutDirtyPost();

private:
    void setShaderAttributes() override;

private:
    glm::vec4 color_{1.0f};
    glm::vec4 borderColor_{1.0f};
    int32_t rowSize_{20};
    utils::Layout::TBLR itemMargin_{0};
    utils::Layout::TBLR itemBorder_{0};
    utils::Layout::TBLR itemBorderRadius_{0};
    std::vector<node::utils::ListItem> listItems_;

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

using RecycleListPtr = std::shared_ptr<RecycleList>;
} // namespace msgui
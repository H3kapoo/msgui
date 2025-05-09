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
    RecycleList& setItemScale(const Layout::ScaleXY scale);
    RecycleList& setItemMargin(const Layout::TBLR margin);
    RecycleList& setItemBorder(const Layout::TBLR border);
    RecycleList& setItemBorderRadius(const Layout::TBLR borderRadius);

    glm::vec4 getColor() const;
    glm::vec4 getBorderColor() const;
    Layout::ScaleXY getItemScale() const;
    Layout::TBLR getItemMargin() const;
    Layout::TBLR getItemBorder() const;
    Layout::TBLR getItemBorderRadius() const;
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
    Layout::ScaleXY itemScale_{200_px, 20_px};
    Layout::TBLR itemMargin_{0};
    Layout::TBLR itemBorder_{0};
    Layout::TBLR itemBorderRadius_{0};
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
#pragma once

#include "msgui/node/AbstractNode.hpp"
#include "msgui/node/Box.hpp"
#include "msgui/node/Slider.hpp"
#include "msgui/nodeEvent/Scroll.hpp"

namespace msgui::recyclelist
{

/* Node used for efficiently handling lists with a large amount of entries. */
class RecycleList : public AbstractNode
{
public:
    RecycleList(const std::string& name);

    /**
        Adds a new item to the list.

        @note @todo For now items are just colors, but when text rendering will be implemented, the items
                    will be able to display both text, colors and images.

        @param color Color of the item
    */
    void addItem(const glm::vec4& color);

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

    RecycleList& setColor(const glm::vec4& color);
    RecycleList& setBorderColor(const glm::vec4& color);
    RecycleList& setRowSize(const int32_t rowSize);
    RecycleList& setItemMargin(const Layout::TBLR margin);
    RecycleList& setItemBorder(const Layout::TBLR border);
    RecycleList& setItemBorderRadius(const Layout::TBLR borderRadius);

    glm::vec4 getColor() const;
    glm::vec4 getBorderColor() const;
    int32_t getRowSize() const;
    Layout::TBLR getItemMargin() const;
    Layout::TBLR getItemBorder() const;
    Layout::TBLR getItemBorderRadius() const;
    SliderWPtr getSlider();
    BoxWPtr getContainer();

private: // friend
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
    std::vector<glm::vec4> listItems_;

    SliderPtr slider_{nullptr};
    BoxPtr boxCont_{nullptr};

    bool listIsDirty_{true};
    int32_t oldTopOfList_{-1};
    int32_t oldVisibleNodes_{0};
    float lastScaleY_{0};
};

using RecycleListPtr = std::shared_ptr<RecycleList>;
} // namespace msgui::recyclelist
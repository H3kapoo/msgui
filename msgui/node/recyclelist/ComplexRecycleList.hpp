#pragma once

#include "msgui/node/AbstractNode.hpp"
#include "msgui/node/Box.hpp"
#include "msgui/node/Slider.hpp"
#include "msgui/nodeEvent/Scroll.hpp"

namespace msgui::recyclelist
{
struct ListItem
{
    glm::vec4 color{0};
    float push{0};
};

/* Node used for efficiently handling lists with a large amount of entries. */
class ComplexRecycleList : public AbstractNode
{
public:
    ComplexRecycleList(const std::string& name);

    void addItem(const glm::vec4& color);
    void removeItem(const int32_t idx);
    void removeTailItems(const int32_t amount);

    ComplexRecycleList& setColor(const glm::vec4& color);
    ComplexRecycleList& setBorderColor(const glm::vec4& color);
    ComplexRecycleList& setRowSize(const int32_t rowSize);

    glm::vec4 getColor() const;
    glm::vec4 getBorderColor() const;
    int32_t getRowSize() const;
    SliderPtr getSlider();

private: // friend
    friend WindowFrame;
    void onLayoutUpdateNotify();

private:
    void setShaderAttributes() override;

    void onSliderValueChanged(nodeevent::Scroll evt);

    void updateNodePositions();
    void setupLayoutReloadables();

private:
    glm::vec4 color_{1.0f};
    glm::vec4 borderColor_{1.0f};
    int32_t rowSize_{20};
    // int32_t rowMargin_{4};
    int32_t rowMargin_{0};
    std::vector<ListItem> listItems_;
    SliderPtr slider_{nullptr};
    BoxPtr boxCont_{nullptr};

    bool listIsDirty_{true};
    int32_t oldTopOfList_{-1};
    int32_t oldVisibleNodes_{0};
    float lastScaleY_{0};
};

using ComplexRecycleListPtr = std::shared_ptr<ComplexRecycleList>;
} // namespace msgui::recyclelist
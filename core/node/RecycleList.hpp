#pragma once

#include "AbstractNode.hpp"
#include "core/Listeners.hpp"
#include "core/node/Box.hpp"
#include "core/node/Slider.hpp"

namespace msgui
{
class RecycleList : public AbstractNode
{
public:
    RecycleList(const std::string& name);

    void addItem(const glm::vec4& color);

    // template<typename T>
    // void addItemCustom(const T item)
    // {
    //     listItems_
    // }


    void removeItem(const int32_t idx);
    void removeTailItems(const int32_t amount);

    RecycleList& setColor(const glm::vec4& color);
    RecycleList& setBorderColor(const glm::vec4& color);
    RecycleList& setRowSize(const int32_t rowSize);

    glm::vec4 getColor() const;
    glm::vec4 getBorderColor() const;
    int32_t getRowSize() const;
    SliderPtr getSlider();
    Listeners& getListeners();

private: // friend
    friend WindowFrame;
    void onLayoutUpdateNotify();

private:
    void setShaderAttributes() override;

    void onSliderValueChanged(float newVal);
    void onMouseButtonNotify() override;

    void updateNodePositions();
    void setupLayoutReloadables();

private:
    glm::vec4 color_{1.0f};
    glm::vec4 borderColor_{1.0f};
    int32_t rowSize_{20};
    int32_t rowMargin_{4};
    std::vector<glm::vec4> listItems_;
    SliderPtr slider_{nullptr};
    BoxPtr boxCont_{nullptr};

    bool listIsDirty_{true};
    int32_t oldTopOfList_{-1};
    int32_t oldVisibleNodes_{0};
    float lastScaleY_{0};
    Listeners listeners_;
};

using RecycleListPtr = std::shared_ptr<RecycleList>;
} // namespace msgui
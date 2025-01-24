#pragma once

#include "AbstractNode.hpp"
#include "core/Listeners.hpp"
#include "core/node/Box.hpp"
#include "core/node/Slider.hpp"
#include "core/node/utils/LayoutData.hpp"

namespace msgui
{
class RecycleList : public AbstractNode
{
public:
    struct Props
    {
        Layout layout; // Do not change position
        glm::vec4 color{1.0f};
        glm::vec4 borderColor{1.0f};
        int32_t rowSize{20};
    };

public:
    RecycleList(const std::string& name);

    void addItem(const glm::vec4& color);
    void removeItem(const int32_t idx);
    void removeTailItems(const int32_t amount);

    void setShaderAttributes() override;
    void* getProps() override;

private: // friend
    friend WindowFrame;
    void onLayoutUpdateNotify();

private:
    void onSliderValueChanged(float newVal);
    void onMouseButtonNotify() override;

    void updateNodePositions();
    void setupReloadables();

public:
    Listeners listeners;
    Props props;

private:
    std::vector<glm::vec4> listItems_;
    SliderPtr slider_{nullptr};
    BoxPtr boxCont_{nullptr};

    bool listIsDirty_{true};
    int32_t oldTopOfList_{-1};
    int32_t oldVisibleNodes_{0};
    float lastScaleY_{0};
};
using RecycleListPtr = std::shared_ptr<RecycleList>;
} // namespace msgui
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

    // void appendBoxContainer(const BoxPtr& box);
    // void append(const BoxPtr& box);
    void createSlots(uint32_t slotCount, std::vector<float> initialPercSize);

    void setShaderAttributes() override;
    void* getProps() override;

    void onLayoutUpdateNotify();

private:
    void onSliderValueChanged(float newVal);

    void onMouseButtonNotify() override;
    void onWindowResizeNotify() override;

    void setupReloadables();

public:
    Listeners listeners;
    Props props;

private:
    float diff_{0};
    std::vector<glm::vec4> itemColors_;
    SliderPtr slider_{nullptr};
    BoxPtr boxCont_{nullptr};
};
using RecycleListPtr = std::shared_ptr<RecycleList>;
} // namespace msgui
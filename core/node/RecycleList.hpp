#pragma once

#include "AbstractNode.hpp"
#include "core/Listeners.hpp"
#include "core/node/Box.hpp"
#include "core/node/Slider.hpp"

namespace msgui
{
class RecycleList : public AbstractNode
{
struct Props;
public:
    RecycleList(const std::string& name);

    void addItem(const glm::vec4& color);
    void removeItem(const int32_t idx);
    void removeTailItems(const int32_t amount);

    Props& setColor(const glm::vec4& color);
    Props& setBorderColor(const glm::vec4& color);
    Props& setRowSize(const int32_t rowSize);

    glm::vec4 getColor() const;
    glm::vec4 getBorderColor() const;
    int32_t getRowSize() const;

private: // friend
    friend WindowFrame;
    void onLayoutUpdateNotify();

private:
    void setShaderAttributes() override;

    void onSliderValueChanged(float newVal);
    void onMouseButtonNotify() override;

    void updateNodePositions();
    void setupLayoutReloadables();

public:
    Listeners listeners;

private:
    struct Props
    {
        glm::vec4 color{1.0f};
        glm::vec4 borderColor{1.0f};
        int32_t rowSize{20};
    };
    Props props;
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
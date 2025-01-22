#include "RecycleList.hpp"

#include "core/MeshLoader.hpp"
#include "core/ShaderLoader.hpp"
#include "core/node/AbstractNode.hpp"
#include "core/node/Button.hpp"
#include "core/node/FrameState.hpp"
#include "core/node/Slider.hpp"
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <string>

namespace msgui
{
RecycleList::RecycleList(const std::string& name)
    : AbstractNode(MeshLoader::loadQuad(), ShaderLoader::load("assets/shader/sdfRect.glsl"),
        name, NodeType::RECYCLE_LIST)
{
    log_ = ("RecycleList(" + name + ")");

    props.layout.setAllowOverflow({true, true})
        // .setPadding({10, 10, 10, 10})
        .setType(Layout::Type::HORIZONTAL);

    //TODO: Box divider should not be "active" with < 2 boxes
    setupReloadables();


    slider_ = std::make_shared<Slider>("RLSlider");
    slider_->props.layout
        .setType(Layout::Type::VERTICAL)
        .setScaleType({Layout::ScaleType::ABS, Layout::ScaleType::REL})
        .setScale({30, 1.0f});
    slider_->props.color = Utils::hexToVec4("#ddaaffff");
    slider_->props.slideFrom = 0;
    slider_->listeners.setOnSlideValueChanged(
        std::bind(&RecycleList::onSliderValueChanged, this, std::placeholders::_1));

    boxCont_ = std::make_shared<Box>("RLBox");
    boxCont_->props.layout
        .setAllowOverflow({false, false}) // In this context, it shall never have overflow enabled
        .setType(Layout::Type::VERTICAL)
        .setScaleType({Layout::ScaleType::REL, Layout::ScaleType::REL})
        .setScale({1.0f, 1.0f});
    boxCont_->props.color = Utils::hexToVec4("#42056bff");
    boxCont_->listeners.setOnMouseButtonLeftClick(std::bind(&RecycleList::onMouseButtonNotify, this));
    append(slider_);
    append(boxCont_);

    int32_t elNo = 50;
    float step = 1.0f / elNo;
    step *= 2;
    for (int32_t i = 0; i < elNo; i++)
    {
        if (i + 1 == elNo)
        {
            listItems_.push_back(glm::vec4(0, 0, 1, 1));
            continue;
        }
        // log_.debugLn("%f", i*step);
        listItems_.push_back(glm::vec4(i*step, 0, 0, 1));
    }

    for (uint32_t i = 0; i < listItems_.size(); i++)
    {
        auto ref = std::make_shared<Button>("ListBtn" + std::to_string(i));
        ref->props.layout.setMargin({0, 0, 5, 5})
            .setScaleType({Layout::ScaleType::REL, Layout::ScaleType::ABS})
            .setScale({1.0f, props.rowSize});
        ref->props.color = listItems_[i];
        boxCont_->append(ref);
    }

    // slider_->props.slideTo = 74;
}

void RecycleList::setShaderAttributes()
{
    transform_.computeModelMatrix();
    shader_->setMat4f("uModelMat", transform_.modelMatrix);
    shader_->setVec4f("uColor", props.color);
    shader_->setVec4f("uBorderColor", props.borderColor);
    shader_->setVec4f("uBorderSize", props.layout.border);
    shader_->setVec4f("uBorderRadii", props.layout.borderRadius);
    shader_->setVec2f("uResolution", glm::vec2{transform_.scale.x, transform_.scale.y});
}

void* RecycleList::getProps() { return (void*)&props; }

void RecycleList::onLayoutUpdateNotify()
{
    // Setting the overflow if none
    if (lastScaleY != transform_.scale.y)
    {
        int32_t totalElements = listItems_.size();
        slider_->props.slideTo = std::max(totalElements * props.rowSize - transform_.scale.y, 0.0f);
    }

    float sliderVal = slider_->props.slideValue;
    uint64_t maxDisplayAmt = transform_.scale.y / props.rowSize + 1;
    int32_t topOfListIdx = sliderVal / props.rowSize;
    int32_t botOfListIdx = topOfListIdx + maxDisplayAmt;
    int32_t slideIndexDiff = topOfListIdx - oldTopOfList_;
    int32_t visibleNodes = botOfListIdx - topOfListIdx + 1;

    log_.debugLn("top: %d bot %d vis %d", topOfListIdx, botOfListIdx, visibleNodes);

    if (topOfListIdx != oldTopOfList_ || oldVisibleNodes_ != visibleNodes)
    {
        boxCont_->removeAll();
        for (int32_t i = 0; i < visibleNodes; i++)
        {
            if (topOfListIdx + i < (int32_t)listItems_.size())
            {
                auto ref = std::make_shared<Button>("ListBtn2222");
                ref->props.layout.setMargin({0, 0, 5, 5})
                    .setScaleType({Layout::ScaleType::REL, Layout::ScaleType::ABS})
                    .setScale({1.0f, props.rowSize});
                ref->props.color = listItems_[topOfListIdx + i];
                boxCont_->append(ref);
            }
        }
        MAKE_LAYOUT_DIRTY
        REQUEST_STORE_RECREATE
    }

    auto& children = boxCont_->getChildren();
    uint32_t size = children.size();
    for (uint32_t i = 0; i < size; i++)
    {
        if (sliderVal == 0) { break; }
        children[i]->getTransform().pos.y -= (int32_t)sliderVal % props.rowSize + 1;
    }

    oldTopOfList_ = topOfListIdx;
    oldVisibleNodes_ = visibleNodes;
    lastScaleY = transform_.scale.y;
    slideDiff_ = 0;
}

void RecycleList::onSliderValueChanged(float newSliderVal)
{
    auto& children = boxCont_->getChildren();
    uint32_t size = children.size();
    for (uint32_t i = 0; i < size; i++)
    {
        if (newSliderVal == 0) { break; }
        children[i]->getTransform().pos.y -= (int32_t)newSliderVal % props.rowSize + 1;
    }
}

void RecycleList::onMouseButtonNotify()
{
    // slider_->props.slideFrom = 0;
    // slider_->props.slideTo = transform_.scale.y;
    // if (!state_->mouseButtonState[GLFW_MOUSE_BUTTON_LEFT])
    // {
    //     // to be removed from here later on

    //     log_.debugLn("needed %d", buttonsNeeded);
    // }
    // User custom behavior
    // listeners.callOnMouseButton(
    //     state_->lastMouseButtonTriggeredIdx,
    //     state_->mouseButtonState[state_->lastMouseButtonTriggeredIdx],
    //     state_->mouseX,
    //     state_->mouseY);
}

void RecycleList::setupReloadables()
{
    props.layout._onTypeChange = [this]()
    {
        MAKE_LAYOUT_DIRTY
    };

    auto updateCb = [this ](){ MAKE_LAYOUT_DIRTY_AND_REQUEST_NEW_FRAME };

    props.layout._onAlignSelfChange = updateCb;
    props.layout._onMarginChange = updateCb;
    props.layout._onBorderChange = updateCb;
    props.layout._onScaleTypeChange = updateCb;
    props.layout._onScaleChange = updateCb;
}
} // msgui
#include "RecycleList.hpp"

#include "core/MeshLoader.hpp"
#include "core/ShaderLoader.hpp"
#include "core/node/AbstractNode.hpp"
#include "core/node/Button.hpp"
#include "core/node/FrameState.hpp"
#include "core/node/Slider.hpp"
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
        .setScale({50, 1.0f});
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

    int32_t elNo = 100;
    float step = 1.0f / elNo;
    step *= 2;
    for (int32_t i = 0; i < elNo; i++)
    {
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
    int32_t totalElements = listItems_.size();
    float availableSpace = transform_.scale.y;

    // Setting the overflow if none
    if (slider_->props.slideTo == 0)
    {
        //TODO: Ensure OF >0 before setting
        slider_->props.slideTo = totalElements * props.rowSize - availableSpace;
    }

    float sliderVal = slider_->props.slideValue;
    uint64_t maxDisplayAmt = availableSpace / props.rowSize;
    int32_t topOfListIdx = sliderVal / props.rowSize;
    int32_t botOfListIdx = topOfListIdx + std::min(maxDisplayAmt, listItems_.size() - 1);
    int32_t diff = topOfListIdx - oldTopOfList_;

    log_.debugLn("TOP: %d BOL: %d DIFF: %d SV: %f T %d",
        topOfListIdx, botOfListIdx, diff, sliderVal, boxCont_->getChildren().size());

    // If diff is positive it means the list is going "down" so the node elements will go "up".
    // We need to remove "|diff|" elements from the top and put them at the bottom.
    // Basically remove top "|diff|" times & append back "|diff|" times.
    if (diff > 0)
    {
        int32_t absDiff = std::abs(diff);
        for (int32_t i = 0; i < absDiff; i++)
        {
            boxCont_->removeAt(0);
            boxCont_->props.additionalOffset -= props.rowSize;
        }

        for (int32_t i = 0; i < absDiff; i++)
        {
            auto ref = std::make_shared<Button>("ListBtn2222");
            ref->props.layout.setMargin({0, 0, 5, 5})
                .setScaleType({Layout::ScaleType::REL, Layout::ScaleType::ABS})
                .setScale({1.0f, props.rowSize});
            // ref->props.color = Utils::randomRGB();
            // ref->props.color = listItems_[botOfListIdx - i];
            ref->props.color = listItems_[topOfListIdx + i];

            boxCont_->append(ref);
        }
    }
    // If "diff" is negative it means the list is going "up" so the node elements will go "down".
    // We need to remove "|diff|" elements from the bottom and put them at the top.
    // Basically remove bot "|diff|" times & append back to the top "|diff|" times.
    else if (diff < 0)
    {
        int32_t absDiff = std::abs(diff);
        for (int32_t i = 0; i < absDiff; i++)
        {
            boxCont_->removeAt(boxCont_->getChildren().size() - 1);
            boxCont_->props.additionalOffset += props.rowSize;
        }

        for (int32_t i = 0; i < absDiff; i++)
        {
            auto ref = std::make_shared<Button>("ListBtn2222");
            ref->props.layout.setMargin({0, 0, 5, 5})
                .setScaleType({Layout::ScaleType::REL, Layout::ScaleType::ABS})
                .setScale({1.0f, props.rowSize});
            // ref->props.color = Utils::randomRGB();
            ref->props.color = listItems_[topOfListIdx - i];

            boxCont_->appendAt(ref, 0);
        }
    }

    oldTopOfList_ = topOfListIdx;
}

void RecycleList::dummy()
{
    float currentOverflow = listItems_.size() * props.rowSize - transform_.scale.y;

    // // Remove scrollbar if there's no overflow
    // if (getChildren().size() == 2 && currentOverflow <= 0)
    // {
    //     remove(slider_->getId());
    //     boxCont_->props.additionalOffset = 0;
    //     slider_->props.slideValue = 0;
    // }
    // // Put it back after overflow has been deduced to be >0
    // else if (getChildren().size() == 1 && currentOverflow > 0)
    // {
    //     appendAt(slider_, 0);
    // }

    if (slider_->props.slideTo == 0) // or when elements are added or removed
    {
        if (currentOverflow > 0)
        {
            slider_->props.slideTo = currentOverflow;
        }
    }

    float sv = slider_->props.slideValue;
    int32_t topIdx = (int32_t)sv / props.rowSize;
    int32_t botIdx = topIdx + transform_.scale.y / props.rowSize;
    log_.debugLn("slilder val: %f | top of the list: %d | bottom: %d",
    sv, topIdx, botIdx);

    // Only do this when we are not actively scrolling through the elements
    if (diff_ == 4323.0f)
    // if (diff_ == 0)
    {   
        // float childrenLen = boxCont_->getChildren().size() * props.rowSize;
        AbstractNodePtr lastEl = boxCont_->getChildren()[boxCont_->getChildren().size() - 1];
        float childEnd = lastEl->getTransform().pos.y + props.rowSize;
        float parentEnd = transform_.pos.y + transform_.scale.y;
        if (childEnd < parentEnd + props.rowSize)
        {
            int32_t nodesToAdd = (parentEnd - childEnd) / props.rowSize + 1;
            // log_.debugLn("removed %f %d", transform_.scale.y - childrenLen, nodesToAdd);
            for (int32_t i = 0; i < nodesToAdd; i++)
            {
                auto ref = std::make_shared<Button>("ListBtn2222");
                ref->props.layout.setMargin({0, 0, 5, 5})
                    .setScaleType({Layout::ScaleType::REL, Layout::ScaleType::ABS})
                    .setScale({1.0f, props.rowSize});
                ref->props.color = Utils::randomRGB();
                boxCont_->append(ref);
            }
        }

        // log_.debugLn("here %f %f", childEnd, parentEnd - props.rowSize);
        if (childEnd > parentEnd - props.rowSize)
        {
            log_.debugLn("overflow");

            int32_t nodesToRemove = (childEnd - parentEnd) / props.rowSize - 1;
            // log_.debugLn("here %d", nodesToRemove);
            for (int32_t i = 0; i < nodesToRemove; i++)
            {
                boxCont_->removeAt(boxCont_->getChildren().size());
            }
        }
    }

    // Don't do append/remove calcs if there's no diff in scroll
    if (diff_ == 0) { return; }

    // std::vector<glm::vec4> removedColors; // shall be "removed items" or to at least know at what index it was
    // int32_t removed = boxCont_->removeBy([this, &removedColors](AbstractNodePtr btn)
    // {
    //     Button* ref = static_cast<Button*>(btn.get());
    //     // Scroll knob goes downwards
    //     if (diff_ > 0)
    //     {
    //         if (ref->getTransform().pos.y < boxCont_->getTransform().pos.y - props.rowSize)
    //         {
    //             boxCont_->props.additionalOffset -= props.rowSize;
    //             // removedColor = ref->props.color;
    //             removedColors.push_back(ref->props.color);
    //             return true;
    //         }
    //     }
    //     // Scroll knob goes upwards
    //     else if (diff_ < 0)
    //     {
    //         if (ref->getTransform().pos.y + ref->getTransform().scale.y
    //             > boxCont_->getTransform().pos.y + boxCont_->getTransform().scale.y + props.rowSize)
    //         {
    //             boxCont_->props.additionalOffset += props.rowSize;
    //             removedColors.push_back(ref->props.color);
    //             return true;
    //         }
    //     }
    //     return false;
    // });

    // TODO: Check if we really need to add more or leave the current amount of nodes + remove slider
    // for (int32_t i = 0; i < removed; i++)
    // {
    //     // TODO: Nodes on scroll should be recycled, not recreated
    //     auto ref = std::make_shared<Button>("ListBtn2222");
    //     ref->props.layout.setMargin({0, 0, 5, 5})
    //         .setScaleType({Layout::ScaleType::REL, Layout::ScaleType::ABS})
    //         .setScale({1.0f, props.rowSize});
    //     // ref->props.color = Utils::randomRGB();

    //     // Scroll knob goes downwards
    //     if (diff_ > 0)
    //     {
    //         ref->props.color = removedColors[i];
    //         boxCont_->append(ref);
    //     }
    //     // Scroll knob goes upwards
    //     else if (diff_ < 0)
    //     {
    //         ref->props.color = removedColors[removed - 1 - i];
    //         boxCont_->appendAt(ref, 0);
    //     }
    // }

    diff_ = 0;
}

void RecycleList::onSliderValueChanged(float newVal)
{
    static float prevVal = 0;
    diff_ = newVal - prevVal;
    // boxCont_->props.additionalOffset = newVal;
    boxCont_->props.additionalOffset += diff_;
    // diff_ = 0;
    prevVal = newVal;

    // of 100 | rs 20
    // sv 0 -> top of the list is index 0
    // sv 20 -> top of the list is index 1
    // sv 40 -> top of the list is index 2
    // sv / rs ?
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
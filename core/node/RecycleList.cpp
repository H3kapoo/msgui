#include "RecycleList.hpp"

#include "core/MeshLoader.hpp"
#include "core/ShaderLoader.hpp"
#include "core/node/AbstractNode.hpp"
#include "core/node/Button.hpp"
#include "core/node/FrameState.hpp"
#include "core/node/Slider.hpp"
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
    slider_->props.slideTo = 100;
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
    /*
        pSize 160
        rowSize 20
        I'll need => 160/20 = 8

        RL
            - BOX L 1.0f
            - SB R 20.0px
    */
    itemColors_ = {
        Utils::hexToVec4("#aa0000ff"), Utils::hexToVec4("#00dd00ff"), Utils::hexToVec4("#0000eeff"),
        Utils::hexToVec4("#aa0000ff"), Utils::hexToVec4("#00dd00ff"), Utils::hexToVec4("#0000eeff"),
        Utils::hexToVec4("#aa0000ff"), Utils::hexToVec4("#00dd00ff"), Utils::hexToVec4("#0000eeff"),
        Utils::hexToVec4("#aa0000ff"), Utils::hexToVec4("#00dd00ff"), Utils::hexToVec4("#0000eeff"),
        Utils::hexToVec4("#aa0000ff"), Utils::hexToVec4("#00dd00ff"), Utils::hexToVec4("#0000eeff"),
        Utils::hexToVec4("#aa0000ff"), Utils::hexToVec4("#00dd00ff"), Utils::hexToVec4("#0000eeff"),
        Utils::hexToVec4("#aa0000ff"), Utils::hexToVec4("#00dd00ff"), Utils::hexToVec4("#0000eeff"),
        Utils::hexToVec4("#aa0000ff"), Utils::hexToVec4("#00dd00ff"), Utils::hexToVec4("#0000eeff"),
        Utils::hexToVec4("#aa0000ff"), Utils::hexToVec4("#00dd00ff"), Utils::hexToVec4("#0000eeff"),
        Utils::hexToVec4("#aa0000ff"), Utils::hexToVec4("#00dd00ff"), Utils::hexToVec4("#0000eeff"),
    };

    slider_->props.slideFrom = 0;
    // slider_->props.slideTo = transform_.scale.y;
    slider_->props.slideTo = 1900;
    // log_.debugLn("got it");

    for (uint32_t i = 0; i < itemColors_.size(); i++)
    {
        auto ref = std::make_shared<Button>("ListBtn" + std::to_string(i));
        ref->props.layout.setMargin({0, 0, 5, 5})
            .setScaleType({Layout::ScaleType::REL, Layout::ScaleType::ABS})
            .setScale({1.0f, props.rowSize});
        ref->props.color = itemColors_[i];
        boxCont_->append(ref);
    }
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
    if (diff_ > 0)
    {
        int32_t removed = boxCont_->removeBy([this](AbstractNodePtr btn)
        {
            Button* ref = static_cast<Button*>(btn.get());
            if (ref->getTransform().pos.y < boxCont_->getTransform().pos.y - props.rowSize)
            {
                boxCont_->props.additionalOffset -= props.rowSize;
                return true;
            }
            return false;
        });

        for (int32_t i = 0; i < removed; i++)
        {
            auto ref = std::make_shared<Button>("ListBtn2222");
            ref->props.layout.setMargin({0, 0, 5, 5})
                .setScaleType({Layout::ScaleType::REL, Layout::ScaleType::ABS})
                .setScale({1.0f, props.rowSize});
            ref->props.color = Utils::randomRGB();
            boxCont_->append(ref);
        }
    }
    else if (diff_ < 0)
    {
        int32_t removed = boxCont_->removeBy([this](AbstractNodePtr btn)
        {
            Button* ref = static_cast<Button*>(btn.get());
            if (ref->getTransform().pos.y + ref->getTransform().scale.y
                > boxCont_->getTransform().pos.y + boxCont_->getTransform().scale.y + props.rowSize)
            {
                boxCont_->props.additionalOffset += props.rowSize;
                return true;
            }
            return false;
        });

        for (int32_t i = 0; i < removed; i++)
        {
            auto ref = std::make_shared<Button>("ListBtn2222");
            ref->props.layout.setMargin({0, 0, 5, 5})
                .setScaleType({Layout::ScaleType::REL, Layout::ScaleType::ABS})
                .setScale({1.0f, props.rowSize});
            ref->props.color = Utils::randomRGB();
            boxCont_->appendAt(ref, 0);
        }
    }
}

void RecycleList::onSliderValueChanged(float newVal)
{
    static float prevVal = 0;
    diff_ = newVal - prevVal;
    boxCont_->props.additionalOffset += diff_;
    log_.debugLn("removed %f", boxCont_->props.additionalOffset);
    prevVal = newVal;
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

void RecycleList::onWindowResizeNotify()
{
    // Currently doesnt play well with going fullscreen
    // log_.debugLn("needed %d %d", state_->frameSize.x, state_->frameSize.y);
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
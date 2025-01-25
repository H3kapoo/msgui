#include "RecycleList.hpp"

#include "core/MeshLoader.hpp"
#include "core/ShaderLoader.hpp"
#include "core/node/AbstractNode.hpp"
#include "core/node/Button.hpp"
#include "core/node/FrameState.hpp"
#include "core/node/Slider.hpp"

namespace msgui
{
RecycleList::RecycleList(const std::string& name)
    : AbstractNode(MeshLoader::loadQuad(), ShaderLoader::load("assets/shader/sdfRect.glsl"),
        name, NodeType::RECYCLE_LIST)
{
    log_ = ("RecycleList(" + name + ")");

    layout_.setAllowOverflow({true, true})
        // .setPadding({10, 10, 10, 10})
        .setType(Layout::Type::HORIZONTAL);

    //TODO: Box divider should not be "active" with < 2 boxes
    setupLayoutReloadables();


    slider_ = std::make_shared<Slider>("RLSlider");
    slider_->getLayout()
        .setType(Layout::Type::VERTICAL)
        .setScaleType({Layout::ScaleType::ABS, Layout::ScaleType::REL})
        .setScale({50, 1.0f});
    slider_->setColor(Utils::hexToVec4("#ddaaffff"));
    slider_->setSlideFrom(0);
    slider_->listeners.setOnSlideValueChanged(
        std::bind(&RecycleList::onSliderValueChanged, this, std::placeholders::_1));

    boxCont_ = std::make_shared<Box>("RLBox");
    boxCont_->getLayout()
        .setAllowOverflow({false, false}) // In this context, it shall never have overflow enabled
        .setType(Layout::Type::VERTICAL)
        .setScaleType({Layout::ScaleType::REL, Layout::ScaleType::REL})
        .setScale({1.0f, 1.0f});
    boxCont_->setColor(Utils::hexToVec4("#42056bff"));
    boxCont_->listeners.setOnMouseButtonLeftClick(std::bind(&RecycleList::onMouseButtonNotify, this));
    // append(slider_);
    append(boxCont_);

    // int32_t elNo = 1'000'000;
    // int32_t elNo = 3000;
    // listItems_.reserve(elNo);
    // for (int32_t i = 0; i < elNo; i++)
    // {
    //     if (i + 1 == elNo)
    //     {
    //         listItems_.push_back(glm::vec4(0, 0, 1, 1));
    //         continue;
    //     }
    //     // listItems_.push_back(glm::vec4(0, 1, 1, 1));
    //     listItems_.emplace_back(Utils::randomRGB());
    // }
}

void RecycleList::addItem(const glm::vec4& color)
{
    listItems_.emplace_back(color);

    listIsDirty_ = true;
    MAKE_LAYOUT_DIRTY
}

void RecycleList::removeItem(const int32_t idx)
{   
    if (idx < 0 || idx > (int32_t)listItems_.size() - 1) { return; }
    listItems_.erase(listItems_.begin() + idx);

    listIsDirty_ = true;
    MAKE_LAYOUT_DIRTY
}

void RecycleList::removeTailItems(const int32_t amount)
{
    for (int32_t i = 0; i < amount; i++)
    {
        removeItem(listItems_.size() - i - 1);
    }
}

void RecycleList::setShaderAttributes()
{
    transform_.computeModelMatrix();
    shader_->setMat4f("uModelMat", transform_.modelMatrix);
    shader_->setVec4f("uColor", props.color);
    shader_->setVec4f("uBorderColor", props.borderColor);
    shader_->setVec4f("uBorderSize", layout_.border);
    shader_->setVec4f("uBorderRadii", layout_.borderRadius);
    shader_->setVec2f("uResolution", glm::vec2{transform_.scale.x, transform_.scale.y});
}

void RecycleList::onLayoutUpdateNotify()
{
    // Setting the overflow if none
    if (listIsDirty_ || lastScaleY_ != transform_.scale.y)
    {
        int32_t totalElements = listItems_.size();
        slider_->setSlideTo(std::max(totalElements * props.rowSize - transform_.scale.y, 0.0f));

        if (slider_->getSlideTo() == 0 && children_.size() == 2)
        {
            remove(slider_->getId());
        }
        else if (slider_->getSlideTo() > 0 && children_.size() == 1)
        {
            appendAt(slider_, 0);
        }
    }

    int32_t maxDisplayAmt = transform_.scale.y / props.rowSize + 1;
    int32_t topOfListIdx = slider_->getSlideCurrentValue() / props.rowSize;
    int32_t botOfListIdx = topOfListIdx + maxDisplayAmt;
    int32_t visibleNodes = botOfListIdx - topOfListIdx + 1;

    // log_.debugLn("list dirty: %d", listIsDirty_);
    // log_.debugLn("top: %d bot %d vis %d", topOfListIdx, botOfListIdx, visibleNodes);
    if (listIsDirty_ || topOfListIdx != oldTopOfList_ || oldVisibleNodes_ != visibleNodes)
    {
        listIsDirty_ = false;
        boxCont_->removeAll();
        for (int32_t i = 0; i < visibleNodes; i++)
        {
            if (topOfListIdx + i < (int32_t)listItems_.size())
            {
                auto ref = std::make_shared<Button>("ListBtn2222");
                ref->getLayout().setMargin({4, 0, 5, 5})
                    .setScaleType({Layout::ScaleType::REL, Layout::ScaleType::ABS})
                    .setScale({1.0f, props.rowSize});
                ref->setColor(listItems_[topOfListIdx + i]);
                boxCont_->append(ref);
            }
        }
    }

    updateNodePositions();

    oldTopOfList_ = topOfListIdx;
    oldVisibleNodes_ = visibleNodes;
    lastScaleY_ = transform_.scale.y;
}

void RecycleList::onSliderValueChanged(float newSliderVal)
{
    (void)newSliderVal;
    updateNodePositions();
}

void RecycleList::onMouseButtonNotify() {}

void RecycleList::updateNodePositions()
{
    if (slider_->getSlideCurrentValue() == 0) { return; }

    auto& children = boxCont_->getChildren();
    uint32_t size = children.size();
    for (uint32_t i = 0; i < size; i++)
    {
        children[i]->getTransform().pos.y -= (int32_t)slider_->getSlideCurrentValue() % props.rowSize;
    }
}

void RecycleList::setupLayoutReloadables()
{
    layout_.onTypeChange = [this]()
    {
        MAKE_LAYOUT_DIRTY
    };

    auto updateCb = [this ](){ MAKE_LAYOUT_DIRTY_AND_REQUEST_NEW_FRAME };

    layout_.onAlignSelfChange = updateCb;
    layout_.onMarginChange = updateCb;
    layout_.onBorderChange = updateCb;
    layout_.onScaleTypeChange = updateCb;
    layout_.onScaleChange = updateCb;
}

RecycleList::Props& RecycleList::setColor(const glm::vec4& color)
{
    props.color = color;
    return props;
}

RecycleList::Props& RecycleList::setBorderColor(const glm::vec4& color)
{
    props.borderColor = color;
    return props;
}

RecycleList::Props& RecycleList::setRowSize(const int32_t rowSize)
{
    props.rowSize = rowSize;
    return props;
}

glm::vec4 RecycleList::getColor() const { return props.color; }

glm::vec4 RecycleList::getBorderColor() const { return props.borderColor; }

int32_t RecycleList::getRowSize() const { return props.rowSize; }
} // msgui
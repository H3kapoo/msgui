#include "RecycleList.hpp"

#include "core/MeshLoader.hpp"
#include "core/ShaderLoader.hpp"
#include "core/node/AbstractNode.hpp"
#include "core/node/Button.hpp"
#include "core/node/FrameState.hpp"
#include "core/node/Slider.hpp"

namespace msgui
{
RecycleList::RecycleList(const std::string& name) : AbstractNode(name, NodeType::RECYCLE_LIST)
{
    setShader(ShaderLoader::load("assets/shader/sdfRect.glsl"));
    setMesh(MeshLoader::loadQuad());
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
    slider_->getListeners().setOnSlideValueChanged(
        std::bind(&RecycleList::onSliderValueChanged, this, std::placeholders::_1));

    boxCont_ = std::make_shared<Box>("RLBox");
    boxCont_->getLayout()
        .setAllowOverflow({false, false}) // In this context, it shall never have overflow enabled
        .setType(Layout::Type::VERTICAL)
        .setScaleType({Layout::ScaleType::REL, Layout::ScaleType::REL})
        .setScale({1.0f, 1.0f});
    boxCont_->setColor(Utils::hexToVec4("#42056bff"));
    boxCont_->getListeners().setOnMouseButtonLeftClick(std::bind(&RecycleList::onMouseButtonNotify, this));
    // append(slider_);
    append(boxCont_);
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
    auto shader = getShader();
    shader->setMat4f("uModelMat", transform_.modelMatrix);
    shader->setVec4f("uColor", color_);
    shader->setVec4f("uBorderColor", borderColor_);
    shader->setVec4f("uBorderSize", layout_.border);
    shader->setVec4f("uBorderRadii", layout_.borderRadius);
    shader->setVec2f("uResolution", glm::vec2{transform_.scale.x, transform_.scale.y});
}

void RecycleList::onLayoutUpdateNotify()
{
    // Setting the overflow if none
    if (listIsDirty_ || lastScaleY_ != transform_.scale.y)
    {
        int32_t totalElements = listItems_.size();
        slider_->setSlideTo(std::max(totalElements * rowSize_ - transform_.scale.y, 0.0f));

        if (slider_->getSlideTo() == 0 && children_.size() == 2)
        {
            remove(slider_->getId());
        }
        else if (slider_->getSlideTo() > 0 && children_.size() == 1)
        {
            appendAt(slider_, 0);
        }
    }

    int32_t maxDisplayAmt = transform_.scale.y / rowSize_ + 1;
    int32_t topOfListIdx = slider_->getSlideCurrentValue() / rowSize_;
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
                    .setScale({1.0f, rowSize_});
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
        children[i]->getTransform().pos.y -= (int32_t)slider_->getSlideCurrentValue() % rowSize_;
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

RecycleList& RecycleList::setColor(const glm::vec4& color)
{
    color_ = color;
    return *this;
}

RecycleList& RecycleList::setBorderColor(const glm::vec4& color)
{
    borderColor_ = color;
    return *this;
}

RecycleList& RecycleList::setRowSize(const int32_t rowSize)
{
    rowSize_ = rowSize;
    return *this;
}

glm::vec4 RecycleList::getColor() const { return color_; }

glm::vec4 RecycleList::getBorderColor() const { return borderColor_; }

int32_t RecycleList::getRowSize() const { return rowSize_; }

Listeners& RecycleList::getListeners() { return listeners_; }
} // msgui
#include "RecycleList.hpp"

#include "msgui/MeshLoader.hpp"
#include "msgui/ShaderLoader.hpp"
#include "msgui/node/AbstractNode.hpp"
#include "msgui/node/Button.hpp"
#include "msgui/node/FrameState.hpp"
#include "msgui/node/Slider.hpp"

namespace msgui
{
RecycleList::RecycleList(const std::string& name) : AbstractNode(name, NodeType::RECYCLE_LIST)
{
    setShader(ShaderLoader::loadShader("assets/shader/sdfRect.glsl"));
    setMesh(MeshLoader::loadQuad());
    log_ = ("RecycleList(" + name + ")");

    setupLayoutReloadables();

    slider_ = std::make_shared<Slider>("RLSlider");
    slider_->getLayout()
        .setType(Layout::Type::VERTICAL)
        .setScaleType({Layout::ScaleType::ABS, Layout::ScaleType::REL})
        .setScale({20 /* Default but you should use setGirth instead */, 1.0f});
    slider_->setGirth(20);
    slider_->setColor(Utils::hexToVec4("#ddaaffff"));
    slider_->setSlideFrom(0);
    // slider_->getListeners().setOnSlideValueChanged(
    //     std::bind(&RecycleList::onSliderValueChanged, this, std::placeholders::_1));

    boxCont_ = std::make_shared<Box>("RLBox");
    boxCont_->getLayout()
        .setAllowOverflow({false, false}) /* In this context, it shall never have overflow enabled */
        .setType(Layout::Type::VERTICAL)
        .setScaleType({Layout::ScaleType::REL, Layout::ScaleType::REL})
        .setScale({1.0f, 1.0f});
    boxCont_->setColor(Utils::hexToVec4("#42056bff"));
    // boxCont_->getListeners().setOnMouseButtonLeftClick(std::bind(&RecycleList::onMouseButtonNotify, this));
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
    int32_t rowSizeAndMargin = rowSize_ + rowMargin_;
    if (listIsDirty_ || lastScaleY_ != transform_.scale.y)
    {
        int32_t totalElements = listItems_.size();
        slider_->setSlideTo(std::max(totalElements * rowSizeAndMargin - transform_.scale.y, 0.0f));

        if (slider_->getSlideTo() == 0 && children_.size() == 2)
        {
            slider_->setSlideCurrentValue(0);
            remove(slider_->getId());
        }
        else if (slider_->getSlideTo() > 0 && children_.size() == 1)
        {
            appendAt(slider_, 0);
        }
    }

    int32_t maxDisplayAmt = transform_.scale.y / rowSizeAndMargin + 1;
    int32_t topOfListIdx = slider_->getSlideCurrentValue() / rowSizeAndMargin;
    int32_t botOfListIdx = topOfListIdx + maxDisplayAmt;
    int32_t visibleNodes = botOfListIdx - topOfListIdx + 1;

    if (listIsDirty_ || topOfListIdx != oldTopOfList_ || oldVisibleNodes_ != visibleNodes)
    {
        listIsDirty_ = false;
        int32_t itemSize = listItems_.size();
        boxCont_->removeAll();
        for (int32_t i = 0; i < visibleNodes; i++)
        {
            if (topOfListIdx + i < itemSize)
            {
                auto ref = std::make_shared<Button>("ListItem");
                ref->getLayout().setMargin({(float)rowMargin_, 0, 5, 5})
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

void RecycleList::updateNodePositions()
{
    if (slider_->getSlideCurrentValue() == 0) { return; }

    auto& children = boxCont_->getChildren();
    uint32_t size = children.size();
    int32_t rowSizeAndMargin = rowSize_ + rowMargin_;
    for (uint32_t i = 0; i < size; i++)
    {
        children[i]->getTransform().pos.y -= (int32_t)slider_->getSlideCurrentValue() % rowSizeAndMargin;
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
    boxCont_->setColor(color);
    return *this;
}

RecycleList& RecycleList::setBorderColor(const glm::vec4& color)
{
    borderColor_ = color;
    return *this;
}

RecycleList& RecycleList::setRowSize(const int32_t rowSize)
{
    if (rowSize < 2 || rowSize > 200) { return *this ; }

    rowSize_ = rowSize;
    listIsDirty_ = true;
    MAKE_LAYOUT_DIRTY_AND_REQUEST_NEW_FRAME
    return *this;
}

glm::vec4 RecycleList::getColor() const { return color_; }

glm::vec4 RecycleList::getBorderColor() const { return borderColor_; }

int32_t RecycleList::getRowSize() const { return rowSize_; }

SliderPtr RecycleList::getSlider() { return slider_; }
} // msgui
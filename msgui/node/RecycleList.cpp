#include "RecycleList.hpp"

#include "msgui/MeshLoader.hpp"
#include "msgui/ShaderLoader.hpp"
#include "msgui/node/AbstractNode.hpp"
#include "msgui/node/Button.hpp"
#include "msgui/node/FrameState.hpp"
#include "msgui/node/Slider.hpp"
#include "msgui/nodeEvent/LMBItemRelease.hpp"
#include "msgui/nodeEvent/LMBRelease.hpp"
#include "msgui/nodeEvent/Scroll.hpp"
#include <vector>

namespace msgui
{
RecycleList::RecycleList(const std::string& name) : AbstractNode(name, NodeType::RECYCLE_LIST)
{
    setShader(ShaderLoader::loadShader("assets/shader/sdfRect.glsl"));
    setMesh(MeshLoader::loadQuad());
    log_ = ("RecycleList(" + name + ")");

    setupLayoutReloadables();

    /* Defaults */
    color_ = Utils::hexToVec4("#F9F8F7");

    layout_.setScale({100, 100});

    /* RL has a slider to scroll the items. This needs to exist but doesn't need to be appended if not needed. */
    slider_ = std::make_shared<Slider>("RLSlider");
    slider_->getLayout()
        .setType(Layout::Type::VERTICAL)
        .setScaleType({Layout::ScaleType::PX, Layout::ScaleType::REL})
        .setScale({20, 1.0f});
    slider_->setColor(Utils::hexToVec4("#ddaaffff"));
    slider_->setSlideFrom(0);
    slider_->getEvents().listen<nodeevent::Scroll>(
        std::bind(&RecycleList::onSliderValueChanged, this, std::placeholders::_1));

    /* RL has a box container to hold the items. This has to be appended from the start. */
    boxCont_ = std::make_shared<Box>("RLBox");
    boxCont_->getLayout()
        .setAllowOverflow({false, false}) /* In this context, it shall never have overflow enabled */
        .setType(Layout::Type::VERTICAL)
        .setScaleType({Layout::ScaleType::REL, Layout::ScaleType::PX})
        .setScale({1.0f, 1.0f});
    boxCont_->setColor(Utils::hexToVec4("#42056bff"));
    append(boxCont_);
}

void RecycleList::addItem(const glm::vec4& color)
{
    listItems_.emplace_back(color);

    listIsDirty_ = true;
    MAKE_LAYOUT_DIRTY;
}

void RecycleList::removeItemIdx(const int32_t idx)
{   
    if (idx < 0 || idx > (int32_t)listItems_.size() - 1) { return; }
    listItems_.erase(listItems_.begin() + idx);

    listIsDirty_ = true;
    MAKE_LAYOUT_DIRTY;
}

void RecycleList::removeItemsBy(const std::function<bool(const glm::vec4&)> pred)
{
    if (std::erase_if(listItems_, pred))
    {
        listIsDirty_ = true;
        MAKE_LAYOUT_DIRTY;
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
    int32_t rowSizeAndMargin = rowSize_ + itemMargin_.top + itemMargin_.bot;
    const auto& boxLayout = boxCont_->getLayout();
    float scaleY = boxCont_->getTransform().scale.y - boxLayout.border.top - boxLayout.border.bot
        - boxLayout.padding.top - boxLayout.padding.bot;
    if (listIsDirty_ || lastScaleY_ != scaleY)
    {
        int32_t totalElements = listItems_.size();
        slider_->setSlideTo(std::max(totalElements * rowSizeAndMargin - scaleY, 0.0f));

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

    int32_t maxDisplayAmt = scaleY / rowSizeAndMargin + 1;
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
                auto ref = Utils::make<Button>("ListItem");
                ref->getLayout()
                    .setMargin(itemMargin_)
                    .setBorder(itemBorder_)
                    .setBorderRadius(itemBorderRadius_)
                    .setScaleType({Layout::ScaleType::REL, Layout::ScaleType::REL})
                    .setScale({1.0f, rowSize_});
                ref->setColor(listItems_[topOfListIdx + i]);

                ref->getEvents().listen<nodeevent::LMBRelease>([this, index = topOfListIdx + i](const auto&)
                {
                    nodeevent::LMBItemRelease evt{index};
                    getEvents().notifyEvent<nodeevent::LMBItemRelease>(evt);
                });
                boxCont_->append(ref);
            }
        }
    }

    updateNodePositions();

    oldTopOfList_ = topOfListIdx;
    oldVisibleNodes_ = visibleNodes;
    lastScaleY_ = scaleY;
}

void RecycleList::onSliderValueChanged(const nodeevent::Scroll& evt)
{
    (void)evt.value;
    updateNodePositions();
}

void RecycleList::updateNodePositions()
{
    if (slider_->getSlideCurrentValue() == 0) { return; }

    auto& children = boxCont_->getChildren();
    uint32_t size = children.size();
    int32_t rowSizeAndMargin = rowSize_ + itemMargin_.top + itemMargin_.bot;
    for (uint32_t i = 0; i < size; i++)
    {
        children[i]->getTransform().pos.y -= (int32_t)slider_->getSlideCurrentValue() % rowSizeAndMargin;
    }
}

void RecycleList::setupLayoutReloadables()
{
    layout_.onTypeChange = [this]()
    {
        MAKE_LAYOUT_DIRTY;
    };

    auto updateCb = [this ](){ MAKE_LAYOUT_DIRTY_AND_REQUEST_NEW_FRAME; };

    /* Layout will auto recalculate and new frame will be requested on layout data changes. */
    layout_.onMarginChange = updateCb;
    layout_.onPaddingChange = updateCb;
    layout_.onBorderChange = updateCb;
    layout_.onBorderRadiusChange = updateCb;
    layout_.onAlignSelfChange = updateCb;
    layout_.onScaleTypeChange = updateCb;
    layout_.onGridStartRCChange = updateCb;
    layout_.onGridSpanRCChange = updateCb;
    layout_.onScaleChange = updateCb;
    layout_.onMinScaleChange = updateCb;
    layout_.onMaxScaleChange = updateCb;
}

RecycleList& RecycleList::setColor(const glm::vec4& color)
{
    color_ = color;
    boxCont_->setColor(color);
    REQUEST_NEW_FRAME;
    return *this;
}

RecycleList& RecycleList::setBorderColor(const glm::vec4& color)
{
    borderColor_ = color;
    REQUEST_NEW_FRAME;
    return *this;
}

RecycleList& RecycleList::setRowSize(const int32_t rowSize)
{
    if (rowSize < 2 || rowSize > 200) { return *this ; }

    rowSize_ = rowSize;
    listIsDirty_ = true;
    MAKE_LAYOUT_DIRTY_AND_REQUEST_NEW_FRAME;
    return *this;
}

RecycleList& RecycleList::setItemMargin(const Layout::TBLR margin)
{
    itemMargin_ = margin;
    listIsDirty_ = true;
    MAKE_LAYOUT_DIRTY_AND_REQUEST_NEW_FRAME;
    return *this;
}

RecycleList& RecycleList::setItemBorder(const Layout::TBLR border)
{
    itemBorder_ = border;
    listIsDirty_ = true;
    MAKE_LAYOUT_DIRTY_AND_REQUEST_NEW_FRAME;
    return *this;
}

RecycleList& RecycleList::setItemBorderRadius(const Layout::TBLR borderRadius)
{
    itemBorderRadius_ = borderRadius;
    listIsDirty_ = true;
    MAKE_LAYOUT_DIRTY_AND_REQUEST_NEW_FRAME;
    return *this;
}

glm::vec4 RecycleList::getColor() const { return color_; }

glm::vec4 RecycleList::getBorderColor() const { return borderColor_; }

int32_t RecycleList::getRowSize() const { return rowSize_; }

Layout::TBLR RecycleList::getItemMargin() const { return itemMargin_; }

Layout::TBLR RecycleList::getItemBorder() const { return itemBorder_; }

Layout::TBLR RecycleList::getItemBorderRadius() const { return itemBorderRadius_; }

SliderWPtr RecycleList::getSlider() { return slider_; }

BoxWPtr RecycleList::getContainer() { return boxCont_; }

} // msgui
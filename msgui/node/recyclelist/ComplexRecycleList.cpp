#include "ComplexRecycleList.hpp"

#include "msgui/MeshLoader.hpp"
#include "msgui/ShaderLoader.hpp"
#include "msgui/node/AbstractNode.hpp"
#include "msgui/node/Button.hpp"
#include "msgui/node/FrameState.hpp"
#include "msgui/node/Slider.hpp"
#include "msgui/nodeEvent/LMBRelease.hpp"
#include "msgui/nodeEvent/Scroll.hpp"

namespace msgui::recyclelist
{
ComplexRecycleList::ComplexRecycleList(const std::string& name) : AbstractNode(name, NodeType::RECYCLE_LIST)
{
    setShader(ShaderLoader::loadShader("assets/shader/sdfRect.glsl"));
    setMesh(MeshLoader::loadQuad());
    log_ = ("ComplexRecycleList(" + name + ")");

    setupLayoutReloadables();

    /* Defaults */
    color_ = Utils::hexToVec4("#F9F8F7");

    layout_.setScale({100, 100});

    /* RL has a slider to scroll the items. This needs to exist but doesn't need to be appended if not needed. */
    slider_ = std::make_shared<Slider>("RLSlider");
    slider_->getLayout()
        .setType(Layout::Type::VERTICAL)
        .setScaleType({Layout::ScaleType::ABS, Layout::ScaleType::REL})
        .setScale({20, 1.0f});
    slider_->setColor(Utils::hexToVec4("#ddaaffff"));
    slider_->setSlideFrom(0);
    slider_->getEvents().listen<nodeevent::Scroll>(
        std::bind(&ComplexRecycleList::onSliderValueChanged, this, std::placeholders::_1));

    /* RL has a box container to hold the items. This has to be appended from the start. */
    boxCont_ = std::make_shared<Box>("RLBox");
    boxCont_->getLayout()
        .setAllowOverflow({false, false}) /* In this context, it shall never have overflow enabled */
        .setType(Layout::Type::VERTICAL)
        .setScaleType({Layout::ScaleType::REL, Layout::ScaleType::REL})
        .setScale({1.0f, 1.0f});
    boxCont_->setColor(Utils::hexToVec4("#42056bff"));
    boxCont_->setColor(Utils::hexToVec4("#4aabebff"));
    append(boxCont_);
}

void ComplexRecycleList::addItem(const glm::vec4& color)
{
    ListItem item;
    item.color = color;
    if (listItems_.size() % 3 == 0)
    {
        item.push = 1.0f;
    }
    else if (listItems_.size() % 3 == 1)
    {
        item.push = 0.75f;
    }
    else
    {
        item.push = 0.50f;
    }
    listItems_.emplace_back(item);

    listIsDirty_ = true;
    MAKE_LAYOUT_DIRTY
}

void ComplexRecycleList::removeItem(const int32_t idx)
{   
    if (idx < 0 || idx > (int32_t)listItems_.size() - 1) { return; }
    listItems_.erase(listItems_.begin() + idx);

    listIsDirty_ = true;
    MAKE_LAYOUT_DIRTY
}

void ComplexRecycleList::removeTailItems(const int32_t amount)
{
    for (int32_t i = 0; i < amount; i++)
    {
        removeItem(listItems_.size() - i - 1);
    }
}

void ComplexRecycleList::setShaderAttributes()
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

void ComplexRecycleList::onLayoutUpdateNotify()
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
    int32_t visibleNodes = maxDisplayAmt + 1;

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
                ref->getLayout()
                    .setAlignSelf(Layout::Align::RIGHT)
                    .setMargin({(float)rowMargin_, 0, 0, 0})
                    .setScaleType({Layout::ScaleType::REL, Layout::ScaleType::ABS})
                    // .setScale({1.0f, rowSize_});
                    .setScale({listItems_[topOfListIdx + i].push, rowSize_});
                ref->setColor(listItems_[topOfListIdx + i].color);
                boxCont_->append(ref);

                if (listItems_[topOfListIdx + i].push == 0.50f)
                {
                    // log_.debugLn("pe aici %f", listItems_[topOfListIdx + i].push);
                    ref->getEvents().listen<nodeevent::LMBRelease>([this, index = topOfListIdx + i](const auto&)
                    {
                        log_.debugLn("bag element %d", index);
                        auto it = listItems_.begin() + index + 1;
                        ListItem li;
                        li.color = Utils::hexToVec4("#f700ffff");
                        li.push = 0.5f;
                        listItems_.insert(it, li);
                        listIsDirty_ = true;
                        MAKE_LAYOUT_DIRTY;
                    });
                }
            }
        }
    }

    updateNodePositions();

    oldTopOfList_ = topOfListIdx;
    oldVisibleNodes_ = visibleNodes;
    lastScaleY_ = transform_.scale.y;
}

void ComplexRecycleList::onSliderValueChanged(nodeevent::Scroll evt)
{
    (void)evt.value;
    updateNodePositions();
}

void ComplexRecycleList::updateNodePositions()
{
    if (slider_->getSlideCurrentValue() == 0) { return; }

    auto& children = boxCont_->getChildren();
    uint32_t size = children.size();
    int32_t rowSizeAndMargin = rowSize_ + rowMargin_;
    for (uint32_t i = 0; i < size; i++)
    {
        children[i]->getTransform().pos.y -= (int32_t)slider_->getSlideCurrentValue() % rowSizeAndMargin;
        // children[i]->getTransform().pos.y -= (int32_t)slider_->getSlideCurrentValue();// % rowSizeAndMargin;
    }
}

void ComplexRecycleList::setupLayoutReloadables()
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

ComplexRecycleList& ComplexRecycleList::setColor(const glm::vec4& color)
{
    color_ = color;
    boxCont_->setColor(color);
    return *this;
}

ComplexRecycleList& ComplexRecycleList::setBorderColor(const glm::vec4& color)
{
    borderColor_ = color;
    return *this;
}

ComplexRecycleList& ComplexRecycleList::setRowSize(const int32_t rowSize)
{
    if (rowSize < 2 || rowSize > 200) { return *this ; }

    rowSize_ = rowSize;
    listIsDirty_ = true;
    MAKE_LAYOUT_DIRTY_AND_REQUEST_NEW_FRAME
    return *this;
}

glm::vec4 ComplexRecycleList::getColor() const { return color_; }

glm::vec4 ComplexRecycleList::getBorderColor() const { return borderColor_; }

int32_t ComplexRecycleList::getRowSize() const { return rowSize_; }

SliderPtr ComplexRecycleList::getSlider() { return slider_; }
} // msgui::recyclelist
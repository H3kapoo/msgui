#include "TreeView.hpp"

#include <algorithm>
#include <ranges>
#include <stack>

#include "msgui/MeshLoader.hpp"
#include "msgui/ShaderLoader.hpp"
#include "msgui/node/AbstractNode.hpp"
#include "msgui/node/Button.hpp"
#include "msgui/node/FrameState.hpp"
#include "msgui/node/Slider.hpp"
#include "msgui/nodeEvent/LMBRelease.hpp"
#include "msgui/nodeEvent/Scroll.hpp"

namespace msgui
{
TreeView::TreeView(const std::string& name) : AbstractNode(name, NodeType::TREEVIEW)
{
    setShader(ShaderLoader::loadShader("assets/shader/sdfRect.glsl"));
    setMesh(MeshLoader::loadQuad());
    log_ = ("TreeView(" + name + ")");

    setupLayoutReloadables();

    /* Defaults */
    color_ = Utils::hexToVec4("#F9F8F7");

    layout_.setScale({100, 100});

    /* RL has a slider to scroll the items. This needs to exist but doesn't need to be appended if not needed. */
    slider_ = std::make_shared<Slider>("TVSlider");
    slider_->getLayout()
        .setType(Layout::Type::VERTICAL)
        .setScaleType({Layout::ScaleType::ABS, Layout::ScaleType::REL})
        .setScale({20, 1.0f});
    slider_->setColor(Utils::hexToVec4("#ddaaffff"));
    slider_->setSlideFrom(0);
    slider_->getEvents().listen<nodeevent::Scroll>(
        std::bind(&TreeView::onSliderValueChanged, this, std::placeholders::_1));

    /* RL has a box container to hold the items. This has to be appended from the start. */
    boxCont_ = std::make_shared<Box>("TVBox");
    boxCont_->getLayout()
        .setAllowOverflow({false, false}) /* In this context, it shall never have overflow enabled */
        .setType(Layout::Type::VERTICAL)
        .setAlignChild(Layout::Align::LEFT)
        .setScaleType({Layout::ScaleType::REL, Layout::ScaleType::REL})
        .setScale({1.0f, 1.0f});
    boxCont_->setColor(Utils::hexToVec4("#42056bff"));
    append(boxCont_);
}

void TreeView::addItem(const TreeItemPtr& tree)
{
    //TODO: Maybe we dont need to keep both treeItems and a flat version
    // Maybe the flat one is enough.
    treeItems_.emplace_back(tree);
    recalc();
}

void TreeView::recalc()
{
    flatTreeItems_.clear();

    std::stack<TreeItemPtr> st;
    for (auto& item : treeItems_)
    {
        st.push(item);
    }

    while (!st.empty())
    {
        TreeItemPtr currentTreeItem = st.top();
        st.pop();

        flatTreeItems_.push_back(currentTreeItem);

        /* Reverse needed because otherwise children will be displayed in reverse order due to how
           stack traversal works.*/
        for (auto& item : currentTreeItem->items | std::views::reverse)
        {
            if (currentTreeItem->isOpen)
            {
                st.push(item);
            }
        }
    }
}

void TreeView::printTreeView()
{
    for (const auto& item : flatTreeItems_)
    {
        log_.raw("%*s- depth(%d) col(%.2f %.2f %.2f %.2f)\n", item->depth*4, "", item->depth,
            item->color.r, item->color.g, item->color.b, item->color.a);
    }
}

void TreeView::removeItemIdx(const int32_t idx)
{   
    // if (idx < 0 || idx > (int32_t)listItems_.size() - 1) { return; }
    // listItems_.erase(listItems_.begin() + idx);

    // listIsDirty_ = true;
    // MAKE_LAYOUT_DIRTY
}

void TreeView::setShaderAttributes()
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

void TreeView::onLayoutUpdateNotify()
{
    int32_t rowSizeAndMargin = rowSize_ + itemMargin_.top + itemMargin_.bot;
    if (listIsDirty_ || lastScaleY_ != transform_.scale.y)
    {
        int32_t totalElements = flatTreeItems_.size();
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
        int32_t itemSize = flatTreeItems_.size();
        boxCont_->removeAll();
        for (int32_t i = 0; i < visibleNodes; i++)
        {
            if (topOfListIdx + i < itemSize)
            {
                auto ref = std::make_shared<Button>("TVItem");
                ref->getLayout()
                    .setMargin(itemMargin_)
                    .setBorder(itemBorder_)
                    .setAlignSelf(Layout::Align::LEFT)
                    .setScale({100, rowSize_})
                    .setMargin({0, 0, float(flatTreeItems_[topOfListIdx + i]->depth*20), 0})
                    ;
                    ref->setColor(flatTreeItems_[topOfListIdx + i]->color);
                boxCont_->append(ref);

                ref->getEvents().listen<nodeevent::LMBRelease>(
                    [this, index = topOfListIdx + i](const auto&)
                    {
                        // log_.debugLn("opening element %d", index);
                        flatTreeItems_[index]->toggle();
                        recalc();
                        listIsDirty_ = true;
                        MAKE_LAYOUT_DIRTY;
                    });
            }
        }
    }

    updateNodePositions();

    oldTopOfList_ = topOfListIdx;
    oldVisibleNodes_ = visibleNodes;
    lastScaleY_ = transform_.scale.y;
}

void TreeView::onSliderValueChanged(const nodeevent::Scroll& evt)
{
    (void)evt.value;
    updateNodePositions();
}

void TreeView::updateNodePositions()
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

void TreeView::setupLayoutReloadables()
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

TreeView& TreeView::setColor(const glm::vec4& color)
{
    color_ = color;
    boxCont_->setColor(color);
    return *this;
}

TreeView& TreeView::setBorderColor(const glm::vec4& color)
{
    borderColor_ = color;
    return *this;
}

TreeView& TreeView::setRowSize(const int32_t rowSize)
{
    if (rowSize < 2 || rowSize > 200) { return *this ; }

    rowSize_ = rowSize;
    listIsDirty_ = true;
    MAKE_LAYOUT_DIRTY_AND_REQUEST_NEW_FRAME
    return *this;
}

TreeView& TreeView::setItemMargin(const Layout::TBLR margin)
{
    itemMargin_ = margin;
    listIsDirty_ = true;
    MAKE_LAYOUT_DIRTY_AND_REQUEST_NEW_FRAME;
    return *this;
}

TreeView& TreeView::setItemBorder(const Layout::TBLR border)
{
    itemBorder_ = border;
    listIsDirty_ = true;
    MAKE_LAYOUT_DIRTY_AND_REQUEST_NEW_FRAME;
    return *this;
}

TreeView& TreeView::setItemBorderRadius(const Layout::TBLR borderRadius)
{
    itemBorderRadius_ = borderRadius;
    listIsDirty_ = true;
    MAKE_LAYOUT_DIRTY_AND_REQUEST_NEW_FRAME;
    return *this;
}

glm::vec4 TreeView::getColor() const { return color_; }

glm::vec4 TreeView::getBorderColor() const { return borderColor_; }

int32_t TreeView::getRowSize() const { return rowSize_; }

Layout::TBLR TreeView::getItemMargin() const { return itemMargin_; }

Layout::TBLR TreeView::getItemBorder() const { return itemBorder_; }

Layout::TBLR TreeView::getItemBorderRadius() const { return itemBorderRadius_; }

SliderWPtr TreeView::getSlider() { return slider_; }

BoxWPtr TreeView::getContainer() { return boxCont_; }
} // msgui
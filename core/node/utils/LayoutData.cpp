#include "LayoutData.hpp"

namespace msgui
{
Layout& Layout::setType(const Type typeIn)
{
    type = typeIn;
    onTypeChange();
    return *this;
}

Layout& Layout::setAllowWrap(const bool allowIn)
{
    allowWrap = allowIn;
    onAllowWrapChange();
    return *this;
}

Layout& Layout::setAllowOverflow(const AllowXY allowIn)
{
    allowOverflow = allowIn;
    onAllowOverflowChange();
    return *this;
}

Layout& Layout::setMargin(const TBLR valueIn)
{
    margin = valueIn;
    onMarginChange();
    return *this;
}

Layout& Layout::setPadding(const TBLR valueIn)
{
    padding = valueIn;
    onPaddingChange();
    return *this;
}

Layout& Layout::setBorder(const TBLR valueIn)
{
    border = valueIn;
    onBorderChange();
    return *this;
}

Layout& Layout::setBorderRadius(const TBLR valueIn)
{
    borderRadius = valueIn;
    onBorderRadiusChange();
    return *this;
}

Layout& Layout::setAlignSelf(const Align valueIn)
{
    alignSelf = valueIn;
    onAlignSelfChange();
    return *this;
}

Layout& Layout::setAlignChild(const AlignXY valueIn)
{
    alignChild = valueIn;
    onAlignChildChange();
    return *this;
}

Layout& Layout::setSpacing(const Spacing valueIn)
{
    spacing = valueIn;
    onSpacingChange();
    return *this;
}

Layout& Layout::setScaleType(const ScaleTypeXY valueIn)
{
    scaleType = valueIn;
    onScaleTypeChange();
    return *this;
}

Layout& Layout::setScaleType(const ScaleType valueIn)
{
    return setScaleType({valueIn, valueIn});
}

Layout& Layout::setGridDistrib(const DistribRC valueIn)
{
    gridDistrib = valueIn;
    onGridDistribChange();
    return *this;
}

Layout& Layout::setGridStartRC(const GridRC valueIn)
{
    gridStartRC = valueIn;
    onGridStartRCChange();
    return *this;
}

Layout& Layout::setGridSpanRC(const GridRC valueIn)
{
    gridSpanRC = valueIn;
    onGridSpanRCChange();
    return *this;
}

Layout& Layout::setScale(const glm::vec2 valueIn)
{
    scale = valueIn;
    onScaleChange();
    return *this;
}

Layout& Layout::setMinScale(const glm::vec2 valueIn)
{
    minScale = valueIn;
    onMinScaleChange();
    return *this;
}

Layout& Layout::setMaxScale(const glm::vec2 valueIn)
{
    maxScale = valueIn;
    onMaxScaleChange();
    return *this;
}
} // namespace msgui
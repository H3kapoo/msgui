#include "LayoutData.hpp"

namespace msgui
{
Layout& Layout::setType(const Type typeIn)
{
    type = typeIn;
    _onTypeChange();
    return *this;
}

Layout& Layout::setAllowWrap(const bool allowIn)
{
    allowWrap = allowIn;
    _onAllowWrapChange();
    return *this;
}

Layout& Layout::setAllowOverflow(const AllowXY allowIn)
{
    allowOverflow = allowIn;
    _onAllowOverflowChange();
    return *this;
}

Layout& Layout::setMargin(const TBLR valueIn)
{
    margin = valueIn;
    _onMarginChange();
    return *this;
}

Layout& Layout::setPadding(const TBLR valueIn)
{
    padding = valueIn;
    _onPaddingChange();
    return *this;
}

Layout& Layout::setBorder(const TBLR valueIn)
{
    border = valueIn;
    _onBorderChange();
    return *this;
}

Layout& Layout::setBorderRadius(const TBLR valueIn)
{
    borderRadius = valueIn;
    _onBorderRadiusChange();
    return *this;
}

Layout& Layout::setAlignSelf(const Align valueIn)
{
    alignSelf = valueIn;
    _onAlignSelfChange();
    return *this;
}

Layout& Layout::setAlignChild(const AlignXY valueIn)
{
    alignChild = valueIn;
    _onAlignChildChange();
    return *this;
}

Layout& Layout::setSpacing(const Spacing valueIn)
{
    spacing = valueIn;
    _onSpacingChange();
    return *this;
}

Layout& Layout::setScaleType(const ScaleTypeXY valueIn)
{
    scaleType = valueIn;
    _onScaleTypeChange();
    return *this;
}

Layout& Layout::setScale(const glm::vec2 valueIn)
{
    scale = valueIn;
    _onScaleChange();
    return *this;
}

Layout& Layout::setMinScale(const glm::vec2 valueIn)
{
    minScale = valueIn;
    _onMinScaleChange();
    return *this;
}

Layout& Layout::setMaxScale(const glm::vec2 valueIn)
{
    maxScale = valueIn;
    _onMaxScaleChange();
    return *this;
}
} // namespace msgui
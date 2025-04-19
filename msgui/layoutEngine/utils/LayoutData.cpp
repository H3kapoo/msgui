#include "LayoutData.hpp"

namespace msgui::layoutengine::utils
{

#define DEFINE_FUNC(funcName, valueName, valueType, functionToCall)\
Layout& Layout:: funcName (const valueType value)\
{\
    valueName = value;\
    functionToCall();\
    return *this;\
}\

DEFINE_FUNC(setType, type, Type, onTypeChange);
DEFINE_FUNC(setNewScale, newScale, ScaleXY, onTypeChange);
DEFINE_FUNC(setAllowWrap, allowWrap, bool, onAllowWrapChange);
DEFINE_FUNC(setAllowOverflow, allowOverflow, AllowXY, onAllowOverflowChange);
DEFINE_FUNC(setMargin, margin, TBLR, onMarginChange);
DEFINE_FUNC(setPadding, padding, TBLR, onPaddingChange);
DEFINE_FUNC(setBorder, border, TBLR, onBorderChange);
DEFINE_FUNC(setBorderRadius, borderRadius, TBLR, onBorderRadiusChange);
DEFINE_FUNC(setAlignSelf, alignSelf, Align, onAlignSelfChange);
DEFINE_FUNC(setAlignChild, alignChild, AlignXY, onAlignChildChange);
DEFINE_FUNC(setSpacing, spacing, Spacing, onSpacingChange);
DEFINE_FUNC(setScaleType, scaleType, ScaleTypeXY, onScaleTypeChange);
DEFINE_FUNC(setMinScale, minScale, glm::vec2, onMinScaleChange);
DEFINE_FUNC(setMaxScale, maxScale, glm::vec2, onMaxScaleChange);

Layout& Layout::setNewScale(const Scale valueIn)
{
    return setNewScale({valueIn, valueIn});
}

//TO BE INVESTIGATED IF NEEDED STILL
Layout& Layout::setGridDistrib(const GridDistribRC valueIn)
{
    gridDistrib = valueIn;
    onGridDistribChange();
    return *this;
}

//TO BE INVESTIGATED IF NEEDED STILL
Layout& Layout::setGridStartRC(const GridRC valueIn)
{
    gridStartRC = valueIn;
    onGridStartRCChange();
    return *this;
}

//TO BE INVESTIGATED IF NEEDED STILL
Layout& Layout::setGridSpanRC(const GridRC valueIn)
{
    gridSpanRC = valueIn;
    onGridSpanRCChange();
    return *this;
}

//DEPRECATED
Layout& Layout::setScale(const glm::vec2 valueIn)
{
    scale = valueIn;
    onScaleChange();
    return *this;
}

//DEPRECATED
Layout& Layout::setScale(const float valueIn)
{
    return setScale({valueIn, valueIn});
}

Layout& Layout::setAlignChild(const Align valueIn)
{
    return setAlignChild({valueIn, valueIn});
}

Layout& Layout::setScaleType(const ScaleType valueIn)
{
    return setScaleType({valueIn, valueIn});
}

Layout::Scale operator"" _fill(unsigned long long val)
{
    return {.type = Layout::ScaleType::FILL, .value = 1};
}

Layout::Scale operator"" _fit(unsigned long long)
{
    return {.type = Layout::ScaleType::FIT, .value = 1};
}

Layout::Scale operator"" _rel(long double value)
{
    /* Loss of precision justified. */
    return {.type = Layout::ScaleType::REL, .value = (float)value};
}

Layout::Scale operator"" _px(unsigned long long value)
{
    return {.type = Layout::ScaleType::PX, .value = (float)value};
}
} // namespace msgui::layoutengine::utils
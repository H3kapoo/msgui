#pragma once

#include "msgui/Logger.hpp"
#include <cstdint>
#include <functional>

#include <glm/glm.hpp>

namespace msgui::layoutengine::utils
{

/* Defines all the layout properties of a UI node. */
struct Layout
{
    /* Defines how nodes will be arranged inside the parent. */
    enum class Type : uint8_t { HORIZONTAL, VERTICAL, GRID };

    /* Defines how nodes will be aligned inside the parent. */
    enum Align : uint8_t
    {
        /* HORIZONTAL/VERTICAL Type specific */
        TOP, LEFT, BOTTOM, RIGHT,
        /* All Type specific */
        CENTER,
        /* Grid Type specific */
        TOP_LEFT, TOP_RIGHT, CENTER_LEFT, CENTER_RIGHT, CENTER_TOP, CENTER_BOTTOM, BOTTOM_LEFT, BOTTOM_RIGHT
    };

    /* Defines how nodes will be spaces from each other inside the parent. */
    enum Spacing : uint8_t { TIGHT, EVEN_WITH_NO_START_GAP, EVEN_WITH_START_GAP };

    /* Defines how nodes will be scaled. Pixel values (PX) are exactly what you think they are while
       relative values (REL 0 <= val <= 1) defines the node scale to be a fractional part of the parent's
       total scale (minus padding + borders of parent). */
    enum ScaleType : uint8_t { PX, REL, FIT, FILL};

    /* Scale type structure holding each axis. */
    struct ScaleTypeXY { ScaleType x; ScaleType y; };

    /* Align structure holding each axis. */
    struct AlignXY { Align x{Align::LEFT}; Align y{Align::TOP}; };

    /* Allow overflow structure holding each axis. */
    struct AllowXY { bool x{false}; bool y{false}; };

    /* Grid distribution details for the parent of type grid. */
    struct GridDistrib
    {
        /* Note: ABS is the same as before but FRAC (1 <= FRAC <= max_int) here refers to a fractional
           part just like in the CSS web grid mechanism. Parent padding and border sizes are taken into
           account when calculating fract parts. */
        enum Type { ABS, FRAC };

        Type type{Type::ABS};
        int32_t value{0};
        int32_t computedStart{0};
    };
    using DistribVec = std::vector<GridDistrib>;

    /* Grid distribution details for each axis. */
    struct GridDistribRC { DistribVec rows; DistribVec cols; };

    /* Grid row + col. Used to specify node grid position and span along cols and rows. */
    struct GridRC { int32_t row{0}; int32_t col{0}; };

    /* Easy structure for holding top/bot/left/right values for things like margins/paddings/borderSizes etc */
    struct TBLR
    {
        TBLR(float val) : top(val), bot(val), left(val), right(val) {}
        TBLR(float topBot, float leftRight) : top(topBot), bot(topBot), left(leftRight), right(leftRight) {}
        TBLR(float top_, float bot_, float left_, float right_)
            : top(top_), bot(bot_), left(left_), right(right_) {}

        operator glm::vec4() const { return glm::vec4{top, bot, left, right}; }

        float top{0};
        float bot{0};
        float left{0};
        float right{0};
    };

    /* Setters used due to potential layout recalc/custom logic execution needed after assignment.
       Values can be retrieved by public means however direct assignment may lead to layout or
       logic not taking effect right away. */
    Layout& setType(const Type valueIn);
    Layout& setAllowWrap(const bool valueIn);
    Layout& setAllowOverflow(const AllowXY valueIn);
    Layout& setMargin(const TBLR valueIn);
    Layout& setPadding(const TBLR valueIn);
    Layout& setBorder(const TBLR valueIn);
    Layout& setBorderRadius(const TBLR valueIn);
    Layout& setAlignSelf(const Align valueIn);
    Layout& setAlignChild(const AlignXY valueIn);
    Layout& setAlignChild(const Align valueIn);
    Layout& setSpacing(const Spacing valueIn);
    Layout& setScaleType(const ScaleTypeXY valueIn);
    Layout& setScaleType(const ScaleType valueIn);
    Layout& setGridDistrib(const GridDistribRC valueIn);
    Layout& setGridStartRC(const GridRC valueIn);
    Layout& setGridSpanRC(const GridRC valueIn);
    Layout& setScale(const glm::vec2 valueIn);
    Layout& setScale(const float valueIn);
    Layout& setMinScale(const glm::vec2 valueIn);
    Layout& setMaxScale(const glm::vec2 valueIn);

    struct Scale
    {
        ScaleType type{ScaleType::PX};
        float value{1};
    };

    struct ScaleXY
    {
        Scale x{};
        Scale y{};
    };
    Layout& setNewScale(const ScaleXY valueIn);
    Layout& setNewScale(const Scale valueIn);

    AllowXY allowOverflow     {false};
    bool allowWrap            {false};
    Type type                 {Type::HORIZONTAL};
    TBLR margin               {TBLR{0}};
    TBLR padding              {TBLR{0}};
    TBLR border               {TBLR{0}};
    TBLR borderRadius         {TBLR{0}};
    Align alignSelf           {Align::TOP};
    AlignXY alignChild        {Align::LEFT, Align::TOP};
    Spacing spacing           {Spacing::TIGHT};
    ScaleTypeXY scaleType     {ScaleType::PX, ScaleType::PX};
    GridDistribRC gridDistrib {DistribVec{GridDistrib{GridDistrib::Type::FRAC, 1}},
                               DistribVec{GridDistrib{GridDistrib::Type::FRAC, 1}}};
    GridRC gridStartRC        {0, 0};
    GridRC gridSpanRC         {1, 1};
    glm::vec2 scale           {0, 0};
    glm::vec2 minScale        {0, 0};
    glm::vec2 maxScale        {10'000, 10'000};
    glm::vec2 shrink          {0, 0};

    ScaleXY newScale{};

    /* Functions that will be ran upon setting new values. Usually the nodes execute custom logic using
       these but the user can also set them to do custom logic at their discretion. */
    std::function<void()> onAllowOverflowChange {[](){}};
    std::function<void()> onAllowWrapChange {[](){}};
    std::function<void()> onTypeChange {[](){}};
    std::function<void()> onMarginChange {[](){}};
    std::function<void()> onPaddingChange {[](){}};
    std::function<void()> onBorderChange {[](){}};
    std::function<void()> onBorderRadiusChange {[](){}};
    std::function<void()> onAlignSelfChange {[](){}};
    std::function<void()> onAlignChildChange {[](){}};
    std::function<void()> onSpacingChange {[](){}};
    std::function<void()> onScaleTypeChange {[](){}};
    std::function<void()> onGridDistribChange {[](){}};
    std::function<void()> onGridStartRCChange {[](){}};
    std::function<void()> onGridSpanRCChange {[](){}};
    std::function<void()> onScaleChange {[](){}};
    std::function<void()> onMinScaleChange {[](){}};
    std::function<void()> onMaxScaleChange {[](){}};

    /* Used for BoxDivider calcs. No ideea where to put it. Shall not be used by user.
       This is a design artifact and shall be addressed later. */
    glm::vec2 tempScale {0, 0};
};

} // namespace msgui::layoutengine::utils

namespace msgui
{
    using  namespace msgui::layoutengine::utils;

    Layout::Scale operator"" _fill(unsigned long long val);
    Layout::Scale operator"" _fit(unsigned long long);
    Layout::Scale operator"" _rel(long double value);
    Layout::Scale operator"" _px(unsigned long long value);
    
    inline Layout::Scale operator*(Layout::Scale lhs, float rhs)
    {
        lhs.value *= rhs;
        lhs.value = lhs.type == Layout::ScaleType::PX ? (int32_t)lhs.value : lhs.value;
        return lhs;
    }
} // msgui
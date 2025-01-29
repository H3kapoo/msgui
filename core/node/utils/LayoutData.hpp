#pragma once

#include "core/Utils.hpp"
#include <cstdint>
#include <functional>

namespace msgui
{
struct Layout
{
    enum class Type : uint8_t
    {
        HORIZONTAL,
        VERTICAL,
        GRID // not supported yet
    };

    enum Align : uint8_t
    {
        // H/V Type specific
        TOP,
        LEFT,
        BOTTOM,
        RIGHT,
        // All Type specific
        CENTER,
        // Grid Type specific
        TOP_LEFT,
        TOP_RIGHT,
        CENTER_LEFT,
        CENTER_RIGHT,
        CENTER_TOP,
        CENTER_BOTTOM,
        BOTTOM_LEFT,
        BOTTOM_RIGHT
    };

    enum Spacing : uint8_t
    {
        TIGHT,
        EVEN_WITH_NO_START_GAP,
        EVEN_WITH_START_GAP
    };

    enum ScaleType : uint8_t
    {
        ABS,
        REL
    };

    struct ScaleTypeXY
    {
        ScaleType x;
        ScaleType y;
    };

    struct AlignXY
    {
        Align x{Align::LEFT};
        Align y{Align::TOP};
    };

    struct AllowXY
    {
        bool x{false};
        bool y{false};
    };

    //TopBottomLeftRight
    struct TBLR
    {
        // Using AR here increases memory usage by x2.5
        // Maybe some ctors can be created to reload just on TBLR assignment?
        // Maybe use "-1" as a way to say keep current value?
        // = TBLR{-1, 40, -1, -1} // change Bot & leave the rest alone?
        // This way we only use one AR, not 4.

        TBLR(float val) : top(val), bot(val), left(val), right(val) {}
        TBLR(float top_, float bot_, float left_, float right_)
            : top(top_), bot(bot_), left(left_), right(right_) {}

        operator glm::vec4() const
        {
            return glm::vec4{top, bot, left, right};
        }

        float top{0};
        float bot{0};
        float left{0};
        float right{0};
    };

    struct Distrib
    {
        enum Type { ABS, FRAC };
        Type type{Type::ABS};
        int32_t value{0};
        int32_t computedStart{0};
    };
    using DistribVec = std::vector<Distrib>;

    struct DistribRC
    {
        DistribVec rows;
        DistribVec cols;
    };

    struct GridRC
    {
        int32_t row{0};
        int32_t col{0};
    };

    Layout& setType(const Type valueIn);
    Layout& setAllowWrap(const bool valueIn);
    Layout& setAllowOverflow(const AllowXY valueIn);
    Layout& setMargin(const TBLR valueIn);
    Layout& setPadding(const TBLR valueIn);
    Layout& setBorder(const TBLR valueIn);
    Layout& setBorderRadius(const TBLR valueIn);
    Layout& setAlignSelf(const Align valueIn);
    Layout& setAlignChild(const AlignXY valueIn);
    Layout& setSpacing(const Spacing valueIn);
    Layout& setScaleType(const ScaleTypeXY valueIn);
    Layout& setScaleType(const ScaleType valueIn);
    Layout& setGridDistrib(const DistribRC valueIn);
    Layout& setGridStartRC(const GridRC valueIn);
    Layout& setGridSpanRC(const GridRC valueIn);
    Layout& setScale(const glm::vec2 valueIn);
    Layout& setMinScale(const glm::vec2 valueIn);
    Layout& setMaxScale(const glm::vec2 valueIn);

    AllowXY allowOverflow {false};
    bool allowWrap        {false};
    Type type             {Type::HORIZONTAL};
    TBLR margin           {TBLR{0}};
    TBLR padding          {TBLR{0}};
    TBLR border           {TBLR{0}};
    TBLR borderRadius     {TBLR{0}};
    Align alignSelf       {Align::TOP};
    AlignXY alignChild    {Align::LEFT, Align::TOP};
    Spacing spacing       {Spacing::TIGHT};
    ScaleTypeXY scaleType {ScaleType::ABS, ScaleType::ABS};
    DistribRC gridDistrib {DistribVec{Distrib{Distrib::Type::FRAC, 1}}, DistribVec{Distrib{Distrib::Type::FRAC, 1}}};
    GridRC gridStartRC    {0, 0};
    GridRC gridSpanRC     {1, 1};
    glm::vec2 scale       {0, 0};
    glm::vec2 minScale    {0, 0};
    glm::vec2 maxScale    {10'000, 10'000};

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

    // Used for BoxDivider calcs
    glm::vec2 tempScale {0, 0};
};

} // namespace msgui
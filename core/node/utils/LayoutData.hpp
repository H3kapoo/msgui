#pragma once

#include "core/Utils.hpp"
#include <cstdint>

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

    // In case AR values end up eating too much memory, maybe we could default to setting some
    // internal var to: needsLayoutUpdate=true triggered by the user.
    // But AR is so convenient..
    AR<bool>        allowOverflowX{false};
    AR<bool>        allowOverflowY{false};
    AR<bool>        allowWrap     {false};
    AR<Type>        type          {Type::HORIZONTAL};
    AR<TBLR>        margin        {TBLR{0}};
    AR<TBLR>        padding       {TBLR{0}};
    AR<TBLR>        border        {TBLR{0}};
    AR<TBLR>        borderRadius  {TBLR{0}};
    AR<Align>       alignSelf     {Align::TOP};
    AR<Align>       alignChildX   {Align::LEFT};
    AR<Align>       alignChildY   {Align::TOP};
    AR<Spacing>     spacing       {Spacing::TIGHT};
    AR<ScaleTypeXY> scaleType     {{ScaleType::ABS, ScaleType::ABS}};
    AR<glm::vec2>   scale     {{30, 30}};
};

} // namespace msgui
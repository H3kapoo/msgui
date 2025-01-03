#pragma once

#include "core/Utils.hpp"

namespace msgui
{
struct Layout
{
    enum class Orientation
    {
        HORIZONTAL,
        VERTICAL
    };

    AR<bool>        allowOverflowX{false};
    AR<bool>        allowOverflowY{false};
    AR<bool>        allowWrap{false};
    AR<Orientation> orientation{Orientation::HORIZONTAL};
};

} // namespace msgui
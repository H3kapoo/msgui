#pragma once

#include "core/Utils.hpp"

namespace msgui
{
struct Layout
{
    enum class Orientation // TODO: rename to 'Type' as we need to support Grid also
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
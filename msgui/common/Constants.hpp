#pragma once

#include <stdint.h>

namespace msgui::common
{
static constexpr uint32_t MAX_DEFAULT_CURSORS = 6;

/*  Since scrollbars and dropdowns need to be displayed on top of the container they attach to and kinda on top of
    everything else, a custom Z start is needed. Same for floating windows or dropdowns.
*/
static constexpr uint32_t SCROLL_LAYER_START = 250;
static constexpr uint32_t DROPDOWN_LAYER_START = 300;
static constexpr uint32_t FLOATING_LAYER_START = 400;
} // namespace msgui::common
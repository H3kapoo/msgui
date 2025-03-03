#pragma once

#include "msgui/node/Dropdown.hpp"

namespace msgui
{
class ContextMenu
{
public:
    ContextMenu() = default;

    DropdownPtr dd_{nullptr};
};
} // namespace msgui
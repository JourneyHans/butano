/*
 * Copyright (c) 2020-2021 Gustavo Valiente gustavo.valiente@protonmail.com
 * zlib License, see LICENSE file.
 */

#ifndef BN_RECT_WINDOW_VERTICAL_BOUNDARIES_HBE_HANDLER_H
#define BN_RECT_WINDOW_VERTICAL_BOUNDARIES_HBE_HANDLER_H

#include "bn_any.h"
#include "bn_fixed.h"
#include "bn_display_manager.h"
#include "../hw/include/bn_hw_display.h"

namespace bn
{

class rect_window_vertical_boundaries_hbe_handler
{

public:
    [[nodiscard]] static bool target_visible(intptr_t)
    {
        return true;
    }

    static void setup_target(intptr_t, iany& target_last_value)
    {
        target_last_value = pair<int, int>();
    }

    [[nodiscard]] static bool target_updated(intptr_t target_id, iany& target_last_value)
    {
        pair<int, int>& last_value = target_last_value.value<pair<int, int>>();
        pair<int, int> new_value = display_manager::rect_window_hw_vertical_boundaries(target_id);
        bool updated = last_value != new_value;
        last_value = new_value;
        return updated;
    }

    [[nodiscard]] static uint16_t* output_register(intptr_t target_id)
    {
        return hw::display::window_vertical_boundaries_register(target_id);
    }

    static void write_output_values(intptr_t, const iany& target_last_value, const void* input_values_ptr,
                                    uint16_t* output_values_ptr)
    {
        const pair<int, int>& last_value = target_last_value.value<pair<int, int>>();
        auto fixed_pairs_ptr = reinterpret_cast<const pair<fixed, fixed>*>(input_values_ptr);
        display_manager::fill_rect_window_hblank_effect_vertical_boundaries(
                    last_value, fixed_pairs_ptr, output_values_ptr);
    }

    static void show(intptr_t)
    {
    }

    static void cleanup(intptr_t)
    {
        display_manager::reload_rect_windows_boundaries();
    }
};

}

#endif

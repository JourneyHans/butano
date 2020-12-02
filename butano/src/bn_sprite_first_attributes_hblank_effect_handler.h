/*
 * Copyright (c) 2020 Gustavo Valiente gustavo.valiente@protonmail.com
 * zlib License, see LICENSE file.
 */

#ifndef BN_SPRITE_FIRST_ATTRIBUTES_HBLANK_EFFECT_HANDLER_H
#define BN_SPRITE_FIRST_ATTRIBUTES_HBLANK_EFFECT_HANDLER_H

#include "bn_any.h"
#include "bn_sprite_first_attributes.h"
#include "bn_sprites_manager.h"
#include "bn_display_manager.h"
#include "../hw/include/bn_hw_sprites.h"

namespace bn
{

class sprite_first_attributes_hblank_effect_handler
{

public:
    static void setup_target(int, iany& target_last_value)
    {
        target_last_value = last_value_type();
    }

    [[nodiscard]] static bool target_visible(int target_id)
    {
        auto handle = reinterpret_cast<void*>(target_id);
        return sprites_manager::hw_id(handle).has_value();
    }

    [[nodiscard]] static bool target_updated(int target_id, iany& target_last_value)
    {
        last_value_type& last_value = target_last_value.value<last_value_type>();
        last_value_type new_value(target_id);
        bool updated = last_value != new_value;
        last_value = new_value;
        return updated;
    }

    [[nodiscard]] static uint16_t* output_register(int target_id)
    {
        auto handle = reinterpret_cast<void*>(target_id);
        return hw::sprites::first_attributes_register(*sprites_manager::hw_id(handle));
    }

    static void write_output_values(int, const iany& target_last_value, const void* input_values_ptr,
                                    uint16_t* output_values_ptr)
    {
        const last_value_type& last_value = target_last_value.value<last_value_type>();
        auto sprite_first_attributes_ptr = reinterpret_cast<const sprite_first_attributes*>(input_values_ptr);
        sprites_manager::fill_hblank_effect_first_attributes(
                    last_value.hw_y, bn::sprite_shape(last_value.shape), bpp_mode(last_value.bpp),
                    last_value.view_mode, sprite_first_attributes_ptr, output_values_ptr);
    }

    static void show(int)
    {
    }

    static void cleanup(int target_id)
    {
        auto handle = reinterpret_cast<void*>(target_id);
        sprites_manager::reload(handle);
    }

private:
    class last_value_type
    {

    public:
        int hw_y = -1;
        int view_mode = -1;
        uint8_t shape = 0;
        uint8_t bpp = 0;
        bool fade_enabled = false;

        last_value_type() = default;

        explicit last_value_type(void* handle) :
            hw_y(sprites_manager::hw_position(handle).y()),
            view_mode(sprites_manager::view_mode(handle)),
            shape(uint8_t(sprites_manager::shape(handle))),
            bpp(uint8_t(sprites_manager::palette(handle).bpp())),
            fade_enabled(display_manager::blending_fade_enabled())
        {
        }

        explicit last_value_type(int target_id) :
            last_value_type(reinterpret_cast<void*>(target_id))
        {
        }

        [[nodiscard]] friend bool operator==(const last_value_type& a, const last_value_type& b) = default;
    };
};

}

#endif

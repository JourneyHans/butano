#include "bf_game_hero_bullets.h"

#include "btn_math.h"
#include "btn_sound.h"
#include "btn_keypad.h"
#include "btn_sound_items.h"
#include "btn_sprite_builder.h"
#include "btn_hero_bullets_sprite_item.h"
#include "bf_constants.h"
#include "bf_game_hero.h"

namespace bf
{

namespace
{
    constexpr btn::fixed_point _unit_vector(btn::fixed x, btn::fixed y)
    {
        btn::fixed magnitude = btn::newton_raphson_sqrt((x * x) + (y * y));
        return btn::fixed_point(x, y) / magnitude;
    }

    constexpr btn::fixed_point _direction_vector(btn::fixed x, btn::fixed y, btn::fixed speed = 1)
    {
        return _unit_vector(x, y) * speed * btn::fixed(2.5);
    }

    struct shoot_event
    {
        btn::fixed_point direction;
        int8_t counter;
        int8_t level;
        bool play_sound;
    };

    struct level_data
    {
        btn::span<const shoot_event> shoot_events;
        btn::sound_item sound_item;
        btn::color color;
        int8_t max_counter;
    };

    constexpr const shoot_event _level0_shoot_events[] = {
        { _direction_vector(0, -1), 1,  0,  true },
    };

    constexpr const shoot_event _level1_shoot_events[] = {
        { _direction_vector(0, -1),         1,  1,  true },
        { _direction_vector(-0.075, -1),    11, 0,  true },
        { _direction_vector(0.075, -1),     31, 0,  true },
    };

    constexpr const shoot_event _level2_shoot_events[] = {
        { _direction_vector(0, -1, 0.7),    1,  2,  true },
        { _direction_vector(-0.085, -1),    11, 0,  false },
        { _direction_vector(0.05, -1),      21, 1,  true },
        { _direction_vector(0.085, -1),     31, 0,  false },

        { _direction_vector(0, -1, 0.7),    41, 2,  true },
        { _direction_vector(-0.085, -1),    51, 0,  false },
        { _direction_vector(-0.05, -1),     61, 1,  true },
        { _direction_vector(0.085, -1),     71, 0,  false },
    };

    constexpr const level_data _levels_data[] = {
        { _level0_shoot_events, btn::sound_items::gun_5,  btn::color(31, 28, 5),  20 },
        { _level1_shoot_events, btn::sound_items::gun_5,  btn::color(31, 28, 5),  40 },
        { _level2_shoot_events, btn::sound_items::gun_1,  btn::color(13, 2, 2),   80 },
    };
}

void game_hero_bullets::update(game_hero& hero)
{
    if(btn::keypad::held(btn::keypad::button_type::B))
    {
        _b_held_counter = 10;
    }
    else if(_b_held_counter)
    {
        --_b_held_counter;
    }

    _remove_bullets();
    hero.set_is_shooting(_b_held_counter);

    if(_shoot_event_counter || _b_held_counter)
    {
        ++_shoot_event_counter;
    }

    if(_b_held_counter)
    {
        _add_bullets(hero);
    }

    if(_shoot_event_counter == _levels_data[hero.level()].max_counter)
    {
        _shoot_event_counter = 0;
    }
}

void game_hero_bullets::_remove_bullets()
{
    int bullets_count = _sprite_move_actions.size();

    for(int index = 0; index < bullets_count; ++index)
    {
        btn::sprite_move_by_action& sprite_move_action = _sprite_move_actions[index];
        const btn::fixed_point& position = sprite_move_action.sprite().position();

        if(position.x() < -constants::view_width || position.x() > constants::view_width ||
                position.y() < -constants::view_height || position.y() > constants::view_height)
        {
            if(index < bullets_count - 1)
            {
                swap(sprite_move_action, _sprite_move_actions[bullets_count - 1]);
                --index;
                --bullets_count;
            }
        }
        else
        {
            sprite_move_action.update();
        }
    }

    _sprite_move_actions.shrink(bullets_count);
}

void game_hero_bullets::_add_bullets(game_hero& hero)
{
    int counter = _shoot_event_counter;

    for(const shoot_event& shoot_event : _levels_data[hero.level()].shoot_events)
    {
        if(counter == shoot_event.counter)
        {
            BTN_ASSERT(! _sprite_move_actions.full(), "No more space for sprite bullets");

            int shoot_event_level = shoot_event.level;
            const level_data& shoot_level_data = _levels_data[shoot_event_level];
            btn::sprite_builder builder(btn::sprite_items::hero_bullets, shoot_event.level);
            builder.set_position(hero.weapon_position());
            builder.set_z_order(constants::hero_bullets_z_order);
            _sprite_move_actions.push_back(btn::sprite_move_by_action(builder.release_build(),
                                                                      shoot_event.direction));
            hero.show_shoot(shoot_level_data.color);

            if(shoot_event.play_sound)
            {
                btn::sound::play(shoot_level_data.sound_item, 0.5);
            }

            break;
        }
    }
}

}

#include "character.h"
#include "units.h"

struct HMove
{
    // speed is in metres/second
    HMove(SDL_Rect towards, uint32_t start_time, units::Speed const &speed, Character &character)
    {
        character._update += [from = character._position, towards, speed, &character, start_time](Scene *scene) { // update until it returns falls
            auto keep_moving{true}; // true means keep updating
            auto step = units::Time{scene->age() - start_time} * speed;
            if (from.x < towards.x)
            {
                character._position.x = from.x + step;
                if (character._position.x >= towards.x)
                {
                    character._position.x = towards.x;
                    keep_moving = false;
                }
            }
            else
            {
                character._position.x = from.x - step;
                if (character._position.x <= towards.x)
                {
                    character._position.x = towards.x;
                    keep_moving = false;
                }
            }
            return keep_moving;
        };
    }
};

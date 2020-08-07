#include "character.h"
#include "units.h"

template <typename TCharacter>
struct HMove
{
    // speed is in metres/second
    HMove(SDL_Rect towards, uint32_t start_time, units::Speed const &speed, TCharacter &character)
    {
        character.addUpdate([from = character._position, towards, speed, &character, start_time](typename TCharacter::SceneType *scene) {
            auto keep_moving{true};
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
        });
    }
};

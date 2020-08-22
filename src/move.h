#include "units.h"

template <typename TCharacter>
struct HMove
{
    // speed is in metres/second
    HMove(typename TCharacter::RendererType::RectType towards, uint32_t start_time, units::Speed const &speed, TCharacter &character)
    { //  speed and character reference passing
        character.addUpdate([from = character._position, towards, speed, &character, start_time](typename TCharacter::SceneType *scene) {
            auto keep_moving{true}; // bool keep_moving and set to true
            auto step = units::Time{scene->age() - start_time} * speed; // calculate distance
            if (from.x < towards.x) // not reached
            {
                character._position.x = from.x + step; // update position
                if (character._position.x >= towards.x) // already reached?
                {
                    character._position.x = towards.x; // set final destination
                    keep_moving = false; // move set to false
                }
            }
            else // already reached
            {
                character._position.x = from.x - step; // reverse ?? why??
                if (character._position.x <= towards.x) // code duplication ?? can we add into seperate method?
                {
                    character._position.x = towards.x;
                    keep_moving = false;
                }
            }
            return keep_moving;
        });
    }
};

#include "units.h"
#include <functional>

enum HDirection
{
    left,
    right
};

template <typename TCharacter>
struct HMove
{

    auto direction() const { return _direction; }

    void cancel()
    {
        _isCancelled = true;
    }

    HMove(HDirection direction, uint32_t start_time, units::Speed const &speed, TCharacter &character)
        : _direction{direction}
    {
        AddUpdate(start_time, speed, character);
    }

private:
    HDirection _direction;
    bool _isCancelled{false};
    // speed is in metres/second
    void AddUpdate(uint32_t start_time, units::Speed const &speed, TCharacter &character)
    { //  speed and character reference passing
        if (_direction == HDirection::left)
        {
            character.addUpdate([from = character._position, speed, &character, start_time, this](typename TCharacter::SceneType *scene) {
                if (_isCancelled)
                {
                    return false;
                }
                auto step = units::Time{scene->age() - start_time} * speed; // calculate distance
                character._position.x = from.x - step;                      // update position
                return true;
            });
        }
        else
        {
            character.addUpdate([from = character._position, speed, &character, start_time, this](typename TCharacter::SceneType *scene) {
                if (_isCancelled)
                {
                    return false;
                }
                auto step = units::Time{scene->age() - start_time} * speed; // calculate distance
                character._position.x = from.x + step;                      // update position
                return true;
            });
        }
    }
};

#include "units.h"
#include <functional>

// HOMEWORK: remove "prosecution" concepts from HMove (make it simpler)

template <typename TCharacter>
struct HMove
{
    bool _isCancelled {false};

    void cancel() {
        _isCancelled = true;
    }

    HMove(typename TCharacter::RendererType::RectType towards, uint32_t start_time, units::Speed const &speed, TCharacter &character)
    {
        auto towards_getter = [towards](){ return towards; };
        AddUpdate(towards_getter, start_time, speed, character, false);
    }
    HMove(std::function<typename TCharacter::RendererType::RectType()> towards_getter, uint32_t start_time, units::Speed const &speed, TCharacter &character, bool continue_pursue = true)
    {
        AddUpdate(towards_getter, start_time, speed, character, continue_pursue);
    }
    // speed is in metres/second
    void AddUpdate(std::function<typename TCharacter::RendererType::RectType()> towards_getter, uint32_t start_time, units::Speed const &speed, TCharacter &character, bool continue_pursue)
    { //  speed and character reference passing
        if (continue_pursue) {
            character.addUpdate([from = character._position, &current = character._position, towards_getter, speed, &character, start_time, this](typename TCharacter::SceneType *scene) {
                if (_isCancelled) { return false; }
                auto step = units::Time{scene->age() - start_time} * speed; // calculate distance
                auto towards = towards_getter();
                if (current.x < towards.x) // not reached
                {
                    character._position.x = from.x + step; // update position
                }
                else // already reached
                {
                    // character._position.x = from.x - step; // reverse ?? why??
                }
                return true;
            });
        }
        else {
            character.addUpdate([from = character._position, towards_getter, speed, &character, start_time, this](typename TCharacter::SceneType *scene) {
                if (_isCancelled) { return false; }
                auto keep_moving{true}; // bool keep_moving and set to true
                auto step = units::Time{scene->age() - start_time} * speed; // calculate distance
                auto towards = towards_getter();
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
    }
};

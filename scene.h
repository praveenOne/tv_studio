#pragma once
#include <memory>
#include <vector>
#include "sdladapters.h"
#include "plane.h"

// A Scene is a collection of superposing Planes of the same size
struct Scene : public std::vector<std::unique_ptr<Plane>>
{
    Scene(unsigned int w, unsigned int h, unsigned int z)
        : _w(w), _h(h)
    {
        reserve(z);
        for (; z; --z)
        {
            push_back(std::make_unique<Plane>(w, h));
        }
        _start = ::SDL_GetTicks();
    }

    auto pixel_size() const
    {
        return SDL_Rect{0, 0, _w, _h};
    }

    auto age() const
    {
        return ::SDL_GetTicks() - _start; // how many milisecons passed since it created
    }

    template <typename T>
    void foreach_character(T action)
    {
        for (auto &plane : *this)
        {
            for (auto &ch : *plane)
            {
                action(ch);
            }
        }
    }

    void handle_event(SDL_Event *ev)
    {
        foreach_character([ev](auto &ch) {
            if (ch->_react)
            {
                ch->_react(ev);
            }
        });
    }

    void update()
    {
        foreach_character([this](auto &ch) {
            if (ch->_update)
            {
                ch->_update(this);
            }
        });
    }

private:
    Uint32 _start;
    int _w, _h;
};

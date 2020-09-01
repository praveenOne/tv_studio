#pragma once
#include <memory>
#include <vector>
#include "plane.h"

// A Scene is a collection of superposing Planes of the same size
template <typename TRenderable>
struct Scene
{
    Scene(unsigned int w, unsigned int h, unsigned int z)
        : _w(w), _h(h)
    {
        _planes.reserve(z);
        for (; z; --z)
        {
            _planes.push_back(std::make_shared<Plane<TRenderable>>(w, h)); // assign plane to the render in reverse order
        }
        _start = TRenderable::RendererType::GetTicks(); // get delta time?
    }

    auto pixel_size() const // get pixel size
    {
        return typename TRenderable::RendererType::RectType{0, 0, _w, _h};
    }

    auto age() const
    {
        return TRenderable::RendererType::GetTicks() - _start;
    }

    template <typename T>            // templeate for for each , it can be do any action
    void foreach_character(T action) // handle event for every character?
    {
        for (auto &plane : _planes)
        {
            for (auto &ch : *plane) // for each
            {
                action(ch);
            }
        }
    }

    bool handle_event(typename TRenderable::EventType *ev)
    {
        foreach_character([ev](auto &ch) { // for each react
            if (ch->_react)                // _react ??
            {
                ch->_react(ev);
            }
        });
        return true;
    }

    void update()
    {
        foreach_character([this](auto &ch) { // for each update
            ch->Update(this);
        });
    }

    auto back() const
    {
        return _planes.back();
    }

    auto at(int idx) const
    {
        return _planes.at(idx);
    }

    auto size() const { return _planes.size(); }

private:
    Uint32 _start;
    int _w, _h;
    std::vector<std::shared_ptr<Plane<TRenderable>>> _planes;
};

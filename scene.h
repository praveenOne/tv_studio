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
            _planes.push_back(std::make_shared<Plane<TRenderable>>(w, h));
        }
        _start = TRenderable::RendererType::GetTicks();
    }

    auto pixel_size() const
    {
        return typename TRenderable::RendererType::RectType{0, 0, _w, _h};
    }

    auto age() const
    {
        return TRenderable::RendererType::GetTicks() - _start;
    }

    template <typename T>
    void foreach_character(T action)
    {
        for (auto &plane : _planes)
        {
            for (auto &ch : *plane)
            {
                action(ch);
            }
        }
    }

    void handle_event(typename TRenderable::EventType *ev)
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

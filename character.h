#pragma once
#include <functional>
#include "sdladapters.h"
#include "scene.h"

class Scene;

// A Character is a renderable presence on a plane
// It may respond to events
struct Character
{
    // callbacks
    std::function<void(sdl::Renderer *, std::function<SDL_Rect(SDL_Rect)>)> _render; // when rendering position should not change
    std::function<bool(SDL_Event *)> _react; // event may react to
    std::function<bool(Scene *)> _update; // update it's position
    SDL_Rect _position;
};

// HOMEWORK: explain this operator+
// (spoiler) it implements forgetting the updates after they return "false"
std::function<bool(Scene *)> operator+(std::function<bool(Scene *)> const &a, std::function<bool(Scene *)> b)
{
    if (!a)
        return b;
    if (!b)
        return a;
    auto pa = std::make_shared<std::function<bool(Scene *)>>(a);
    auto pb = std::make_shared<std::function<bool(Scene *)>>(b);
    return [pa, pb](Scene *scene) mutable {
        bool result{false};
        if (pa)
        {
            result = pa->operator()(scene);
            if (!result)
                pa.reset();
        }
        if (pb)
        {
            if (pb->operator()(scene))
            {
                result = true;
            }
            else
            {
                pb.reset();
            }
        }
        return result;
    };
}

std::function<bool(Scene *)> &operator+=(std::function<bool(Scene *)> &a, std::function<bool(Scene *)> b)
{
    a = a + b;
    return a;
}

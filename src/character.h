#pragma once
#include <functional>

template <typename TRenderable>
struct Scene;

template <typename TScene>
std::function<bool(TScene *)> operator+(std::function<bool(TScene *)> const &a, std::function<bool(TScene *)> b)
{
    if (!a)
        return b;
    if (!b)
        return a;
    auto pa = std::make_shared<std::function<bool(TScene *)>>(a);
    auto pb = std::make_shared<std::function<bool(TScene *)>>(b);
    return [pa, pb](TScene *scene) mutable {
        bool result{false};
        if (pa)
        {
            result = (*pa)(scene);
            if (!result)
                pa.reset();
        }
        if (pb)
        {
            if ((*pb)(scene))
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

template <typename TScene>
std::function<bool(TScene *)> &operator+=(std::function<bool(TScene *)> &a, std::function<bool(TScene *)> b)
{
    a = a + b;
    return a;
}

// A Character is a renderable presence on a plane
// It may respond to events
template <typename TRenderer, typename TEvent>
struct Character
{
    typedef TEvent EventType;
    typedef TRenderer RendererType;
    typedef Scene<Character<TRenderer, TEvent>> SceneType;
    typedef std::function<bool(SceneType *)> UpdateType;

    std::function<void(TRenderer *, std::function<typename TRenderer::RectType(typename TRenderer::RectType)>)> _render;
    std::function<bool(EventType *)> _react;
    typename TRenderer::RectType _position;

    void addUpdate(UpdateType update)
    {
        _update += update;
    }

    bool Update(SceneType *scene)
    {
        if (_update)
            return _update(scene);
        return false;
    }

private:
    UpdateType _update;
};

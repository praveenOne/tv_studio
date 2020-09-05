#pragma once
#include <memory>
#include <algorithm>
#include "units.h"

// A Camera produces renders by superposing the planes of a scene, from a perspective
// camera is a class has two parameters of render and scene
template <typename TRenderer, typename TScene>
struct Camera
{
    Camera(std::shared_ptr<TRenderer> renderer) // constructor by passing render shared_ptr
        : _renderer(renderer) { ; }

    void scene(TScene const *scene_param) // setting up scene to the camera
    {
        _scene = scene_param;
        _plane_translations.resize(0); // why it resize to zero? initial vector size is not zero?
        _camera_y = scene_param->pixel_size().h / 2; 
    }

    void render()
    {
        _renderer->Clear(); // clear the render
        if (_plane_translations.empty())
        {
            create_plane_translations();
        }
        const int planes = _scene->size(); // get number of planes
        for (int idx{planes - 1}; idx >= 0; --idx) // loop from highest value
        {
            _scene->at(idx)->render(_renderer.get(), _plane_translations[idx]);
        }
        _renderer->Present(); // SDL requirement
    }

    void update()
    {
        // _camera_z = std::min({(-50) + (static_cast<int>(_scene->age()) / 100), -10});
        _camera_x = _scene->age() / 20; // speed of the cam
        // _zoom = _scene->age() * 0.00001 + 1.0;
        create_plane_translations();
    }

private:
    void create_plane_translations()
    {
        const int planes = _scene->size();
        _plane_translations.resize(planes); // does this change at runtime?
        // int reduction{-_camera_z};
        int w, h;
        _renderer->GetDimensions(&w, &h); // out parameters ? why pass the address?
        // we pass the address-of each integer, so that the fn can modify the values
        // C# ref => initialized values, pass by-ref (by-ref in C++)
        // C# out => non-initialized values, pass by-ref (no equivalent)
        for (int idx = 0; idx < planes; ++idx)
        {
            auto reduction = static_cast<uint32_t>(log(
                (_camera_z + _distance_between_planes * idx) / _aperture) * _aperture / 4); // how small are things in the far
            _plane_translations[idx] = ([this, w, h, reduction](typename TRenderer::RectType rc) {
                // center
                rc.x -= _camera_x - w / 2;
                rc.y += _camera_y - h / 2;
                // zoom
                // rc.x -= static_cast<uint32_t>(w * (1 - _zoom));
                // rc.y += static_cast<uint32_t>(h * (1 - _zoom));
                // rc.w = static_cast<uint32_t>(rc.w * _zoom);
                // rc.h = static_cast<uint32_t>(rc.h * _zoom);
                // reduce as per the distance
                rc.x += reduction * rc.w / w;
                rc.y += reduction * rc.h / h;
                rc.w -= reduction * 2 * rc.w / w;
                rc.h -= reduction * 2 * rc.h / h;
                return rc;
            });
        }
    }

    std::shared_ptr<TRenderer> _renderer;
    std::vector<std::function<typename TRenderer::RectType(typename TRenderer::RectType)>> _plane_translations;
    // System.Collections.Generic.Array<System.Func<Rect, Rect>>
    // it's a dynamic array of functions that take a Rect and return a Rect
    const units::Distance _camera_z{units::Distance::Metres(10.0)}; // camera is away 10m from the first plane
    const units::Distance _distance_between_planes{units::Distance::Metres(2.0)}; // difference between planes 
    int _camera_x;
    int _camera_y;
    double _zoom{1.0};
    const double _aperture{40.0};
    TScene const *_scene;
};

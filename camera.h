#pragma once
#include <memory>
#include <algorithm>
#include "sdladapters.h"
#include "scene.h"
#include "units.h"

// A Camera produces renders by superposing the planes of a scene, from a perspective
struct Camera
{
    Camera(std::shared_ptr<sdl::Renderer> renderer)
        : _renderer(renderer) { ; }

    void scene(Scene const *scene_param)
    {
        _scene = scene_param;
        _plane_translations.resize(0);
        _camera_y = scene_param->pixel_size().h / 2;
    }

    void render()
    {
        _renderer->Clear(sdl::Color(255, 255, 255));
        if (_plane_translations.empty())
        {
            create_plane_translations();
        }
        const int planes = _scene->size();
        for (int idx{planes - 1}; idx >= 0; --idx)
        {
            _scene->at(idx)->render(_renderer.get(), _plane_translations[idx]);
        }
        _renderer->Present();
    }

    void update()
    {
        // _camera_z = std::min({(-50) + (static_cast<int>(_scene->age()) / 100), -10});
        _camera_x = _scene->age() / 10;
        // _zoom = _scene->age() * 0.00001 + 1.0;
        create_plane_translations();
    }

private:
    void create_plane_translations()
    {
        const int planes = _scene->size();
        _plane_translations.resize(planes);
        // int reduction{-_camera_z};
        int w, h;
        _renderer->GetDimensions(&w, &h);
        for (int idx = 0; idx < planes; ++idx)
        {
            auto reduction = log((_camera_z + _distance_between_planes * idx) / _aperture) * _aperture / 4;
            _plane_translations[idx] = ([this, w, h, reduction](SDL_Rect rc) {
                // center
                rc.x -= _camera_x - w / 2;
                rc.y += _camera_y - h / 2;
                // zoom
                rc.x -= w * (1 - _zoom);
                rc.y += h * (1 - _zoom);
                rc.w = rc.w * _zoom;
                rc.h = rc.h * _zoom;
                // reduce as per the distance
                rc.x += reduction * rc.w / w;
                rc.y += reduction * rc.h / h;
                rc.w -= reduction * 2 * rc.w / w;
                rc.h -= reduction * 2 * rc.h / h;
                return rc;
            });
        }
    }

    std::shared_ptr<sdl::Renderer> _renderer;
    std::vector<std::function<SDL_Rect(SDL_Rect)>> _plane_translations;
    const units::Distance _camera_z{units::Distance::Metres(10.0)};
    const units::Distance _distance_between_planes{units::Distance::Metres(2.0)};
    int _camera_x;
    int _camera_y;
    double _zoom{1.0};
    const double _aperture{40.0};
    Scene const *_scene;
};

#pragma once
#include <string>
#include <memory>
#include <iostream>
#include "sdladapters.h"
#include "character.h"

struct Sprite
{
    Sprite(sdl::Renderer const &renderer, std::string const &imagefile, int rows = 1, int cols = 1, int initial_step = 0, int loop_to = -1, int step_divisor = -1)
        : _rows{rows}, _cols{cols}, _loop_from{initial_step}, _loop_to{loop_to}, _step_divisor{step_divisor}
    {
        if (_loop_to == -1)
            _loop_to = initial_step;
        if (step_divisor == -1)
            _step_divisor = 200 / (_loop_to - _loop_from + 1);
        sdl::Surface surface(imagefile.c_str());
        _dimensions = surface.Dimensions();
        _dimensions.h /= rows;
        _dimensions.w /= cols;
        _texture = std::make_unique<sdl::Texture>(renderer, surface);
    }

    void setupCharacter(Character &character)
    {
        character._render = [&](sdl::Renderer *renderer, std::function<SDL_Rect(SDL_Rect)> translator) {
            auto destination = translator(character._position);
            auto cell = (_current_step % (_loop_to - _loop_from + 1)) + _loop_from;
            auto from = SDL_Rect{_dimensions.w * (cell % _cols), _dimensions.h * (cell / _cols),
                                 _dimensions.w, _dimensions.h};
            renderer->Copy(*_texture, &from, &destination);
        };
        character._update += [&](Scene *scene) {
            _current_step = scene->age() / _step_divisor;
            return true;
        };
    }

private:
    std::unique_ptr<sdl::Texture> _texture;
    SDL_Rect _dimensions;
    const int _rows;
    const int _cols;
    int _current_step{0}, _loop_from, _loop_to, _step_divisor;
};

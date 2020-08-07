#pragma once
#include <string>
#include <memory>
#include <iostream>
#include "character.h"

template <typename TRenderer>
struct Sprite
{
    Sprite(TRenderer const &renderer, std::string const &imagefile, int rows = 1, int cols = 1, int initial_step = 0, int loop_to = -1, int step_divisor = -1)
        : _rows{rows}, _cols{cols}, _loop_from{initial_step}, _loop_to{loop_to}, _step_divisor{step_divisor}
    {
        if (_loop_to == -1)
            _loop_to = initial_step;
        if (step_divisor == -1)
            _step_divisor = 200 / (_loop_to - _loop_from + 1);
        _texture = renderer.CreateTexture(imagefile, &_dimensions.w, &_dimensions.h);
        _dimensions.h /= rows;
        _dimensions.w /= cols;
    }

    template <typename TCharacter>
    void setupCharacter(TCharacter &character)
    {
        character._render = [&](TRenderer *renderer, std::function<typename TRenderer::RectType(typename TRenderer::RectType)> translator) {
            auto destination = translator(character._position);
            auto cell = (_current_step % (_loop_to - _loop_from + 1)) + _loop_from;
            auto from = SDL_Rect{_dimensions.w * (cell % _cols), _dimensions.h * (cell / _cols),
                                 _dimensions.w, _dimensions.h};
            renderer->Copy(*_texture, &from, &destination);
        };
        character.addUpdate([&](typename TCharacter::SceneType *scene) {
            _current_step = scene->age() / _step_divisor;
            return true;
        });
    }

private:
    std::shared_ptr<typename TRenderer::TextureType> _texture;
    SDL_Rect _dimensions;
    const int _rows;
    const int _cols;
    int _current_step{0}, _loop_from, _loop_to, _step_divisor;
};

#pragma once
#include <string>
#include <memory>
#include <iostream>
#include <map>

// TODO: create a mechanism by which we can specify different move animations possibly with names, in order to be used by disconnected components
// for example: "jumping" "running" -they would be invoked by actions or updates of any kind

template <typename TRenderer>
struct Sprite
{
    Sprite(TRenderer const &renderer, std::string const &imagefile, int rows = 1, int cols = 1) // sprite sheet?? want to undestand
        : _rows{rows}, _cols{cols}
    {
        _texture = renderer.CreateTexture(imagefile, &_dimensions.w, &_dimensions.h);
        _dimensions.h /= rows;
        _dimensions.w /= cols;
    }

    void addAnimation(std::string const &name = "default", int from =0, int to=-1, int step_division=-1, SDL_RendererFlip flip = SDL_FLIP_NONE) 
    {
        if (to == -1)
            to = from;
        if (step_division == -1)
            step_division = 200 / (to - from + 1);
        _animations[name] = Animation{from, to, step_division, flip};
        chooseAnimation(name);
    }

    void chooseAnimation(std::string const &name) 
    {
        _current_animation = &_animations.at(name);
    }

    template <typename TCharacter>
    void setupCharacter(TCharacter &character)
    {
        character._render = [&](TRenderer *renderer, std::function<typename TRenderer::RectType(typename TRenderer::RectType)> translator) { // lamda expression?
            if (_current_animation) {
                auto destination = translator(character._position);
                auto cell = (_current_step % (_current_animation->_loop_to -_current_animation->_loop_from + 1)) + _current_animation->_loop_from;
                typename TCharacter::RendererType::RectType
                    from{_dimensions.w * (cell % _cols), _dimensions.h * (cell / _cols),
                        _dimensions.w, _dimensions.h};
                renderer->CopyEx(*_texture, &from, &destination, 0, nullptr, _current_animation->_flip);
            }
        };
        character.addUpdate([&](typename TCharacter::SceneType *scene) {
            if (_current_animation)
                _current_step = scene->age() / _current_animation->_step_divisor;
            return true;
        });
    }

private:
    std::shared_ptr<typename TRenderer::TextureType> _texture;
    typename TRenderer::RectType _dimensions;
    const int _rows;
    const int _cols;
    struct Animation {
        int _loop_from, _loop_to, _step_divisor;
        SDL_RendererFlip _flip;
    };
    std::map<std::string,Animation> _animations;
    int _current_step{0};
    const Animation *_current_animation {nullptr};
};

#pragma once
#include <functional>
#include <memory>
#include "sdladapters.h"
#include "character.h"

// A Plane is one positioning line along the x,y axis
struct Plane : public std::list<std::unique_ptr<Character>>
{
    Plane(int w, int h) : _w(w), _h(h) {}

    void background(sdl::Color const &color)
    {
        _background = [color, this](sdl::Renderer *renderer, std::function<SDL_Rect(SDL_Rect)> translator) {
            SDL_Rect rc{0, 0, _w, _h};
            rc = translator(rc);
            renderer->SetDrawColor(color);
            renderer->FillRect(&rc);
        };
    }

    void background(std::string const &filename)
    {
        _background = [filename, this](sdl::Renderer *renderer, std::function<SDL_Rect(SDL_Rect)> translator) {
            if (!_background_texture)
            {
                sdl::Surface surface(filename.c_str());
                _background_texture = std::make_unique<sdl::Texture>(*renderer, surface);
            }
            SDL_Rect rc{0, 0, _w, _h};
            rc = translator(rc);
            renderer->Copy(*_background_texture, nullptr, &rc);
        };
    }

    void render(sdl::Renderer *renderer, std::function<SDL_Rect(SDL_Rect)> translator) const
    {
        SDL_Rect obstacle{120, 300, 50, 50};
        // render background
        if (_background)
            _background(renderer, translator);

        obstacle.y -= 100;

        // render all characters
        for (auto &ch : *this)
        {
            if (ch->_render)
            {
                ch->_render(renderer, translator);
            }
        }
    }

    void add(std::unique_ptr<Character> &&character)
    {
        push_back(std::move(character));
    }

private:
    int _w, _h;
    std::function<void(sdl::Renderer *, std::function<SDL_Rect(SDL_Rect)>)> _background;
    std::unique_ptr<sdl::Texture> _background_texture;
};

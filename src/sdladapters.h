#pragma once
extern "C" {
#define SDL_MAIN_HANDLED

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
}
#include <stdexcept>
#include <string_view>
#include <string>
#include <functional>
#include <memory>
#include <list>
#include <iostream>
#include <map>
#include <algorithm>

namespace sdl
{
    class Error : public std::runtime_error
    {
    public:
        Error() : std::runtime_error(SDL_GetError()) { ; }
    };

    class FontError : public std::runtime_error
    {
    public:
        FontError() : std::runtime_error(TTF_GetError()) { ; }
    };

    class Color
    {
    public:
        Color(unsigned char red, unsigned char green, unsigned char blue, unsigned char opacity = SDL_ALPHA_OPAQUE)
            : _value{.r = red, .g = green, .b = blue, .a = opacity}
        {
        }
        Color(SDL_Color const &src) : _value{src} { ; }

        auto red() const { return _value.r; }
        auto green() const { return _value.g; }
        auto blue() const { return _value.g; }
        auto opacity() const { return _value.a; }

        operator SDL_Color() const { return _value; }

    private:
        SDL_Color _value;
    };

    class UsesSDL
    {
    public:
        UsesSDL()
        {
            if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
            {
                throw Error();
            }
        }
        ~UsesSDL()
        {
            SDL_Quit();
        }
    };

#pragma region Events
    class EventPump
    {
    public:
        typedef SDL_Event EventType;
        typedef std::function<bool(SDL_Event *)> HandlerType;

        void run(std::function<void()> fn, unsigned int intended_milliseconds, HandlerType handler)
        {
            SDL_Event ev;
            auto last_event{SDL_GetTicks()};
            auto available_time{intended_milliseconds};
            for (auto code = SDL_WaitEventTimeout(&ev, available_time); !code || ev.type != SDL_EventType::SDL_QUIT; code = SDL_WaitEventTimeout(&ev, available_time))
            {
                if (code)
                {
                    if (!std::any_of(_handlers.begin(), _handlers.end(), [&ev](auto const &kv) {
                            return kv.second(&ev);
                        }))
                    {
                        handler(&ev);
                    }
                }
                auto new_time = SDL_GetTicks();
                auto diff = new_time - last_event;
                if (diff < intended_milliseconds)
                {
                    available_time = intended_milliseconds - diff;
                }
                else
                {
                    fn();
                    last_event = new_time;
                    available_time = intended_milliseconds;
                }
            }
        }

        template <typename T>
        auto &operator+=(T *o)
        {
            _handlers[o] = std::bind(&T::handle_event, o, std::placeholders::_1);
            return *this;
        }

        auto &operator-=(void *o)
        {
            _handlers.erase(o);
            return *this;
        }

    private:
        std::map<void *, HandlerType> _handlers;
    };
#pragma endregion

#pragma region Surface
    class Surface
    {
    public:
        Surface(SDL_Surface *ptr) : _surface{ptr}
        {
        }
        Surface(const char *filename)
        {
            _surface = IMG_Load(filename);
            if (!_surface)
            {
                throw Error();
            }
        }
        ~Surface()
        {
            if (_surface)
            {
                SDL_FreeSurface(_surface);
            }
        }
        operator SDL_Surface *() const { return _surface; }
        auto Dimensions() const { return SDL_Rect{.w = _surface->w, .h = _surface->h}; }

    private:
        SDL_Surface *_surface;
    };
#pragma endregion

#pragma region Texture
    class Renderer;

    class Texture
    {
    public:
        Texture(SDL_Renderer *renderer, Surface const &src)
        {
            _texture = SDL_CreateTextureFromSurface(renderer, src);
            if (!_texture)
            {
                throw Error();
            }
        }
        ~Texture()
        {
            if (_texture)
            {
                ::SDL_DestroyTexture(_texture);
            }
        }
        operator SDL_Texture *() const { return _texture; }

        auto &SetColorMod(Uint8 r, Uint8 g, Uint8 b)
        {
            SDL_SetTextureColorMod(_texture, r, g, b);
            return *this;
        }

    private:
        SDL_Texture *_texture;
    };

#pragma endregion

#pragma region Font
    class UsesFont
    {
    public:
        UsesFont()
        {
            if (0 != TTF_Init())
            {
                throw FontError();
            }
        }
        ~UsesFont()
        {
            TTF_Quit();
        }
    };

    class Font
    {
    public:
        Font(const char *filename, int pointsize = 120)
        {
            _font = TTF_OpenFont(filename, pointsize);
            if (NULL == _font)
            {
                throw FontError();
            }
        }
        ~Font()
        {
            if (_font)
            {
                TTF_CloseFont(_font);
            }
        }

        auto RenderText_Solid(std::string_view const &text, Color const &color) const
        {
            return std::make_shared<Surface>(TTF_RenderUTF8_Solid(const_cast<TTF_Font *>(_font), std::string(text).c_str(), color));
        }

    private:
        TTF_Font *_font;
    };
#pragma endregion
#pragma region Window
    class Renderer;
    class Window
    {
    public:
        Window(std::string_view const &title, int width, int height, EventPump &pump)
            : _pump{pump}
        {
            _window = SDL_CreateWindow(
                std::string(title).c_str(),
                SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                width, height,
                SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
            if (!_window)
            {
                throw Error();
            }
            pump += this;
        }
        ~Window()
        {
            if (_window)
            {
                SDL_DestroyWindow(_window);
            }
            _pump -= this;
        }

        std::shared_ptr<Renderer> renderer()
        {
            if (!_renderer)
            {
                _renderer = std::make_shared<Renderer>(*this);
            }
            return _renderer;
        }

        void AssumeClosed()
        {
            _window = nullptr;
        }

        auto WindowID() const
        {
            return SDL_GetWindowID(_window);
        }

        bool handle_event(SDL_Event *ev)
        {
            if (0 == (ev->type & SDL_WINDOWEVENT) || ev->window.windowID != WindowID())
            {
                return false;
            }
            if (ev->window.event == SDL_WINDOWEVENT_CLOSE)
            {
                AssumeClosed();
                return true;
            }
            return false;
        }

        operator SDL_Window *()
        {
            return _window;
        }

    private:
        SDL_Window *_window;
        std::shared_ptr<Renderer> _renderer;
        EventPump &_pump;
    };
#pragma endregion
#pragma region Cursor
    class Cursor
    {
    public:
        Cursor(SDL_SystemCursor c)
            : _should_free{true}
        {
            _cursor = ::SDL_CreateSystemCursor(c);
            if (!_cursor)
                throw Error();
        }
        Cursor()
        {
            _cursor = ::SDL_GetCursor();
            if (!_cursor)
                throw Error();
        }
        ~Cursor()
        {
            if (_cursor && _should_free)
            {
                ::SDL_FreeCursor(_cursor);
            }
        }

        auto &Set()
        {
            ::SDL_SetCursor(_cursor);
            return *this;
        }

    private:
        SDL_Cursor *_cursor;
        bool _should_free{false};
    };
#pragma endregion
#pragma region Renderer
    class Renderer
    {
    public:
        typedef SDL_Rect RectType;
        typedef sdl::Texture TextureType;

        static auto GetTicks()
        {
            return ::SDL_GetTicks();
        }

        Renderer(SDL_Window *window)
        {
            _renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
            if (!_renderer)
            {
                throw Error();
            }
            int w, h;
            SDL_GetWindowSize(window, &w, &h);
            SDL_RenderSetLogicalSize(_renderer, w, h);
        }
        ~Renderer()
        {
            if (_renderer)
            {
                ::SDL_DestroyRenderer(_renderer);
            }
        }

        operator SDL_Renderer *() const { return _renderer; }

        auto &GetDimensions(int *w, int *h) const
        {
            ::SDL_RenderGetLogicalSize(*this, w, h);
            return *this;
        }

        auto &SetDrawColor(const Color &color)
        {
            if (0 != SDL_SetRenderDrawColor(*this, color.red(), color.green(), color.blue(), color.opacity()))
            {
                throw Error();
            }
            return *this;
        }

        auto &FillRect(const RectType *rc)
        {
            if (0 != SDL_RenderFillRect(*this, rc))
            {
                throw Error();
            }
            return *this;
        }

        auto &Clear()
        {
            SDL_RenderClear(*this);
            return *this;
        }

        auto &Clear(const Color &color)
        {
            return SetDrawColor(color).Clear();
        }

        auto &Present()
        {
            SDL_RenderPresent(*this);
            return *this;
        }

        auto &Copy(Texture const &texture, RectType const *from = nullptr, RectType const *to = nullptr)
        {
            if (0 != SDL_RenderCopy(*this, texture, from, to))
                throw Error();
            return *this;
        }

        auto &CopyEx(Texture const &texture, RectType const *from, RectType const *to,
                     double angle, SDL_Point const *center, SDL_RendererFlip flip)
        {
            if (0 != SDL_RenderCopyEx(*this, texture, from, to, angle, center, flip))
            {
                throw Error();
            }
            return *this;
        }

        auto &SetViewPort(RectType *rc)
        {
            if (0 != ::SDL_RenderSetViewport(*this, rc))
                throw Error();
            return *this;
        }

        auto &DrawLines(SDL_Point const *points, int count)
        {
            if (0 != ::SDL_RenderDrawLines(*this, points, count))
            {
                throw Error();
            }
            return *this;
        }

        auto CreateTexture(std::string const &filename, int *w = nullptr, int *h = nullptr) const
        {
            Surface surface(filename.c_str());
            auto dimensions{surface.Dimensions()};
            if (w)
                *w = dimensions.w;
            if (h)
                *h = dimensions.h;
            return std::make_shared<TextureType>(*this, surface);
        }

    private:
        SDL_Renderer *_renderer;
    };
#pragma endregion

} // namespace sdl

std::ostream &operator<<(std::ostream &out, SDL_Rect const &r)
{
    return out << "{" << r.x << "," << r.y << "," << r.w << "," << r.h << "}";
}

#pragma once
#include <functional>
#include <memory>

// A Plane is one positioning line along the x,y axis
template <typename TRenderable>
struct Plane : public std::list<std::shared_ptr<TRenderable>>
{
    typedef typename TRenderable::RendererType TRenderer;

    class ImageBackground
    {
    public:
        ImageBackground(int const &w, int const &h, std::string const &filename)
            : _w{w}, _h{h}, _filename{filename} {}

        auto operator()(TRenderer *renderer, std::function<typename TRenderer::RectType(typename TRenderer::RectType)> translator)
        {
            if (!_background_texture)
            {
                _background_texture = renderer->CreateTexture(_filename);
            }
            typename TRenderer::RectType rc{0, 0, _w, _h};
            rc = translator(rc);
            renderer->Copy(*_background_texture, nullptr, &rc);
        }

    private:
        int const &_w;
        int const &_h;
        std::string _filename;
        std::shared_ptr<typename TRenderer::TextureType> _background_texture;
    };

    Plane(int w, int h) : _w(w), _h(h) {}

    void background(std::string const &filename)
    {
        _image_background = std::make_shared<ImageBackground>(_w, _h, filename);
        _background = std::bind(&ImageBackground::operator(), _image_background, std::placeholders::_1, std::placeholders::_2);
    }

    template <typename TColor>
    void background(TColor const &color)
    {
        _background = [color, this](TRenderer *renderer, std::function<typename TRenderer::RectType(typename TRenderer::RectType)> translator) {
            typename TRenderer::RectType rc{0, 0, _w, _h};
            rc = translator(rc);
            renderer->SetDrawColor(color);
            renderer->FillRect(&rc);
        };
    }

    void render(TRenderer *renderer, std::function<typename TRenderer::RectType(typename TRenderer::RectType)> translator) const
    {
        // render background
        if (_background)
            _background(renderer, translator);

        // render all characters
        for (auto &ch : *this)
        {
            if (ch->_render)
            {
                ch->_render(renderer, translator);
            }
        }
    }

private:
    int _w, _h;
    std::shared_ptr<ImageBackground> _image_background;
    std::function<void(TRenderer *renderer, std::function<typename TRenderer::RectType(typename TRenderer::RectType)> translator)> _background;
};

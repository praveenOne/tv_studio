#pragma once
#include <functional>
#include <memory>

// A Plane is one positioning line along the x,y axis
template <typename TRenderable>
struct Plane : public std::list<std::shared_ptr<TRenderable>> // inherit from list of TRenderable shared ptr
{
    typedef typename TRenderable::RendererType TRenderer;

    class ImageBackground // nested class (within class plane)
    {
    public:
        ImageBackground(int const &w, int const &h, std::string const &filename) // set image background
            : _w{w}, _h{h}, _filename{filename} {}

        auto operator()(TRenderer *renderer, std::function<typename TRenderer::RectType(typename TRenderer::RectType)> translator) // operator function
        {
            if (!_background_texture)
            {
                _background_texture = renderer->CreateTexture(_filename);
            }
            typename TRenderer::RectType rc{0, 0, _w, _h}; // creating a rectangle?
            rc = translator(rc); // assign ??
            renderer->Copy(*_background_texture, nullptr, &rc); // can't navigate?? :(
        }

    private:
        // on an expression "&" can be:
        //  1) the address-of operator as in "int x; int *px = &x;"
        //  2) the bitwise operator and as in "(3 & 2) == 2"
        // on a declaration "&" declares a reference
        //  example: "int x; int &y;" (the first is an object, the second is a reference)
        // a reference works similarly to a pointer with three exceptions:
        // 1) you use the '.' operator instead of '->'
        // 2) a reference NEEDS to be initialized (ANY class that has reference fields, NEEDS constructor)
        // 3) a reference CANNOT be changed so that it refrences a different object
        int const &_w; //plane will need two reference.  - backtracing can be done within declaration
        int const &_h;
        std::string _filename;
        std::shared_ptr<typename TRenderer::TextureType> _background_texture;
    };

    Plane(int w, int h) : _w(w), _h(h) {} // constructor? don't we have a destructor?

    void background(std::string const &filename)
    {
        _image_background = std::make_shared<ImageBackground>(_w, _h, filename); // set background
        _background = [this](auto renderer, auto translator){
                    auto &bg {*_image_background}; // bg initialized from reference of _image_background
                    bg(renderer,translator);
        };
        // std::bind(&ImageBackground::operator(), _image_background, std::placeholders::_1, std::placeholders::_2); // bind operator to
    }                   // image background ??? why??

    template <typename TColor>
    void background(TColor const &color) // background by color
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
        for (auto &ch : *this) // foreach in c#
        {
            if (ch->_render) // does this character has a render
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

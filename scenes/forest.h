#include <functional>
#include <iostream>

#include "../src/sdladapters.h"
#include "../src/scene.h"
#include "../src/camera.h"
#include "../src/sprite.h"
#include "../src/move.h"
#include "../src/units.h"
#include "../src/character.h"

typedef Character<sdl::Renderer, sdl::EventPump::EventType> CharacterType;

sdl::UsesSDL usesSdl;

class ForestScene {
public:
    static void run() {
   
    try
    {
        sdl::EventPump pump;
        sdl::Window win("Runner?", 700, 272*2, pump);
        CharacterType::SceneType scene1(960, 272*2, 5);
        scene1.at(4)->background(std::string("rsrc/sprites/backgrounds/bg.png"));
        scene1.at(3)->background(std::string("rsrc/sprites/backgrounds/far_trees.png"));
        scene1.at(2)->background(std::string("rsrc/sprites/backgrounds/mid_trees.png"));
        scene1.at(0)->background(std::string("rsrc/sprites/backgrounds/close_trees.png"));
        auto renderer{win.renderer()}; // uniform initialization systax
        Camera<CharacterType::RendererType, CharacterType::SceneType> cam1(renderer);
        cam1.scene(&scene1);

        int x = 1000;
        auto ice = std::make_shared<CharacterType>();
        auto p = ice.get();
        p->_position = {x, 300, 80, 80};
        Sprite sprite_ice(*renderer, "rsrc/ice-block.png");
        sprite_ice.setupCharacter(*p);
        scene1.at(1)->push_back(ice);

        auto cat = std::make_shared<CharacterType>();
        cat->_position = {0, 300, 69, 60};
        Sprite sprite_cat(*renderer, "rsrc/cat-tile.png", 10, 8, 46, 53, 95);
        sprite_cat.setupCharacter(*cat);
        HMove(p->_position, 0, units::Speed::MetresPerSecond(9.0), *cat);
        scene1.at(2)->push_back(cat);

        pump.run(
            [&]() {
                cam1.update();
                scene1.update();
                cam1.render();
            },
            16,
            std::bind(&CharacterType::SceneType::handle_event, &scene1, std::placeholders::_1));
    }
    catch (sdl::Error &err)
    {
        std::cerr << err.what() << std::endl;
    }

    }
};

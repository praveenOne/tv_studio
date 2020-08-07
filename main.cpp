#include <functional>
#include <iostream>
#include "sdladapters.h"
#include "scene.h"
#include "camera.h"
#include "sprite.h"
#include "move.h"
#include "units.h"

typedef Character<sdl::Renderer, sdl::EventPump::EventType> CharacterType;

int main()
{
    sdl::Window win("Runner?", 700, 400);
    sdl::EventPump pump;
    auto scene1 = std::make_shared<CharacterType::SceneType>(7000, 400, 4);
    scene1->back()->background(std::string("rsrc/IMG_6110.jpg"));
    auto renderer{win.renderer()};
    Camera<sdl::Renderer, CharacterType::SceneType> cam1(renderer);
    cam1.scene(scene1.get());

    // for (int x{100}; x < 7000; x += 50 + rand() * 7000 / RAND_MAX)
    // {
    int x = 1000;
    auto ice = std::make_shared<CharacterType>();
    auto p = ice.get();
    p->_position = {x, 300, 80, 80};
    Sprite sprite_ice(*renderer, "rsrc/ice-block.png");
    sprite_ice.setupCharacter(*p);
    scene1->at(1)->push_back(ice);
    // }
    auto cat = std::make_shared<CharacterType>();
    cat->_position = {0, 300, 69, 60};
    Sprite sprite_cat(*renderer, "rsrc/cat-tile.png", 10, 8, 46, 53, 95);
    sprite_cat.setupCharacter(*cat);
    HMove(p->_position, 0, units::Speed::MetresPerSecond(9.0), *cat);
    scene1->at(2)->push_back(std::move(cat));

    pump.run(
        [&]() {
            cam1.update();
            scene1->update();
            cam1.render();
        },
        16,
        std::bind(&CharacterType::SceneType::handle_event, scene1, std::placeholders::_1));

    return 0;
}

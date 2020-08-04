#include <functional>
#include <iostream>
#include "sdladapters.h"
#include "scene.h"
#include "camera.h"
#include "sprite.h"
#include "move.h"
#include "units.h"

int main()
{
    sdl::Window win("Runner?", 700, 400);
    sdl::EventPump pump; // event system sdl 
    auto scene1 = std::make_shared<Scene>(7000, 400, 4);
    scene1->back()->background("rsrc/IMG_6110.jpg");
    auto renderer{win.renderer()};
    Camera cam1(renderer);
    cam1.scene(scene1.get());
    
    // ice cube
    int x = 1000;
    Character *pIce = nullptr;
    Sprite sprite_ice(*renderer, "rsrc/ice-block.png"); // sprite is the skin of the character
    {
        auto ice = std::make_unique<Character>();
        pIce = &(*ice);
        ice->_position = {x, 300, 80, 80};
        sprite_ice.setupCharacter(*ice);
        // save the ice character pointer before giving it away
        scene1->at(1)->add(std::move(ice)); // plane 0 is the one is closest
    }

    // cat
    auto cat = std::make_unique<Character>();
    cat->_position = {0, 300, 69, 60};
    Sprite sprite_cat(*renderer, "rsrc/cat-tile.png", 10, 8, 46, 53, 95);
    sprite_cat.setupCharacter(*cat);
    HMove(pIce->_position, 0, units::Speed::MetresPerSecond(9.0), *cat);
    scene1->at(2)->add(std::move(cat));

    // run the game!
    pump.run(
        [&]() {
            cam1.update();
            scene1->update();
            cam1.render();
        },
        16,
        std::bind(&Scene::handle_event, scene1, std::placeholders::_1));

    return 0;


    // Home work 3
    // 1 Pros / cons of current event system vs inheritance
    // 2 Read Source code and undestand
    // 3 Find suitable character for next session
}

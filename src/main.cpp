#include "sdladapters.h"
#include "scenes/forest.h"
#include "scenes/ice.h"

int main()
{
    sdl::UsesSDL use_sdl;
    ForestScene::run();
    // IceScene::run();
    return 0;
}
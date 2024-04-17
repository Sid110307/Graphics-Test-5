#include "include/level.h"
#include "include/player.h"
#include "include/renderer.h"

int main()
{
    Level level("tests/map.txt");

    Player player(level);
    Renderer renderer;
    renderer.init();

    while (!glfwWindowShouldClose(renderer.getWindow()))
    {
        player.handleInput(renderer.getWindow());
        player.getFlagsManager().showMinimap ? renderer.renderMinimap(level, player)
                                             : renderer.renderLevel(level, player);
        glfwPollEvents();
    }

    return EXIT_SUCCESS;
}

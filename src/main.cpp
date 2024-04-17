#include "include/level.h"
#include "include/player.h"
#include "include/renderer.h"

int main()
{
    Level level(20, 20);
    level.generateLevel();

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

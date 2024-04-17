#include "include/level.h"
#include "include/player.h"
#include "include/renderer.h"

Level* level = new Level("tests/map.txt");

int main()
{
    Player player;
    Renderer renderer;
    renderer.init();

    while (!glfwWindowShouldClose(renderer.getWindow()))
    {
        player.handleInput(renderer.getWindow());
        player.getPlayerFlags().showMinimap ? renderer.renderMinimap(player) : renderer.renderLevel(player);
        glfwPollEvents();
    }

    return EXIT_SUCCESS;
}

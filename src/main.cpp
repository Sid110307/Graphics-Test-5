#define STB_IMAGE_IMPLEMENTATION

#include "include/renderer.h"
#include "include/player.h"
#include "include/level.h"

Level* level = new Level("tests/map.txt");

int main()
{
    Player player;
    Renderer renderer;

    while (!glfwWindowShouldClose(renderer.getWindow()))
    {
        player.handleInput(renderer.getWindow());
        player.getPlayerFlags().showMinimap ? renderer.renderMinimap(player) : renderer.renderLevel(player);
        glfwPollEvents();
    }

    return EXIT_SUCCESS;
}

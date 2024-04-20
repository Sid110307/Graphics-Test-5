#pragma once

#include "level.h"
#include "player.h"
#include "utils.h"

class Renderer
{
public:
    Renderer();
    ~Renderer();

    void init();
    void renderLevel(const Player &player);
    void renderMinimap(const Player &player);

    [[nodiscard]] GLFWwindow* getWindow() const;

private:
    GLFWwindow* window;

    static glm::vec3 getColor(Sector sector);
    static void setupViewport();
};

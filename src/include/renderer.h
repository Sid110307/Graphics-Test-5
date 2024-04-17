#pragma once

#include <iostream>
#include <GLFW/glfw3.h>

#include "level.h"
#include "player.h"

class Renderer
{
public:
    Renderer();
    ~Renderer();

    void init();
    void renderLevel(const Level &level, const Player &player);
    void renderMinimap(const Level &level, const Player &player);

    [[nodiscard]] GLFWwindow* getWindow() const;

private:
    GLFWwindow* window;
    static glm::vec3 getColor(SectorType type);
};

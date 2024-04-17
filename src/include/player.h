#pragma once

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "level.h"
#include "utils.h"

class Player
{
public:
    explicit Player(const Level &level);

    void handleInput(GLFWwindow* window);

    [[nodiscard]] const glm::vec2 &getPos() const;
    [[nodiscard]] const glm::vec2 &getDir() const;
    [[nodiscard]] const glm::vec2 &getPlane() const;
    [[nodiscard]] const FlagsManager &getFlagsManager() const;

private:
    void tryMove(float newX, float newY);
    void rotate(float angle);
    void handleFlags(GLFWwindow* window);
    void handleMovement(GLFWwindow* window);

    const Level &level;
    glm::vec2 position, direction, plane;
    FlagsManager flagsManager;

    bool isNoClipTriggered = true, isMinimapTriggered = true;
};

#pragma once

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "level.h"
#include "utils.h"

struct PlayerFlags
{
    bool noClip = false;
    bool showMinimap = false;
};

class Player
{
public:
    Player();
    void handleInput(GLFWwindow* window);

    [[nodiscard]] const glm::vec2 &getPos() const;
    [[nodiscard]] const glm::vec2 &getDir() const;
    [[nodiscard]] const glm::vec2 &getPlane() const;
    [[nodiscard]] const PlayerFlags &getPlayerFlags() const;

private:
    void tryMove(float newX, float newY);
    void rotate(float angle);
    void interact() const;
    void handleFlags(GLFWwindow* window);
    void handleMovement(GLFWwindow* window);

    glm::vec2 position, direction, plane;
    PlayerFlags playerFlags;
    bool isInteracting = false, isNoClipTriggered = true, isMinimapTriggered = true;
};

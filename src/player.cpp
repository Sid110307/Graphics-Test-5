#include "include/player.h"

Player::Player(const Level &level) : level(level), position(), direction(-1, 0), plane(0, 0.66f)
{
    for (size_t x = 0; x < level.size(); ++x)
        for (size_t y = 0; y < level.size(x); ++y)
            if (level.getSector(x, y).getType() == SectorType::PLAYER_START)
            {
                position = {x + 0.5f, y + 0.5f};
                level.getSector(x, y).setType(SectorType::EMPTY);

                return;
            }

    std::cerr << "Player start position not found!" << std::endl;
    exit(EXIT_FAILURE);
}

void Player::handleInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window, GLFW_TRUE);

    handleFlags(window);
    handleMovement(window);
}

const glm::vec2 &Player::getPos() const { return position; }
const glm::vec2 &Player::getDir() const { return direction; }
const glm::vec2 &Player::getPlane() const { return plane; }
const FlagsManager &Player::getFlagsManager() const { return flagsManager; }

void Player::tryMove(float newX, float newY)
{
    auto blocked = {SectorType::WALL, SectorType::DOOR};
    if (std::find(blocked.begin(), blocked.end(), level.getSector(newX, newY).getType()) == blocked.end())
    {
        position.x = newX;
        position.y = newY;
    }
}

void Player::rotate(float angle)
{
    float oldDirX = direction.x;
    direction.x = direction.x * std::cos(angle) - direction.y * std::sin(angle);
    direction.y = oldDirX * std::sin(angle) + direction.y * std::cos(angle);

    float oldPlaneX = plane.x;
    plane.x = plane.x * std::cos(angle) - plane.y * std::sin(angle);
    plane.y = oldPlaneX * std::sin(angle) + plane.y * std::cos(angle);
}

void Player::handleFlags(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS) isNoClipTriggered = false;
    if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS) isMinimapTriggered = false;

    if (glfwGetKey(window, GLFW_KEY_N) == GLFW_RELEASE && !isNoClipTriggered)
    {
        flagsManager.noClip = !flagsManager.noClip;
        isNoClipTriggered = true;
    }
    if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_RELEASE && !isMinimapTriggered)
    {
        flagsManager.showMinimap = !flagsManager.showMinimap;
        isMinimapTriggered = true;
    }
}

void Player::handleMovement(GLFWwindow* window)
{
    float moveSpeed = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS ? 0.1f : 0.05f, rotSpeed = 0.05f;
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
    {
        if (flagsManager.noClip)
        {
            position.x += direction.x * moveSpeed;
            position.y += direction.y * moveSpeed;
        } else
        {
            tryMove(position.x + direction.x * moveSpeed, position.y);
            tryMove(position.x, position.y + direction.y * moveSpeed);
        }
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
    {
        if (flagsManager.noClip)
        {
            position.x -= direction.x * moveSpeed;
            position.y -= direction.y * moveSpeed;
        } else
        {
            tryMove(position.x - direction.x * moveSpeed, position.y);
            tryMove(position.x, position.y - direction.y * moveSpeed);
        }
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) rotate(rotSpeed);
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) rotate(-rotSpeed);
}

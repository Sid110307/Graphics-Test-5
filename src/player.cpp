#include "include/player.h"

Player::Player() : position(), direction(-1, 0), plane(0, 0.66f)
{
    for (size_t x = 0; x < level->size(); ++x)
        for (size_t y = 0; y < level->size(x); ++y)
            if (level->getSector(x, y).getType() == SectorType::PLAYER_START)
            {
                position = {x + 0.5f, y + 0.5f};
                level->getSector(x, y).setType(SectorType::EMPTY);

                return;
            }

    std::cerr << "Player start position not found! Defaulting to (0, 0)." << std::endl;
}

void Player::handleInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window, GLFW_TRUE);
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && !isInteracting)
    {
        interact();
        isInteracting = true;
    } else if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE) isInteracting = false;

    handleFlags(window);
    handleMovement(window);
}

const glm::vec2 &Player::getPos() const { return position; }
const glm::vec2 &Player::getDir() const { return direction; }
const glm::vec2 &Player::getPlane() const { return plane; }
const PlayerFlags &Player::getPlayerFlags() const { return playerFlags; }

void Player::move(float newX, float newY)
{
    Sector sector = level->getSector(newX, newY);
    if (sector.getType() != SectorType::WALL || sector.getFlag(SectorFlags::IS_DOOR_OPEN) || playerFlags.noClip)
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

void Player::interact() const
{
    size_t frontX = position.x + direction.x, frontY = position.y + direction.y;
    Sector frontSector = level->getSector(frontX, frontY);

    switch (frontSector.getType())
    {
        case SectorType::DOOR:
        {
            frontSector.getFlag(SectorFlags::IS_DOOR_OPEN) ? frontSector.clearFlag(SectorFlags::IS_DOOR_OPEN)
                                                           : frontSector.setFlag(SectorFlags::IS_DOOR_OPEN);
            level->setSector(frontX, frontY, frontSector);

            break;
        }
        default:
        {
            break;
        }
    }
}

void Player::handleFlags(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS) isNoClipTriggered = false;
    if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS) isMinimapTriggered = false;

    if (glfwGetKey(window, GLFW_KEY_N) == GLFW_RELEASE && !isNoClipTriggered)
    {
        playerFlags.noClip = !playerFlags.noClip;
        isNoClipTriggered = true;
    }
    if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_RELEASE && !isMinimapTriggered)
    {
        playerFlags.showMinimap = !playerFlags.showMinimap;
        isMinimapTriggered = true;
    }
}

void Player::handleMovement(GLFWwindow* window)
{
    float moveSpeed = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS ? 0.1f : 0.05f, rotSpeed = 0.05f;
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
    {
        move(position.x + direction.x * moveSpeed, position.y);
        move(position.x, position.y + direction.y * moveSpeed);
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
    {
        move(position.x - direction.x * moveSpeed, position.y);
        move(position.x, position.y - direction.y * moveSpeed);
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) rotate(rotSpeed);
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) rotate(-rotSpeed);
}

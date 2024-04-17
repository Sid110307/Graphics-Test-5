#include "include/renderer.h"

Renderer::Renderer() : window(nullptr) {}
Renderer::~Renderer()
{
    glfwDestroyWindow(window);
    glfwTerminate();
}

void Renderer::init()
{
    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW!" << std::endl;
        exit(EXIT_FAILURE);
    }

    window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Graphics Test 5", nullptr, nullptr);
    if (!window)
    {
        std::cerr << "Failed to create window!" << std::endl;
        glfwTerminate();

        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, [](GLFWwindow*, int width, int height) { glViewport(0, 0, width, height); });
}

void Renderer::renderLevel(const Level &level, const Player &player)
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, SCREEN_WIDTH, SCREEN_HEIGHT, 0, 1, -1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glClear(GL_COLOR_BUFFER_BIT);

    for (size_t x = 0; x < static_cast<size_t>(SCREEN_WIDTH); ++x)
    {
        float cameraX = 2.0f * x / SCREEN_WIDTH - 1,
            rayDirX = player.getDir().x + player.getPlane().x * cameraX,
            rayDirY = player.getDir().y + player.getPlane().y * cameraX,
            deltaDistX = std::abs(1 / rayDirX), deltaDistY = std::abs(1 / rayDirY), sideDistX, sideDistY;
        size_t mapX = player.getPos().x, mapY = player.getPos().y, hit = 0, stepX, stepY, side;

        if (rayDirX < 0)
        {
            stepX = -1;
            sideDistX = (player.getPos().x - mapX) * deltaDistX;
        } else
        {
            stepX = 1;
            sideDistX = (mapX + 1.0f - player.getPos().x) * deltaDistX;
        }

        if (rayDirY < 0)
        {
            stepY = -1;
            sideDistY = (player.getPos().y - mapY) * deltaDistY;
        } else
        {
            stepY = 1;
            sideDistY = (mapY + 1.0f - player.getPos().y) * deltaDistY;
        }

        while (hit == 0 && mapX < level.size() && mapY < level.size(0))
        {
            if (sideDistX < sideDistY)
            {
                sideDistX += deltaDistX;
                mapX += stepX;
                side = 0;
            } else
            {
                sideDistY += deltaDistY;
                mapY += stepY;
                side = 1;
            }

            if (mapX < level.size() && mapY < level.size(0) &&
                level.getSector(mapX, mapY).getType() != SectorType::EMPTY)
                hit = 1;
        }

        if (mapX >= level.size() || mapY >= level.size(0))
        {
            glColor3f(0.0f, 0.0f, 0.0f);
            glBegin(GL_LINES);
            glVertex2f(x, 0);
            glVertex2f(x, SCREEN_HEIGHT);
            glEnd();

            continue;
        }

        float perpWallDist = side == 0 ? (mapX - player.getPos().x + (1 - stepX) / 2) / rayDirX :
                             (mapY - player.getPos().y + (1 - stepY) / 2) / rayDirY;
        int lineHeight = SCREEN_HEIGHT / perpWallDist, drawStart = std::max(0, -lineHeight / 2 + SCREEN_HEIGHT / 2),
            drawEnd = std::min(lineHeight / 2 + SCREEN_HEIGHT / 2, SCREEN_HEIGHT - 1);

        glm::vec3 color;
        switch (level.getSector(mapX, mapY).getType())
        {
            case SectorType::EMPTY:
                color = glm::vec3(1.0f, 1.0f, 1.0f);
                break;
            case SectorType::WALL:
                color = glm::vec3(1.0f, 0.0f, 0.0f);
                break;
            case SectorType::DOOR:
                color = glm::vec3(0.0f, 1.0f, 0.0f);
                break;
            default:
                color = glm::vec3(1.0f, 0.0f, 1.0f);
                break;
        }

        glColor3f(color.r, color.g, color.b);
        glBegin(GL_LINES);
        glVertex2f(x, drawStart);
        glVertex2f(x, drawEnd);
        glEnd();
    }

    glfwSwapBuffers(window);
}

void Renderer::renderMinimap(const Level &level, const Player &player)
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, SCREEN_WIDTH, SCREEN_HEIGHT, 0, 1, -1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glClear(GL_COLOR_BUFFER_BIT);

    for (size_t x = 0; x < level.size(); ++x)
        for (size_t y = 0; y < level.size(0); ++y)
        {
            glm::vec3 color;
            switch (level.getSector(x, y).getType())
            {
                case SectorType::EMPTY:
                    color = glm::vec3(1.0f, 1.0f, 1.0f);
                    break;
                case SectorType::WALL:
                    color = glm::vec3(1.0f, 0.0f, 0.0f);
                    break;
                case SectorType::DOOR:
                    color = glm::vec3(0.0f, 1.0f, 0.0f);
                    break;
                default:
                    color = glm::vec3(1.0f, 0.0f, 1.0f);
                    break;
            }

            glColor3f(color.r, color.g, color.b);
            glBegin(GL_QUADS);
            glVertex2f(x * MAP_UNIT, SCREEN_HEIGHT - y * MAP_UNIT);
            glVertex2f((x + 1) * MAP_UNIT, SCREEN_HEIGHT - y * MAP_UNIT);
            glVertex2f((x + 1) * MAP_UNIT, SCREEN_HEIGHT - (y + 1) * MAP_UNIT);
            glVertex2f(x * MAP_UNIT, SCREEN_HEIGHT - (y + 1) * MAP_UNIT);
            glEnd();
        }

    glColor3f(0.0f, 0.0f, 1.0f);
    glBegin(GL_LINES);
    glVertex2f(player.getPos().x * MAP_UNIT, SCREEN_HEIGHT - player.getPos().y * MAP_UNIT);
    glVertex2f((player.getPos().x + player.getDir().x) * MAP_UNIT,
               SCREEN_HEIGHT - (player.getPos().y + player.getDir().y) * MAP_UNIT);
    glVertex2f((player.getPos().x + player.getDir().x) * MAP_UNIT,
               SCREEN_HEIGHT - (player.getPos().y + player.getDir().y) * MAP_UNIT);
    glVertex2f((player.getPos().x + player.getDir().x * 0.8f + player.getPlane().x * 0.2f) * MAP_UNIT,
               SCREEN_HEIGHT - (player.getPos().y + player.getDir().y * 0.8f + player.getPlane().y * 0.2f) * MAP_UNIT);
    glVertex2f((player.getPos().x + player.getDir().x) * MAP_UNIT,
               SCREEN_HEIGHT - (player.getPos().y + player.getDir().y) * MAP_UNIT);
    glVertex2f((player.getPos().x + player.getDir().x * 0.8f - player.getPlane().x * 0.2f) * MAP_UNIT,
               SCREEN_HEIGHT - (player.getPos().y + player.getDir().y * 0.8f - player.getPlane().y * 0.2f) * MAP_UNIT);
    glEnd();

    glfwSwapBuffers(window);
}

GLFWwindow* Renderer::getWindow() const { return window; }

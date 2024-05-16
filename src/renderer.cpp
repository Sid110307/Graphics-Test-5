#include "include/renderer.h"

Renderer::Renderer() : window(nullptr), zBuffer(new float[SCREEN_WIDTH])
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
    for (int i = 0; i < SCREEN_WIDTH; ++i) zBuffer[i] = std::numeric_limits<float>::max();

    for (int i = 0; i < static_cast<int>(ObjectType::OBJECT_TYPE_COUNT); ++i)
    {
        std::string filename = "lib/" + std::to_string(i) + ".png";
        textures[i] = stbi_load(filename.c_str(), &TEX_WIDTH, &TEX_HEIGHT, nullptr, 3);
        if (!textures[i])
        {
            std::cerr << "Failed to load texture: " << filename << std::endl;
            exit(EXIT_FAILURE);
        }
    }
}

Renderer::~Renderer()
{
    glfwDestroyWindow(window);
    glfwTerminate();
}

void Renderer::renderLevel(const Player &player)
{
    setupViewport();

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

        while (hit == 0 && mapX < level->size() && mapY < level->size(0))
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

            if (mapX < level->size() && mapY < level->size(0) &&
                level->getSector(mapX, mapY).getType() != SectorType::EMPTY)
                hit = 1;
        }

        if (mapX >= level->size() || mapY >= level->size(0))
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

        glm::vec3 color = getColor(level->getSector(mapX, mapY).getType());

        glColor3f(color.r, color.g, color.b);
        glBegin(GL_LINES);
        glVertex2f(x, drawStart);
        glVertex2f(x, drawEnd);
        glEnd();
    }

    for (size_t i = 0; i < level->getObjects().size(); ++i)
    {
        const Object &object = level->getObject(i);
        float spriteX = object.getX() - player.getPos().x, spriteY = object.getY() - player.getPos().y,
            invDet = 1.0f / (player.getPlane().x * player.getDir().y - player.getDir().x * player.getPlane().y),
            transformX = invDet * (player.getDir().y * spriteX - player.getDir().x * spriteY),
            transformY = invDet * (-player.getPlane().y * spriteX + player.getPlane().x * spriteY),
            spriteScreenX = static_cast<int>((SCREEN_WIDTH / 2) * (1 + transformX / transformY)),
            spriteHeight = std::abs(static_cast<int>(SCREEN_HEIGHT / transformY)),
            drawStartY = std::max(0.0f, -spriteHeight / 2 + SCREEN_HEIGHT / 2),
            drawEndY = std::min(SCREEN_HEIGHT - 1.0f, spriteHeight / 2 + SCREEN_HEIGHT / 2),
            drawStartX = std::max(0.0f, -spriteHeight / 2 + spriteScreenX),
            drawEndX = std::min(SCREEN_WIDTH - 1.0f, spriteHeight / 2 + spriteScreenX);

        for (int stripe = drawStartX; stripe < drawEndX; ++stripe)
            if (transformY > 0 && stripe > 0 && stripe < SCREEN_WIDTH && transformY < zBuffer[stripe])
            {
                for (int y = drawStartY; y < drawEndY; ++y)
                {
                    int d = y * 256 - SCREEN_HEIGHT * 128 + spriteHeight * 128,
                        texX = (stripe - drawStartX) * TEX_WIDTH / (drawEndX - drawStartX),
                        texY = ((d * TEX_HEIGHT) / spriteHeight) / 256;

                    if (texX >= 0 && texX < TEX_WIDTH && texY >= 0 && texY < TEX_HEIGHT)
                    {
                        unsigned char color[3] = {
                            textures[static_cast<int>(object.getType())][(texY * TEX_WIDTH + texX) * 3],
                            textures[static_cast<int>(object.getType())][(texY * TEX_WIDTH + texX) * 3 + 1],
                            textures[static_cast<int>(object.getType())][(texY * TEX_WIDTH + texX) * 3 + 2]
                        };

                        if (color[0] != 0 || color[1] != 0 || color[2] != 0)
                        {
                            glColor3f(color[0] / 255.0f, color[1] / 255.0f, color[2] / 255.0f);
                            glBegin(GL_POINTS);
                            glVertex2f(stripe, y);
                            glEnd();
                        }
                    }
                }

                zBuffer[stripe] = transformY;
            }
    }

    glfwSwapBuffers(window);
}

void Renderer::renderMinimap(const Player &player)
{
    setupViewport();
    float minimapPosX = (SCREEN_WIDTH - level->size() * MAP_UNIT) / 2, minimapPosY = 0;
    for (size_t x = 0; x < level->size(); ++x)
        for (size_t y = 0; y < level->size(x); ++y)
        {
            glm::vec3 color = getColor(level->getSector(x, y).getType());

            glColor3f(color.r, color.g, color.b);
            glBegin(GL_QUADS);
            glVertex2f(minimapPosX + x * MAP_UNIT, minimapPosY + SCREEN_HEIGHT - y * MAP_UNIT);
            glVertex2f(minimapPosX + (x + 1) * MAP_UNIT, minimapPosY + SCREEN_HEIGHT - y * MAP_UNIT);
            glVertex2f(minimapPosX + (x + 1) * MAP_UNIT, minimapPosY + SCREEN_HEIGHT - (y + 1) * MAP_UNIT);
            glVertex2f(minimapPosX + x * MAP_UNIT, minimapPosY + SCREEN_HEIGHT - (y + 1) * MAP_UNIT);
            glEnd();
        }

    for (const Object &object: level->getObjects())
    {
        glm::vec3 color = getColor(object.getType());

        glColor3f(color.r, color.g, color.b);
        glBegin(GL_QUADS);
        glVertex2f(minimapPosX + object.getX() * MAP_UNIT, minimapPosY + SCREEN_HEIGHT - object.getY() * MAP_UNIT);
        glVertex2f(minimapPosX + (object.getX() + 1) * MAP_UNIT,
                   minimapPosY + SCREEN_HEIGHT - object.getY() * MAP_UNIT);
        glVertex2f(minimapPosX + (object.getX() + 1) * MAP_UNIT,
                   minimapPosY + SCREEN_HEIGHT - (object.getY() + 1) * MAP_UNIT);
        glVertex2f(minimapPosX + object.getX() * MAP_UNIT,
                   minimapPosY + SCREEN_HEIGHT - (object.getY() + 1) * MAP_UNIT);
        glEnd();
    }

    glColor3f(0.0f, 0.0f, 1.0f);
    glBegin(GL_LINES);
    glVertex2f(minimapPosX + player.getPos().x * MAP_UNIT, minimapPosY + SCREEN_HEIGHT - player.getPos().y * MAP_UNIT);
    glVertex2f(minimapPosX + (player.getPos().x + player.getDir().x) * MAP_UNIT,
               minimapPosY + SCREEN_HEIGHT - (player.getPos().y + player.getDir().y) * MAP_UNIT);
    glVertex2f(minimapPosX + (player.getPos().x + player.getDir().x) * MAP_UNIT,
               minimapPosY + SCREEN_HEIGHT - (player.getPos().y + player.getDir().y) * MAP_UNIT);
    glVertex2f(minimapPosX + (player.getPos().x + player.getDir().x * 0.8f + player.getPlane().x * 0.2f) * MAP_UNIT,
               minimapPosY + SCREEN_HEIGHT -
               (player.getPos().y + player.getDir().y * 0.8f + player.getPlane().y * 0.2f) * MAP_UNIT);
    glVertex2f(minimapPosX + (player.getPos().x + player.getDir().x) * MAP_UNIT,
               minimapPosY + SCREEN_HEIGHT - (player.getPos().y + player.getDir().y) * MAP_UNIT);
    glVertex2f(minimapPosX + (player.getPos().x + player.getDir().x * 0.8f - player.getPlane().x * 0.2f) * MAP_UNIT,
               minimapPosY + SCREEN_HEIGHT -
               (player.getPos().y + player.getDir().y * 0.8f - player.getPlane().y * 0.2f) * MAP_UNIT);
    glEnd();

    glfwSwapBuffers(window);
}

GLFWwindow* Renderer::getWindow() const { return window; }

glm::vec3 Renderer::getColor(SectorType sectorType)
{
    switch (sectorType)
    {
        case SectorType::EMPTY:
        case SectorType::PLAYER_START:
            return {0.0f, 0.0f, 0.0f};
        case SectorType::WALL:
            return {1.0f, 0.0f, 0.0f};
        case SectorType::DOOR:
            return {0.0f, 1.0f, 0.0f};
        default:
            return {1.0f, 0.0f, 1.0f};
    }
}

glm::vec3 Renderer::getColor(ObjectType object)
{
    switch (object)
    {
        case ObjectType::KEY1:
            return {0.5f, 0.5f, 0.0f};
        case ObjectType::KEY2:
            return {0.0f, 0.5f, 0.5f};
        case ObjectType::KEY3:
            return {0.5f, 0.0f, 0.5f};
        case ObjectType::KEY4:
            return {0.5f, 0.5f, 0.5f};
        default:
            return {1.0f, 0.0f, 1.0f};
    }
}

void Renderer::setupViewport()
{
    glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, SCREEN_WIDTH, SCREEN_HEIGHT, 0, 1, -1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

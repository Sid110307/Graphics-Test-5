#pragma once

#include <stb/stb_image.h>

#include "level.h"
#include "player.h"
#include "utils.h"

class Renderer
{
public:
    Renderer();
    ~Renderer();

    void renderLevel(const Player &player);
    void renderMinimap(const Player &player);

    [[nodiscard]] GLFWwindow* getWindow() const;

private:
    GLFWwindow* window;
    float* zBuffer;
    unsigned char* textures[static_cast<int>(ObjectType::OBJECT_TYPE_COUNT)] = {nullptr};

    static glm::vec3 getColor(SectorType sectorType);
    static glm::vec3 getColor(ObjectType type);

    static void setupViewport();
};

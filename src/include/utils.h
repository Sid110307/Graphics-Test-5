#pragma once

struct FlagsManager
{
    bool noClip = false;
    bool showMinimap = false;
};

static int SCREEN_WIDTH = 800, SCREEN_HEIGHT = 600;
static float MAP_UNIT = SCREEN_WIDTH > SCREEN_HEIGHT ? SCREEN_HEIGHT / 20.0f : SCREEN_WIDTH / 20.0f;

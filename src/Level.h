#pragma once

#include "Player.h"

struct LevelData {
    char* name;

    EntityBase* props;
    int numProps = 0;
};

namespace Level {
    LevelData *loadLevel(char* filename);
}
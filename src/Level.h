#pragma once

#include "Texture.h"
#include "Vector.h"
#include "Quaternion.h"

#include "Utils.h"
#include "EntityManager.h"

struct Building {
    vec3 position;
    quat orientation;
    vec3 scale = {1, 1, 1};
};

struct LevelData {
    char* name = NULL;

    //Buildings
    int numBuildings;
    Building* buildings = NULL;

    //Ground
    Texture groundHeightmap;
};

namespace Level {
    LevelData *loadLevel(char* filename);
}
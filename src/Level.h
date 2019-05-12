#pragma once

#include "Texture.h"
#include "Vector.h"
#include "Quaternion.h"

#include "Utils.h"
#include "EntityManager.h"

#include <string>

struct PanelEnt;

struct Level {
    std::string name = "NO LEVEL LOADED";

    PanelEnt** panels = NULL;
    int numPanels = 0;
};

namespace LevelLoader {
    extern Level currentLevel;

    Level* loadLevel(char* filename);
    PanelEnt** loadFromFile(char* filename, int* retNumPanels, char*& target);
	vec3* loadPathFile(char* filename, int numNodes = NULL);
}
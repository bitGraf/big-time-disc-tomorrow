#pragma once

#include "Texture.h"
#include "Vector.h"
#include "Quaternion.h"

#include "Utils.h"
#include "EntityManager.h"

#include <string>

struct PanelEnt;
struct AIEnt;

struct Level {
    std::string name = "NO LEVEL LOADED";

    PanelEnt** panels = NULL;
    AIEnt** AIs = NULL;
    int numPanels = 0;
    int numAI = 0;
};

namespace LevelLoader {
    extern Level currentLevel;

    Level* loadLevel(char* filename, char* AIfilename = NULL);
    PanelEnt** loadFromFile(char* filename, int* retNumPanels, char*& target);
    AIEnt** loadLevelAI(char* filename, int* retNumAI);
    vec3* loadPathFile(char* filename);
}
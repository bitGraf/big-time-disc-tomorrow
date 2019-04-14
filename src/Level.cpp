#include "Level.h"

LevelData* Level::loadLevel(char* filename) {
    LevelData *level = (LevelData*)malloc(sizeof(LevelData));

    level->numBuildings = 5;
    level->buildings = (Building*)malloc(level->numBuildings*sizeof(Building));
    for (int i = 0; i < level->numBuildings; i++) {
        level->buildings[i].position = {
            randomFloat(-5, 5),
            randomFloat(-5, 5),
            randomFloat(-5, 5)};
        
        //level->buildings[i].orientation;
        Quaternion::buildFromAxisAngleD(level->buildings[i].orientation, 
            {0, 1, 0}, randomFloat(0, 360));
        
        level->buildings[i].scale = {
            randomFloat(0.3, 10),
            randomFloat(0.3, 10),
            randomFloat(0.3, 10)};
    }

    level->groundHeightmap.loadImage("sample.jpg");

    level->name = "Test Level";

    return NULL;
}
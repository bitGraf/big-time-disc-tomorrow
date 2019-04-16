#include "Level.h"

LevelData* Level::loadLevel(char* filename) {
    printf("Loading level: %s\n", filename);
    LevelData *level = (LevelData*)malloc(sizeof(LevelData));

    level->numBuildings = 25;
    level->buildings = (Building*)malloc(level->numBuildings*sizeof(Building));
    for (int i = 0; i < level->numBuildings; i++) {
        float D = 25;
        level->buildings[i].position = {
            randomFloat(-D, D),
            randomFloat(-2, 2),
            randomFloat(-D, D)};
        
        //level->buildings[i].orientation;
        Quaternion::buildFromAxisAngleD(level->buildings[i].orientation, 
            {0, 1, 0}, randomFloat(0, 360));
        
        level->buildings[i].scale = {
            randomFloat(0.3, 2),
            randomFloat(0.7, 1.4),
            randomFloat(0.3, 2)};

        int id = 0;
        Entity::loadEntityFromFile("box", &id);
        EntityBase* ent = Entity::lookup_entity_by_id(id);

        ent->position = level->buildings[i].position;
        ent->orientation = level->buildings[i].orientation;
        ent->scale = level->buildings[i].scale;
    }

    level->groundHeightmap.loadImage("sample.jpg");

    level->name = "Test Level";

    return level;
}
#include "Level.h"

LevelData* Level::loadLevel(char* filename) {
    printf("Loading level: %s\n", filename);
    LevelData *level = (LevelData*)malloc(sizeof(LevelData));

    level->numBuildings = 25;
    level->buildings = (Building*)malloc(level->numBuildings*sizeof(Building));
    for (int i = 0; i < level->numBuildings; i++) {
        float D = 100;

        level->buildings[i].scale = {
            randomFloat(5, 8),
            randomFloat(1, 15),
            randomFloat(5, 8)};

        level->buildings[i].position = {
            randomFloat(-D, D),
            -1 + level->buildings[i].scale.y,
            randomFloat(-D, D)};
        
        //level->buildings[i].orientation;
        Quaternion::buildFromAxisAngleD(level->buildings[i].orientation, 
            {0, 1, 0}, randomFloat(0, 360));

        int id = 0;
        EntityBase* ent = Entity::createNewEntity(ENT_Static);

        ent->mesh = Resources::manager.getTriMeshResource("cube");
        ent->baseColor = Resources::manager.getTextureResource("wall");

        ent->position = level->buildings[i].position;
        ent->orientation = level->buildings[i].orientation;
        ent->scale = level->buildings[i].scale;
    }

    //level->groundHeightmap.loadImage("sample.jpg");
    Resources::manager.loadTerrainResource("smallMap", ".png");
    EntityBase* terr = Entity::createNewEntity(ENT_Static);
    terr->mesh = Resources::manager.getTriMeshResource("smallMap");
    terr->Remove = true;

    Resources::manager.printAllResources();
    //TerrainResource* ptr = Resources::manager.getTerrainResource("smallMap");
    //terr->mesh->data = Resources::manager.getTerrainResource("smallMap")->mesh;

    level->name = "Test Level";

    return level;
}
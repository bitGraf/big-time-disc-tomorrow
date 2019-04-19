#include "Level.h"

LevelData* Level::loadLevel(char* filename, char* filetype, char* texturename, char* texturetype) {
    printf("Loading level: %s\n", filename);
    LevelData *level = (LevelData*)malloc(sizeof(LevelData));

    // Load terrain from heightmap
    auto data = Resources::manager.loadTerrainResource(filename, filetype);
    EntityBase* terr = Entity::createNewEntity(ENT_Terrain);
    terr->mesh = Resources::manager.getTriMeshResource(filename);
    Resources::manager.loadTextureResource(texturename, texturetype);
    terr->baseColor = Resources::manager.getTextureResource(texturename);
    terr->Color = {1,1,1};

    level->numBuildings = 50;
    level->buildings = (Building*)malloc(level->numBuildings*sizeof(Building));
    for (int i = 0; i < level->numBuildings; i++) {
        float D = 100;

        level->buildings[i].scale = {
            randomFloat(.3, .6),
            randomFloat(.7, 2),
            randomFloat(.3, .6)};

        float xPos = randomFloat(0, D);
        float zPos = randomFloat(0, D);

        int x = (int)(xPos * 5.12f);
        int z = (int)(zPos * 5.12f);
        float height = 10 * Resources::manager.getHeight(data, x, z, 1, 512);

        printf("world pos: (%f,%f) | terrain index: (%d,%d) | height: %f\n", xPos, zPos, x, z, height);

        level->buildings[i].position = {
            xPos,
            level->buildings[i].scale.y + height,
            zPos};
        
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

    level->name = "Test Level";

    return level;
}
#include "Level.h"

void** Level::loadFromFile(char* filename, int* retNumPanels) {
    //Panel resources
    Resources::manager.loadTriMeshResource("plane", ".modl");
    Resources::manager.loadTextureResource("wall", ".jpg");

    //Read whole file into array
    FILE* modelFile = fopen(filename, "rb");
    if (modelFile == NULL) {
        printf("Failed to open file [%s]\n", filename);
        return NULL;
    }
    printf("Reading file: \"%s\"\n", filename);
    
    fseek(modelFile, 0, SEEK_END);
	long fileLength = ftell(modelFile);
	fseek(modelFile, 0, SEEK_SET);

	char *fileContents = (char *)malloc(fileLength + 1);
	fread(fileContents, fileLength, 1, modelFile);
	fclose(modelFile);
	fileContents[fileLength] = 0;

    //start parsing line by line
    char* lineContents;
    lineContents = strtok(fileContents, "\r\n");
    lineContents = strtok(NULL, "\r\n");

    int numPanels = strtod(lineContents + 10, NULL);
    printf("NumPanels = %d\n", numPanels);

    if (retNumPanels != NULL)
        *retNumPanels = numPanels;
    PanelEnt** allPanels = (PanelEnt**)malloc(numPanels * sizeof(PanelEnt*));

    for (int i = 0; i < numPanels; i ++) {
        allPanels[i] = (PanelEnt*)Entity::createNewEntity(ENT_Panel);
        allPanels[i]->mesh = Resources::manager.getTriMeshResource("plane");
        allPanels[i]->baseColor = Resources::manager.getTextureResource("wall");

        lineContents = strtok(NULL, "\r\n");
        char* pstr = lineContents;
        float x = strtof(pstr, &pstr);
        float y = strtof(pstr, &pstr);
        float z = strtof(pstr, &pstr);

        float tx = strtof(pstr, &pstr);
        float bx = strtof(pstr, &pstr);
        float nx = strtof(pstr, &pstr);

        float ty = strtof(pstr, &pstr);
        float by = strtof(pstr, &pstr);
        float ny = strtof(pstr, &pstr);

        float tz = strtof(pstr, &pstr);
        float bz = strtof(pstr, &pstr);
        float nz = strtof(pstr, &pstr);

        float sx = strtof(pstr, &pstr);
        float sy = strtof(pstr, &pstr);
        float sz = strtof(pstr, NULL);

        printf("\n\nLoading Panel %d\n", i);

        if (i == 3) {
            printf("%f %f %f | %f %f %f | %f %f %f | %f %f %f\n",
            x, y, z, tx, ty, tz, bx, by, bz, nx, ny, nz);
        }

        quat blenderOrientation = Quaternion::fromDCM({tx, ty, tz},{bx, by, bz},{nx, ny, nz});

        quat B2D = {-.7071f, 0, 0, .7071f};

        vec3 newAxis = Quaternion::transformVector(B2D, blenderOrientation.axis());
        quat gameOrientation;
        Quaternion::buildFromAxisAngleD(gameOrientation, newAxis, blenderOrientation.angle());

        printf("New angle: %f deg\n", blenderOrientation.angle());
        blenderOrientation.axis().print("Old rotation axis: ");
        newAxis.print("New rotation axis: ");

        vec3 T = {tx, ty, tz};
        vec3 B = {bx, by, bz};
        vec3 N = {nx, ny, nz};

        T.print("T: ");
        B.print("B: ");
        N.print("N: ");

        vec3 blenderPos = {x, y, z};
        vec3 gamePos = Quaternion::transformVector(B2D, blenderPos);

        vec3 worldScale = {sx, sy, sz};
        vec3 panelScale = Quaternion::transformVector(Quaternion::inverse(gameOrientation), worldScale);
        worldScale.print("Scale: ");

        B2D.print("Blender to Game: ");
        blenderOrientation.print("Blender orientation: ");
        gameOrientation.print   ("Game orientation:    ");
        blenderPos.print("Blender position: ");
        gamePos.print   ("Game position:    ");

        //allPanels[i]->orientation = Quaternion::mul(ret, x90);
        allPanels[i]->orientation = gameOrientation;
        allPanels[i]->orientation.print();
        allPanels[i]->position = gamePos;
        allPanels[i]->length = 2*worldScale.x;
        allPanels[i]->width = 2*worldScale.z;
        allPanels[i]->scale = {allPanels[i]->length, 1, allPanels[i]->width};

        allPanels[i]->update(0);
    }

    free(fileContents);

    return (void**)allPanels;

    return NULL;
}

/*
void** Level::loadFromFile(char* filename, int* retNumPanels) {
    //Panel resources
    Resources::manager.loadTriMeshResource("plane", ".modl");
    Resources::manager.loadTextureResource("wall", ".jpg");

    int numPanels = 5;
    if (retNumPanels != NULL)
        *retNumPanels = numPanels;
    PanelEnt** allPanels = (PanelEnt**)malloc(numPanels * sizeof(PanelEnt*));

    for (int i = 0; i < numPanels; i ++) {
        allPanels[i] = (PanelEnt*)Entity::createNewEntity(ENT_Panel);
        allPanels[i]->mesh = Resources::manager.getTriMeshResource("plane");
        allPanels[i]->baseColor = Resources::manager.getTextureResource("wall");
        allPanels[i]->update(0);
        allPanels[i]->scale = {allPanels[i]->length, 1, allPanels[i]->width};
    }

    allPanels[0]->position = {-7, 5, 0};
    Quaternion::buildFromAxisAngleD(allPanels[0]->orientation, {0, 0, 1}, -90);

    allPanels[1]->position = {0, 5, -7};
    Quaternion::buildFromAxisAngleD(allPanels[1]->orientation, {1, 0, 0}, 90);
    allPanels[1]->length = 14;  allPanels[1]->width = 10;
    allPanels[1]->scale = {allPanels[1]->length, 1, allPanels[1]->width};

    allPanels[2]->position = {-1.4645f - 2.0f + 7.0710678f, 13.536f + 7.0710678f, 0.0f};
    Quaternion::buildFromAxisAngleD(allPanels[2]->orientation, {0, 0, 1}, -135);
    allPanels[2]->length = 30;
    allPanels[2]->scale = {allPanels[2]->length, 1, allPanels[2]->width};

    allPanels[3]->position = {-1.4645f + 6.0f + 7.0710678f, 13.536f + 7.0710678f + 8.0f, 14.0f};
    Quaternion::buildFromAxisAngleD(allPanels[3]->orientation, {0, 0, 1}, -160);
    allPanels[3]->length = 30;
    allPanels[3]->scale = {allPanels[3]->length, 1, allPanels[3]->width};

    allPanels[4]->position = {0, 2, 10};
    Quaternion::buildFromAxisAngleD(allPanels[4]->orientation, {1, 0, 0}, -30);

    return (void**)allPanels;
}
*/

LevelData* Level::loadLevel(char* filename, char* filetype, char* texturename, char* texturetype) {
    printf("Loading level: %s\n", filename);
    LevelData *level = (LevelData*)malloc(sizeof(LevelData));

    // Load terrain from heightmap
    TerrainEnt* terr = (TerrainEnt*)Entity::createNewEntity(ENT_Terrain);

    terr->terrainInfo.length = 100;
    terr->terrainInfo.width  = 100;
    terr->terrainInfo.height = 10;
    terr->terrainInfo.N = 513;
    terr->terrainInfo.M = 513;
    terr->terrainInfo.originX = -50;    //more like offset from the origin
    terr->terrainInfo.originZ = -50;
    terr->terrainInfo.originY = -10;

    terr->terrainInfo = Resources::manager.loadTerrainResource(filename, filetype, terr->terrainInfo);
    terr->mesh = Resources::manager.getTriMeshResource(filename);
    Resources::manager.loadTextureResource(texturename, texturetype);
    terr->baseColor = Resources::manager.getTextureResource(texturename);
    terr->Color = {1,1,1};

    level->numBuildings = 50;
    level->buildings = (Building*)malloc(level->numBuildings*sizeof(Building));
    for (int i = 0; i < level->numBuildings; i++) {
        float D = 50;

        level->buildings[i].scale = {
            randomFloat(.3, .6),
            randomFloat(.7, 2),
            randomFloat(.3, .6)};

        float xPos = randomFloat(0, 2*D) + terr->terrainInfo.originX;
        float zPos = randomFloat(0, 2*D) + terr->terrainInfo.originZ;
        float height = Resources::manager.getHeight(terr->terrainInfo, xPos, zPos);

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
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
        vec3 v1 = {strtof(pstr, &pstr), strtof(pstr, &pstr), strtof(pstr, &pstr)};
        vec3 v2 = {strtof(pstr, &pstr), strtof(pstr, &pstr), strtof(pstr, &pstr)};
        vec3 v3 = {strtof(pstr, &pstr), strtof(pstr, &pstr), strtof(pstr, &pstr)};
        vec3 v4 = {strtof(pstr, &pstr), strtof(pstr, &pstr), strtof(pstr, NULL)};

        vec3 panelPos = (v1 + v2 + v3 + v4) * 0.25f;
        float xScale = Vector::magnitude(v3-v2);
        float zScale = Vector::magnitude(v2-v1);
        vec3 T = Vector::normalized(v3 - v2);
        vec3 B = Vector::normalized(v2 - v1);
        vec3 N = Vector::cross(B, T);
        quat panelRotation = Quaternion::fromDCM(T, N, B);

        //panelPos.print("Position: ");
        //panelRotation.print("Rotation: ");
        //printf("Scale: (%f %f)\n", xScale, zScale);

        allPanels[i]->orientation = panelRotation;
        allPanels[i]->position = panelPos;
        allPanels[i]->length = xScale;
        allPanels[i]->width = zScale;
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

vec3* Level::loadPathFile(char* filename, int numNodes) {
	//Read whole file into array
	FILE* modelFile = fopen(filename, "rb");
	if (modelFile == NULL) {
		printf("Failed to open path file [%s]\n", filename);
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
	int numCoords = strtod(lineContents + 1, NULL);
	if (numNodes <= numCoords && numNodes != NULL) {
		numCoords = numNodes;
	}
	vec3* coordList = (vec3*)malloc(numCoords * sizeof(vec3));
	//PanelEnt** allPanels = (PanelEnt**)malloc(numPanels * sizeof(PanelEnt*));
	for (int i = 0; i < numCoords; i++) {
		lineContents = strtok(NULL, "\r\n");
		if (lineContents[0] == 'P') {
			char* pstr = lineContents + 1;
			vec3 coords = { strtof(pstr, &pstr), strtof(pstr, &pstr), strtof(pstr, NULL) };
			coordList[i] = coords;
		}
		if (lineContents[0] == 'R') {
			vec3 coords =  {NULL};
			coordList[i] = coords;
		}
	}
	free(fileContents);

	return coordList;
}
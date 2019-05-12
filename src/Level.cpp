#include "Level.h"

Level LevelLoader::currentLevel;

Level* LevelLoader::loadLevel(char* filename) {
    // Do not call more than once for now...

    int numPanelsLoaded = 0;
    char* nameLoaded = NULL;
    PanelEnt** loadedPanels = loadFromFile(filename, &numPanelsLoaded, nameLoaded);

    currentLevel.panels = loadedPanels;
    currentLevel.numPanels = numPanelsLoaded;
    currentLevel.name = std::string(nameLoaded);

    free(nameLoaded);

    return &currentLevel;
}

PanelEnt** LevelLoader::loadFromFile(char* filename, int* retNumPanels, char*& target) {
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

    // Set target to the level name
    target = (char*)malloc(strlen(lineContents)+1);
    strcpy(target, lineContents);

    lineContents = strtok(NULL, "\r\n");
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

        allPanels[i]->orientation = panelRotation;
        allPanels[i]->position = panelPos;
        allPanels[i]->length = xScale;
        allPanels[i]->width = zScale;
        allPanels[i]->scale = {allPanels[i]->length, 1, allPanels[i]->width};

        allPanels[i]->update(0);
    }

    free(fileContents);

    return allPanels;
}

vec3* LevelLoader::loadPathFile(char* filename, int numNodes) {
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
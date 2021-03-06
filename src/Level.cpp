#include "Level.h"

Level LevelLoader::currentLevel;

Level* LevelLoader::loadLevel(char* filename, char*AIfilename) {
    // Do not call more than once for now...
    if (Entity::manager.Player->currentLevel) {
        //a level is already loaded

        //Remove current level.
        for (int i = 0; i < Entity::manager.Player->currentLevel->numPanels; i++) {
            Entity::manager.Player->currentLevel->panels[i]->Remove = true;
        }
        for (int i = 0; i < Entity::manager.Player->currentLevel->numAI; i++) {
            Entity::manager.Player->currentLevel->AIs[i]->Remove = true;
        }
        Entity::pruneEntities();
        Entity::manager.Player->currentLevel = NULL;
        free(currentLevel.panels);
        free(currentLevel.AIs);
    }

    if (filename) {
        int numPanelsLoaded = 0;
        int numAILoaded = 0;
        char* nameLoaded = NULL;
        PanelEnt** loadedPanels = loadFromFile(filename, &numPanelsLoaded, nameLoaded);
        AIEnt** loadedAI = loadLevelAI(AIfilename, &numAILoaded);
        currentLevel.panels = loadedPanels;
        currentLevel.numPanels = numPanelsLoaded;
        currentLevel.AIs = loadedAI;
        currentLevel.numAI = numAILoaded;
        currentLevel.name = std::string(nameLoaded);

        free(nameLoaded);

        Entity::manager.Player->currentLevel = &currentLevel;

        return &currentLevel;
    }

    currentLevel.name = "No level loaded";

    return NULL;
}

PanelEnt** LevelLoader::loadFromFile(char* filename, int* retNumPanels, char*& target) {
    //Panel resources
    Resources::manager.loadTriMeshResource("plane", ".modl");
    Resources::manager.loadTextureResource("wall", ".jpg");
    
    // Load File
    ResourceFile modelFile;
    modelFile.load(filename);

    // Set target to the level name
    char* lineContents = modelFile.getNextLine();
    target = (char*)malloc(strlen(lineContents) + 1);

    strcpy(target, lineContents);

    lineContents = modelFile.getNextLine();
    lineContents = modelFile.getNextLine();

    int numPanels = strtod(lineContents + 10, NULL);
    printf("NumPanels = %d\n", numPanels);

    if (retNumPanels != NULL)
        *retNumPanels = numPanels;
    PanelEnt** allPanels = (PanelEnt**)malloc(numPanels * sizeof(PanelEnt*));
    for (int i = 0; i < numPanels; i++) {
        allPanels[i] = (PanelEnt*)Entity::createNewEntity(ENT_Panel);
        allPanels[i]->mesh = Resources::manager.getTriMeshResource("plane");
        allPanels[i]->baseColor = Resources::manager.getTextureResource("wall");

        lineContents = modelFile.getNextLine();
        char* pstr = lineContents;
        vec3 v1 = { strtof(pstr, &pstr), strtof(pstr, &pstr), strtof(pstr, &pstr) };
        vec3 v2 = { strtof(pstr, &pstr), strtof(pstr, &pstr), strtof(pstr, &pstr) };
        vec3 v3 = { strtof(pstr, &pstr), strtof(pstr, &pstr), strtof(pstr, &pstr) };
        vec3 v4 = { strtof(pstr, &pstr), strtof(pstr, &pstr), strtof(pstr, NULL) };

        vec3 panelPos = (v1 + v2 + v3 + v4) * 0.25f;
        float xScale = Vector::magnitude(v3 - v2);
        float zScale = Vector::magnitude(v2 - v1);
        vec3 T = Vector::normalized(v3 - v2);
        vec3 B = Vector::normalized(v2 - v1);
        vec3 N = Vector::cross(B, T);
        quat panelRotation = Quaternion::fromDCM(T, N, B);

        allPanels[i]->orientation = panelRotation;
        allPanels[i]->position = panelPos;
        allPanels[i]->length = xScale;
        allPanels[i]->width = zScale;
        allPanels[i]->scale = { allPanels[i]->length, 1, allPanels[i]->width };

        allPanels[i]->update(0);
    }
    modelFile.close();

    return allPanels;
}


vec3* LevelLoader::loadPathFile(char* filename) {
    ResourceFile pathFile;
    pathFile.load(filename);
    vec3* coordList = (vec3*)malloc(pathFile.numLines * sizeof(vec3));
    for (int i = 0; i < pathFile.numLines; i++) {
        char* lineContents = pathFile.getNextLine();
        if (lineContents[0] == 'P') {
            char* pstr = lineContents + 1;
            vec3 coords = { strtof(pstr, &pstr), strtof(pstr, &pstr), strtof(pstr, NULL) };
            coordList[i] = coords;
        }
        if (lineContents[0] == 'R') {
            vec3 coords = { NULL };
            coordList[i] = coords;
        }
    }
    pathFile.close();
    return coordList;
}

AIEnt** LevelLoader::loadLevelAI(char* filename, int* retNumAI) {
    ResourceFile aiFile;
    aiFile.load(filename);
    if (retNumAI != NULL) {
        *retNumAI = aiFile.numLines / 2;
    }
    AIEnt** allAI = (AIEnt**)malloc(aiFile.numLines/2 * sizeof(AIEnt*));
    for (int i = 0; i < aiFile.numLines / 2; i++) {
        char* lineContents = aiFile.getNextLine();
        char* lineCopy = (char*)malloc(1 + strlen(lineContents));
        strcpy(lineCopy, lineContents);
        char* pstr = lineCopy;

        allAI[i] = (AIEnt*)Entity::createNewEntity(ENT_AI);
        char* mesh = strtok(pstr, " ");
        char* baseColor = strtok(NULL, " ");
        char* path = strtok(NULL, " ");
        free(lineCopy);

        lineContents = aiFile.getNextLine();
        lineCopy = (char*)malloc(1 + strlen(lineContents));
        strcpy(lineCopy, lineContents);
        pstr = lineCopy;

        vec3 position = { strtof(pstr, &pstr), strtof(pstr, &pstr), strtof(pstr, NULL) };
        allAI[i]->mesh = Resources::manager.getTriMeshResource(mesh);
        allAI[i]->baseColor = Resources::manager.getTextureResource(baseColor);
        allAI[i]->position = position;

        free(lineCopy);
        lineCopy = NULL;
        pstr = NULL;
    }
    aiFile.close();
    return allAI;
}

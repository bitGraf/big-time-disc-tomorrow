#include "EntityManager.h"

EntityManager Entity::manager;

void Entity::init_entities(WindowInfo windowInfo) {
    //
	// Load assets from file
	//

    ModelLoader::loadFile(&manager.axis, "../data/models/axis.modl");

    // Entities
    //Resources::manager.loadTriMeshResource("cube", ".modl");
    //Resources::manager.loadTriMeshResource("bigBot", ".ply");   //very large file lmao
    Resources::manager.loadTriMeshResource("bot", ".ply");

    Resources::manager.loadTextureResource("wall", ".jpg");
    Resources::manager.loadTextureResource("sample", ".jpg");
    Resources::manager.loadTextureResource("blank", ".png");    //just a white texture


    EntityBase* ent = Entity::createNewEntity(ENT_Crawler);
    ent->mesh = Resources::manager.getTriMeshResource("bot");
    ent->baseColor = Resources::manager.getTextureResource("wall");
    ent->Color = {5, 5, 7};

    Level::loadLevel("smallMap", ".png", "mColor", ".png");

    manager.font = Font::newDynamicFont("../data/fonts/Consolas.ttf", 20);

    Entity::printAllEntities();

    //Camera
	//manager.camera.position = {0, 5, 5};
	//manager.camera.lookAt({0, 0, -3});
	//manager.camera.updateVectors();
	//manager.camera.updateViewMatrix();
	manager.camera.updateProjectionMatrix(windowInfo);
}

EntityBase* Entity::createNewEntity(EntityTypes type, int* id) {
    int IDval = registerEntity(type);
    if (id != NULL)
        *id = IDval;

    EntityBase* newEnt = Entity::lookup_entity_by_id(IDval);

    newEnt->onCreate();

    return newEnt;
}

void Entity::handleInputEvent(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (manager.logData) {
        printf("Handling input.\n");
    }

    if ((key == GLFW_KEY_F) && (action == GLFW_PRESS)) {
        printf("toggle frame rendering...\n");

        Entity::manager.showFrames = !Entity::manager.showFrames;
	}

    for (int i = 0; i < manager.numEntries; i++) {//don't render ID 0
        EntityBase* ent = (manager.pointerList[i]);

        if (!ent->Remove)
            ent->handleInput(key, scancode, action, mods);
    }
}

void Entity::fixedUpdateAllEntities(double dt) {
    if (manager.logData) {
        printf("Updating entities.\n");
    }
    for (int i = 0; i < manager.numEntries; i++) {//don't render ID 0
        EntityBase* ent = (manager.pointerList[i]);
        
        if (!ent->Remove)
            ent->update(dt);    //For now, just ignore "Removed" entitites
    }
}

void Entity::renderAllEntities(ShaderProgram* shader) {
    if (manager.logData) {
        printf("Rendering all.\n");
        Entity::printAllEntities();
    }
    for (int i = 0; i < manager.numEntries; i++) {
        EntityBase* ent = (manager.pointerList[i]);
        if (manager.logData) {
            printf("rendering entity %d/%d\n", i, manager.numEntries);
            if (ent == NULL)
                printf("uh oh\n");
        }

        if (!ent->Remove) {
            ent->preRender();
            
            shader->use();
            shader->setMat4("view", &Entity::manager.camera.viewMatrix);
            shader->setvec3("camPos", &Entity::manager.camera.position);
            if (manager.logData) {
                if (ent == NULL)
                    printf("uh oh.\n");
                if (ent->mesh == NULL)
                    printf("mesh null\n");
                if (ent->mesh->data.VAO == 0)
                    printf("VAO = 0\n");
            }
            shader->setMat4("model", &ent->modelMatrix);
            shader->setvec3("color", &ent->Color);
            shader->setInt("baseColor", 0);
            glBindVertexArray(ent->mesh->data.VAO);
            ent->baseColor->data.bind(GL_TEXTURE0);

            glDrawElements(GL_TRIANGLES, ent->mesh->data.numFaces*3, GL_UNSIGNED_INT, 0);

            if (manager.showFrames) {
                //printf("printing frames\n");
                glDisable(GL_DEPTH_TEST);
                manager.lineShader->use();
                manager.lineShader->setMat4("model", &ent->modelMatrix);
                manager.lineShader->setMat4("view", &Entity::manager.camera.viewMatrix);
                glBindVertexArray(manager.axis.VAO);
                glDrawElements(GL_LINES, manager.axis.numFaces*2, GL_UNSIGNED_INT, 0);
                glBindVertexArray(0);
                glEnable(GL_DEPTH_TEST);
            }
        }
    }
    glBindVertexArray(0);
}

void Entity::printAllEntities() {

    printf("\nEntity Map (%d/%d entries)\n--------------------------------------------------\n",
         manager.numEntries, manager.maxSize);
    printf("Address\t\t\tID\tVAOid\tSubClass\n");
    for (int i = 0; i < manager.numEntries; i++) {
        if (manager.pointerList[i]) {
            EntityBase* ent = manager.pointerList[i];

            printf("%p\t%d\t%d\t%s\n", ent, ent->ID, ent->mesh->data.VAO,
                ent->subType==ENT_Base ? " " : (
                ent->subType==ENT_Player ? "Player" : (
                    "Static")));

            /*switch(ent->subType) {
                case ENT_Base: {
                } break;
                case ENT_Player: {
                    PlayerEnt* pEnt = (PlayerEnt*)ent;
                } break;
                case ENT_Static: {
                    StaticEnt* sEnt = (StaticEnt*)ent;
                } break;
                default: {
                } break;
            }*/
        } else {
            printf("%p\t%d\t%d\n", manager.pointerList[i],
            0, 0);
        }
    }
    printf("--------------------------------------------------\n\n");
}

int Entity::registerEntity(EntityTypes type, LoadOptions* opts) {
    //Ensure array is big enough
    if (manager.numEntries < manager.maxSize) {
        //there is still room in the array
    } else {
        //array needs to be expanded
        printf("----->Expanding array from %d elements to ", manager.maxSize);
        manager.maxSize += manager.entryGrowthAmount;
        manager.pointerList = (EntityBase**)realloc(manager.pointerList, manager.maxSize * sizeof(EntityBase*));
        printf("%d elements\n", manager.maxSize);
    }

    EntityBase* ent = NULL;

    if (opts != NULL) {
        printf("Y'all fucked up...\n");
        assert(false);
        //insert new element of correct subtype
        switch(opts->subType) {
            case ENT_Base: {
                manager.pointerList[manager.numEntries] = new EntityBase;
                printf("Adding new EntityBase\n");
            } break;
            case ENT_Player: {
                manager.pointerList[manager.numEntries] = new PlayerEnt;
                printf("Adding new EntityPlayer\n");
            } break;
            case ENT_Static: {
                manager.pointerList[manager.numEntries] = new StaticEnt;
                printf("Adding new EntityStatic\n");
            } break;
            case ENT_Temporary: {
                manager.pointerList[manager.numEntries] = new TemporaryEnt;
                printf("Adding new TemporaryEnt\n");
            } break;
            default: {
                manager.pointerList[manager.numEntries] = new EntityBase;
                printf("Adding new EntityBase\n");
            } break;
        }

        //insert into list at numEntries
        ent = manager.pointerList[manager.numEntries];
        ent->ID = manager.numEntries;

        //copy data from file
        ent->position = opts->position;
        ent->orientation = opts->orientation;
        ent->scale = opts->scale;
        //ent->modelFilePath = opts->modelFilePath;
        ent->subType = opts->subType;
        ent->Color = opts->color;

        //load mesh
        //ModelLoader::loadFile(&ent->mesh, ent->modelFilePath);

        //load texture
        //ent->baseColor.loadImage("sample.jpg");

        //manager.VAOs.push_back(ent->mesh.VAO);
        //manager.VBOs.push_back(ent->mesh.VBOpos);
        //manager.VBOs.push_back(ent->mesh.VBOnorm);
        //manager.EBOs.push_back(ent->mesh.EBO);
    } else {
        switch(type) {
            case ENT_Base: {
                manager.pointerList[manager.numEntries] = new EntityBase;
                //printf("Adding new EntityBase\n");
            } break;
            case ENT_Player: {
                manager.pointerList[manager.numEntries] = new PlayerEnt;
                //printf("Adding new EntityPlayer\n");
            } break;
            case ENT_Static: {
                manager.pointerList[manager.numEntries] = new StaticEnt;
                //printf("Adding new EntityStatic\n");
            } break;
            case ENT_Temporary: {
                manager.pointerList[manager.numEntries] = new TemporaryEnt;
                //printf("Adding new EntityStatic\n");
            } break;
            case ENT_Terrain: {
                manager.pointerList[manager.numEntries] = new TerrainEnt;
                //printf("Adding new EntityStatic\n");
            } break;
            case ENT_Crawler: {
                manager.pointerList[manager.numEntries] = new CrawlerEnt;
                //printf("Adding new EntityStatic\n");
            } break;
            case ENT_Panel: {
                manager.pointerList[manager.numEntries] = new PanelEnt;
                //printf("Adding new EntityStatic\n");
            } break;
            default: {
                manager.pointerList[manager.numEntries] = new EntityBase;
                //printf("Adding new EntityBase\n");
            } break;
        }
        ent = manager.pointerList[manager.numEntries];
        ent->ID = manager.numEntries;
        ent->subType = type;
    }

    //increment entry count
    //printf("Inserting entry into position %d of %d\n\n", 
    //        manager.numEntries, manager.maxSize);
    manager.numEntries++;

    Matrix::buildFromTRS(&ent->modelMatrix, ent->position, ent->orientation, ent->scale);

    return ent->ID;
}

EntityBase* Entity::lookup_entity_by_id(int ID) {
    if (ID >= manager.numEntries)  {
        printf("Error: Trying to access entity ID %d, which does not exist.\n", ID);
        return NULL;
    }

    return manager.pointerList[ID];
}

/*void Entity::loadEntityFromFile(std::string entFilename, int* idLookup) {
    //Check if entity is already loaded
    bool readFromFile = true;

    std::string entResourcePath = "../data/entities/";
    std::string entResourceEnd = ".ent";
    std::string fullPath = entResourcePath + entFilename + entResourceEnd;

    LoadOptions opts;
    if (readFromFile) {
        printf("Reading from file.\n");
        // Else load entity from file
        FILE* entFile = fopen(fullPath.c_str(), "rb");
        if (entFile == NULL) {
            printf("Failed to open file [%s]\n", fullPath.c_str());
            return;
        }
        printf("Reading file: \"%s\"\n", fullPath.c_str());
        
        fseek(entFile, 0, SEEK_END);
        long fileLength = ftell(entFile);
        fseek(entFile, 0, SEEK_SET);

        char *fileContents = (char *)malloc(fileLength + 1);
        fread(fileContents, fileLength, 1, entFile);
        fclose(entFile);
        fileContents[fileLength] = 0;

        //EntityBase ent;
        char* path = NULL;
        //opts.print();

        char* lineContents = strtok(fileContents, "\r\n");
        while (lineContents != NULL) {
            // lineContents is the current line being read
            if (lineContents[0] != '#') {
                // This line isn't a comment
                //printf("[%d]\t%s\n", i++, lineContents);

                switch (lineContents[0]) {
                    case ':': {
                        //parse command
                        parseCommand(lineContents, &opts);
                    } break;
                    case 'p': {
                        opts.position = parseVec3(lineContents);
                    } break;
                    case 'r': {
                        quat rot = parseQuat(lineContents);
                        if (!opts.quatLoad) {
                            vec3 axis = {rot.x, rot.y, rot.z};
                            float angle = rot.w;
                            if (opts.degreeLoad)
                                Quaternion::buildFromAxisAngleD(rot, axis, angle);
                            else
                                Quaternion::buildFromAxisAngle(rot, axis, angle);
                        }
                        opts.orientation = rot;
                    } break;
                    case 's': {
                        opts.scale = parseVec3(lineContents);
                    } break;
                    case 'M': {
                        //int pathLength = strlen(lineContents+2);
                        //ent.modelFilePath = (char*)malloc((pathLength+1)*sizeof(char));
                        //ent.modelFilePath[pathLength] = 0;
                        //strcpy(ent.modelFilePath, lineContents+2);
                        int pathLength = strlen(lineContents+2);

                        path = (char*)malloc((pathLength+1)*sizeof(char));
                        path[pathLength] = 0;
                        strcpy(path, lineContents+2);
                        opts.modelFilePath = path;
                    } break;
                    case 'C': {
                        opts.color = parseVec3(lineContents) * (1/255.0f);
                    } break;
                    default: {
                    } break;
                }
            }

            //prepare the next line to be read
            lineContents = strtok(NULL, "\r\n");
        }
        free(fileContents);
    }

    int id = registerEntity(&opts);
    if (idLookup != NULL) {
        *idLookup = id;
    }
}

quat Entity::parseQuat(char* line, bool normalise) {
    int start = 0;
    while(line[start] != ' ') {
        start++;
    }
    while(line[start] == ' ') {
        start++;
    }
    char* pstr = line+start;
    float x = strtof(pstr, &pstr);  //split the string into three floats
    float y = strtof(pstr, &pstr);
    float z = strtof(pstr, &pstr);
    float w = strtof(pstr, NULL);

    quat ret = {x, y, z, w};
    if (normalise) {
        Quaternion::normalize(ret);
    }
    //ret.print("\tQuaternion: ");
    return ret;
}

vec3 Entity::parseVec3(char* line, bool normalise) {
    int start = 0;
    while(line[start] != ' ') {
        start++;
    }
    while(line[start] == ' ') {
        start++;
    }
    char* pstr = line+start;
    float x = strtof(pstr, &pstr);  //split the string into three floats
    float y = strtof(pstr, &pstr);
    float z = strtof(pstr, NULL);

    vec3 ret = {x, y, z};
    if (normalise) {
        Vector::normalize(ret);
    }
    //ret.print("\tVector3: ");
    return ret;
}

void Entity::parseCommand(char* line, LoadOptions* ent) {
    int start = 0;
    while (line[start] != ' ') {
        start++;
    }
    start--;
    char* varName = (char*)malloc((start+1) * sizeof(char));
    strncpy(varName, line+1, start);
    varName[start] = 0;
    //printf("\t[%s] ", varName);
    start += 2;
    char* pstr = line+start;

    // set variable [varName] to [value]
    if (!strcmp(varName, "quaternionRot")) {
        ent->quatLoad = true;
    } else if (!strcmp(varName, "player")) {
        ent->subType = ENT_Player;
    } else if (!strcmp(varName, "static")) {
        ent->subType = ENT_Static;
    } else if (!strcmp(varName, "axisAngleRot")) {
        ent->quatLoad = false;
    } else if (!strcmp(varName, "degree")) {
        ent->degreeLoad = true;
    } else if (!strcmp(varName, "radian")) {

        ent->degreeLoad = false;
    } else if (!strcmp(varName, "defaultScale")) {
        ent->scale = {1, 1, 1};        
    } else {
        printf("Unknown variable [%s] found.\n", varName);
    }
    free(varName);
}
*/
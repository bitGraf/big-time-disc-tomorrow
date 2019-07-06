#include "EntityManager.h"

EntityManager Entity::manager;

void Entity::init_entities(WindowInfo windowInfo) {
    //
    // Load assets from file
    //

    ModelLoader::loadFile(&manager.axis, "../data/models/axis.modl");
    ModelLoader::loadFileStanford(&manager.questionMark, "../data/models/unknownModel.ply");
    manager.default_baseColor.loadImage("DEFAULT_baseColor.png");
    manager.default_normalMap.loadImage("DEFAULT_normalMap.png");
    manager.default_amrMap.loadImage("DEFAULT_amrMap.png");

    // Entities
    //Resources::manager.loadTriMeshResource("cube", ".modl");
    Resources::manager.loadTriMeshResource("bigBot", ".ply");   //very large file lmao
    Resources::manager.loadTriMeshResource("bot2", ".ply", true);
    Resources::manager.loadTriMeshResource("bot3", ".ply", true);

    Resources::manager.loadTextureResource("wall", ".jpg");
    Resources::manager.loadTextureResource("sample", ".jpg");
    Resources::manager.loadTextureResource("blank", ".png");    //just a white texture
    Resources::manager.loadTextureResource("grid", ".png");    //UV Grid

    //Texture collection - rustediron2
    Resources::manager.loadTextureResource("rustediron2_basecolor", ".jpg");
    Resources::manager.loadTextureResource("rustediron2_normal", ".jpg");
    Resources::manager.loadTextureResource("rustediron2_amr", ".jpg");


    manager.Player = (CrawlerEnt*)Entity::createNewEntity(ENT_Crawler);
    manager.Player->mesh = Resources::manager.getTriMeshResource("bot3");
    manager.Player->baseColor = Resources::manager.getTextureResource("rustediron2_basecolor");
    manager.Player->normalMap = Resources::manager.getTextureResource("rustediron2_normal");
    manager.Player->amrMap = Resources::manager.getTextureResource("rustediron2_amr");

    printf("Crawler::Before load\n");
    //manager.Player->currentLevel = LevelLoader::loadLevel("../data/levels/level2.lvl");
    printf("Crawler::After load\n");
    //ent->Color = {5, 5, 7};

    //Level::loadLevel("mountains512", ".png", "mColor", ".png");
    EntityBase* ent = Entity::createNewEntity(ENT_Static);
    ent->mesh = Resources::manager.getTriMeshResource("plane");
    ent->baseColor = Resources::manager.getTextureResource("grid");
    ent->scale = { 64, 64, 64 };//one block  = 2 units
    ent->Color = { 4, 4, 4 };
    StaticEnt* se = (StaticEnt*)ent;
    se->Rainbow = false;

    manager.font = Font::newDynamicFont("../data/fonts/Consolas.ttf", 20);

    Entity::printAllEntities();

    //Camera
    manager.camera.updateProjectionMatrix(windowInfo);

    float lightStrength = 5;
    manager.lights[0].position = { 10, 3, 10};
    manager.lights[0].color = { 1, 0, 0 };
    manager.lights[0].strength = lightStrength;
    manager.lights[1].position = {-10, 3, 10};
    manager.lights[1].color = { 0, 1, 0 };
    manager.lights[1].strength = lightStrength;
    manager.lights[2].position = {-10, 3,-10};
    manager.lights[2].color = { 0, 0, 1 };
    manager.lights[2].strength = lightStrength;
    manager.lights[3].position = { 10, 3,-10};
    manager.lights[3].color = { 0, 1, 1 };
    manager.lights[3].strength = lightStrength;

    manager.sun.direction = -Vector::normalized({100,200,0});
    manager.sun.color = { 1,1,1 };
    manager.sun.strength = 0;

    manager.spotlight.position = { 0, 10, 0 };
    manager.spotlight.direction = { 0,-1,0 };
    manager.spotlight.color = { 1,0,1 };
    manager.spotlight.strength = 50;
    manager.spotlight.cutoff = cos(deg2Rad(35));
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
    if ((key == GLFW_KEY_F) && (action == GLFW_PRESS)) {
        printf("toggle frame rendering...\n");

        Entity::manager.showFrames = !Entity::manager.showFrames;
    }

    if ((key == GLFW_KEY_O) && (action == GLFW_PRESS)) {
        printf("Listing all entities...\n");

        Entity::printAllEntities();
    }

    if ((key == GLFW_KEY_P) && (action == GLFW_PRESS)) {
        printf("Listing all entities...\n");

        Entity::pruneEntities();
    }

    if ((key == GLFW_KEY_M) && (action == GLFW_PRESS)) {
        printf("Toggling player control\n");

        manager.Player->processInput = !manager.Player->processInput;
        manager.Player->shouldRender = !manager.Player->shouldRender;
    }

    for (int i = 0; i < manager.numEntries; i++) {
        EntityBase* ent = (manager.pointerList[i]);

        if (!ent->Remove) {
            if (ent->processInput) {
                ent->handleInput(key, scancode, action, mods);
            }
        }
    }
}

void Entity::fixedUpdateAllEntities(double dt) {
    for (int i = 0; i < manager.numEntries; i++) {
        EntityBase* ent = (manager.pointerList[i]);
        
        if (!ent->Remove)
            ent->update(dt);    //For now, just ignore "Removed" entitites
    }
}

void Entity::renderAllEntities(ShaderProgram* shader) {
    for (int i = 0; i < manager.numEntries; i++) {
        EntityBase* ent = (manager.pointerList[i]);

        if (ent->shouldRender) {
            ent->preRender();
            
            shader->use();
            shader->setMat4("view", &Entity::manager.camera.viewMatrix);
            shader->setvec3("camPos", &Entity::manager.camera.position);
            shader->setMat4("model", &ent->modelMatrix);
            shader->setInt("baseColor", 0);
            shader->setInt("normalMap", 1);
            shader->setInt("amrMap", 2);

            std::string uName = "pointLights[";
            for (int i = 0; i < 4; i++) {
                shader->setPointLight(uName + std::to_string(i) + "]", manager.lights[i]);
            }
            shader->setDirectionalLight("sun", manager.sun);
            shader->setSpotLight("spotLight", manager.spotlight);

            int verts2render = 0;
            if (ent->mesh == NULL) {
                glBindVertexArray(manager.questionMark.VAO);
                verts2render = manager.questionMark.numFaces*3;
                ent->Color = {3, 3, 0};
            } else {
                glBindVertexArray(ent->mesh->data.VAO);
                verts2render = ent->mesh->data.numFaces*3;
            }

            if (ent->baseColor == NULL)
                manager.default_baseColor.bind(GL_TEXTURE0);
            else 
                ent->baseColor->data.bind(GL_TEXTURE0);

            if (ent->normalMap == NULL) {
                manager.default_normalMap.bind(GL_TEXTURE1);
            }
            else {
                ent->normalMap->data.bind(GL_TEXTURE1);
            }

            if (ent->amrMap == NULL)
                manager.default_amrMap.bind(GL_TEXTURE2);
            else 
                ent->amrMap->data.bind(GL_TEXTURE2);


            shader->setvec3("color", &ent->Color);

            glDrawElements(GL_TRIANGLES, verts2render, GL_UNSIGNED_INT, 0);
        }
    }


    if (manager.showFrames) {
        for (int i = 0; i < manager.numEntries; i++) {
            EntityBase* ent = (manager.pointerList[i]);

            if (ent->shouldRender) {
                ent->postRender();


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

            printf("%p\t%d\t%d\t%s\n", ent, ent->ID, ent->mesh == NULL ? -1 : ent->mesh->data.VAO,
                ent->subType==ENT_Base ? " " : (
                ent->subType==ENT_Player ? "Player" : (
                ent->subType==ENT_Crawler ? "Crawler" : (
                ent->subType==ENT_AI ? "AI" : (
                ent->subType==ENT_Panel ? "Panel" : (
                ent->subType==ENT_Static ? "Static" : (
                ent->subType==ENT_Temporary ? "Temporary" : (
                ent->subType==ENT_Terrain ? "Terrain" : (
				ent->subType==ENT_Missile ? "Missile" :
                    "Unknown type")))))))));
        } else {
            printf("%p\t%d\t%d\n", manager.pointerList[i],
            0, 0);
        }
    }
    printf("--------------------------------------------------\n\n");
}

int Entity::registerEntity(EntityTypes type) {
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
        case ENT_Missile: {
            manager.pointerList[manager.numEntries] = new MissileEnt;
            //printf("Adding new EntityStatic\n");
        } break;
        case ENT_Physics: {
            manager.pointerList[manager.numEntries] = new PhysicsEnt;
            //printf("Adding new EntityStatic\n");
        } break;
        case ENT_AI: {
            manager.pointerList[manager.numEntries] = new AIEnt;
            //printf("Adding new EntityAI\n");
        } break;
        case ENT_Collision: {
            manager.pointerList[manager.numEntries] = new CollisionEntity;
            //printf("Adding new EntityAI\n");
        } break;
        case ENT_Actor: {
            manager.pointerList[manager.numEntries] = new ActorEntity;
            //printf("Adding new EntityAI\n");
        } break;
        default: {
            manager.pointerList[manager.numEntries] = new EntityBase;
            //printf("Adding new EntityBase\n");
        } break;
    }
    ent = manager.pointerList[manager.numEntries];
    ent->ID = manager.numEntries;
    ent->subType = type;

    //increment entry count
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

void Entity::pruneEntities() {
    printf("Pruning entities. There are currently %d entitiesin the list,\n", manager.numEntries);
    int num2remove = 0;
    for (int i = 0; i < manager.numEntries; i++) {
        if (manager.pointerList[i]->Remove)
            num2remove++;
    }
    printf(" %d need to be removed.\n", num2remove);

    if (num2remove > 0) {
        int newNumEntries = manager.numEntries - num2remove;
        EntityBase** newList = (EntityBase**)malloc(newNumEntries * sizeof(EntityBase*));
        int curr = 0;
        for (int i = 0; i < manager.numEntries; i++) {
            if (!manager.pointerList[i]->Remove) {
                newList[curr] = manager.pointerList[i];
                curr++;
            } else {
                manager.pointerList[i]->onDestroy(); // Call the entity specific cleanup function
                free(manager.pointerList[i]);
                manager.pointerList[i] = NULL;
            }
        }
        free(manager.pointerList);
        manager.pointerList = newList;
        manager.numEntries = newNumEntries;
        manager.maxSize = manager.numEntries;
    }

    Entity::printAllEntities();
}
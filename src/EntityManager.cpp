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
                ent->subType==ENT_Crawler ? "Crawler" : (
                ent->subType==ENT_Panel ? "Panel" : (
                ent->subType==ENT_Static ? "Static" : (
                ent->subType==ENT_Temporary ? "Temporary" : (
                ent->subType==ENT_Terrain ? "Terrain" : 
                    "Unknown type")))))));
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
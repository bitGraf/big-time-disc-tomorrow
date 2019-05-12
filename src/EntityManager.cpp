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

    Resources::manager.loadTextureResource("wall", ".jpg");
    Resources::manager.loadTextureResource("sample", ".jpg");
    Resources::manager.loadTextureResource("blank", ".png");    //just a white texture
    Resources::manager.loadTextureResource("grid", ".png");    //UV Grid

    //Texture collection - rustediron2
    Resources::manager.loadTextureResource("rustediron2_basecolor", ".jpg");
    Resources::manager.loadTextureResource("rustediron2_normal", ".jpg");
    Resources::manager.loadTextureResource("rustediron2_amr", ".jpg");


    manager.Player = (CrawlerEnt*)Entity::createNewEntity(ENT_Crawler);
    manager.Player->mesh = Resources::manager.getTriMeshResource("bigBot");
    manager.Player->baseColor = Resources::manager.getTextureResource("rustediron2_basecolor");
    manager.Player->normalMap = Resources::manager.getTextureResource("rustediron2_normal");
    manager.Player->amrMap = Resources::manager.getTextureResource("rustediron2_amr");
    //ent->Color = {5, 5, 7};

    //Level::loadLevel("mountains512", ".png", "mColor", ".png");
    EntityBase* ent = Entity::createNewEntity(ENT_Static);
    ent->mesh = Resources::manager.getTriMeshResource("plane");
    ent->baseColor = Resources::manager.getTextureResource("grid");
    ent->scale = {64, 64, 64};//one block  = 2 units
    ent->Color = {4, 4, 4};
    StaticEnt* se = (StaticEnt*)ent;
    se->Rainbow = false;

	ent = Entity::createNewEntity(ENT_AI);
	ent->mesh = Resources::manager.getTriMeshResource("bot2");
	ent->baseColor = Resources::manager.getTextureResource("grid");
	ent->position = { 15, 0, 15 };
	
	ent = Entity::createNewEntity(ENT_AI);
	ent->mesh = Resources::manager.getTriMeshResource("bot2");
	ent->baseColor = Resources::manager.getTextureResource("grid");
	ent->position = { -15, 0, 15 };

	ent = Entity::createNewEntity(ENT_AI);
	ent->mesh = Resources::manager.getTriMeshResource("bot2");
	ent->baseColor = Resources::manager.getTextureResource("grid");
	ent->position = { 15, 0, -15 };
	
	ent = Entity::createNewEntity(ENT_AI);
	ent->mesh = Resources::manager.getTriMeshResource("bot2");
	ent->baseColor = Resources::manager.getTextureResource("grid");
	ent->position = { -15, 0, -15 };

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
    if ((key == GLFW_KEY_F) && (action == GLFW_PRESS)) {
        printf("toggle frame rendering...\n");

        Entity::manager.showFrames = !Entity::manager.showFrames;
	}

    if ((key == GLFW_KEY_I) && (action == GLFW_PRESS)) {
        printf("Listing all entities...\n");

        Entity::printAllEntities();
	}

    for (int i = 0; i < manager.numEntries; i++) {
        EntityBase* ent = (manager.pointerList[i]);

        if (!ent->Remove)
            ent->handleInput(key, scancode, action, mods);
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

        if (!ent->Remove) {
            ent->preRender();
            
            shader->use();
            shader->setMat4("view", &Entity::manager.camera.viewMatrix);
            shader->setvec3("camPos", &Entity::manager.camera.position);
            shader->setMat4("model", &ent->modelMatrix);
            shader->setInt("baseColor", 0);
            shader->setInt("normalMap", 1);
            shader->setInt("amrMap", 2);
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

            if (ent->normalMap == NULL)
                manager.default_normalMap.bind(GL_TEXTURE1);
            else 
                ent->normalMap->data.bind(GL_TEXTURE1);

            if (ent->amrMap == NULL)
                manager.default_amrMap.bind(GL_TEXTURE2);
            else 
                ent->amrMap->data.bind(GL_TEXTURE2);


            shader->setvec3("color", &ent->Color);

            glDrawElements(GL_TRIANGLES, verts2render, GL_UNSIGNED_INT, 0);
        }
    }


    for (int i = 0; i < manager.numEntries; i++) {
            EntityBase* ent = (manager.pointerList[i]);

            if (!ent->Remove) {
                ent->postRender();

                if (manager.showFrames) {
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
                ent->subType==ENT_Terrain ? "Terrain" : 
                    "Unknown type"))))))));
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
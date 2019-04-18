#pragma once

#include "Entity.h"
#include "Player.h"
#include "Camera.h"
#include "Level.h"

#include <string>

enum EntityTypes {
    ENT_Base = 0,
    ENT_Player,
    ENT_Static,
    ENT_Temporary
};

struct LoadOptions {
    bool quatLoad = false;
    bool degreeLoad = false;

    char* modelFilePath = NULL;
    int subType = ENT_Base;

    vec3 position;
    quat orientation;
    vec3 scale = {1, 1, 1};
    vec3 color;
};

struct EntityManager {
    bool logData = false;
    bool pointRender = false;
    //Entity list data
    EntityBase** pointerList = NULL;    //list of pointerList
    GLuint maxSize = 0;                 //amount of memory reserved
    GLuint numEntries = 0;              //amount of currently filled pointers
    GLuint entryGrowthAmount = 64;      //amount to **ADD** to maxSize when full

    //Render objects
    //std::vector<GLuint> VAOs;
    //std::vector<GLuint> VBOs;
    //std::vector<GLuint> EBOs;

    //Camera objects
    Camera camera;

    void freeAll() {
        if (pointerList) {
            printf("Freeing all entities\n");
            for (int i = 0; i < numEntries; i++) {
                //printf("\t%02d/%02d\n", i+1, numEntries);

                //delete pointerList[i];
                pointerList[i]->~EntityBase();
                free(pointerList[i]);
                
                pointerList[i] = NULL;
            }
            printf("Entities freed\n");
        } else {
            printf("How da heck static variables work...\n");
        }
        free(pointerList);
        pointerList = NULL;
        numEntries = 0;
        maxSize = 0;
    }
    ~EntityManager() {
        freeAll();
    }
};

namespace Entity {
    extern EntityManager manager;

    EntityBase* createNewEntity(EntityTypes type = ENT_Base, int* id = NULL);

    void init_entities(WindowInfo windowInfo);

    // Loading new entities
    //void loadEntityFromFile(EntityBase* ent, char* filename);
    //void loadEntityFromFile(std::string entFilename, int* idLookup = NULL);

    // Helper functions for loading
    //vec3 parseVec3(char* line, bool normalise = false);
    //quat parseQuat(char* line, bool normalise = false);
    //void parseCommand(char* line, LoadOptions* ent);

    // Adding entities to the manager
    int registerEntity(EntityTypes type = ENT_Base, LoadOptions* opts = NULL);
    void printAllEntities();
    
    // Operating on current entities
    void renderAllEntities(ShaderProgram* shader);
    void fixedUpdateAllEntities(double dt);
    void handleInputEvent(GLFWwindow* window, int key, int scancode, int action, int mods);

    // Entity lookup
    EntityBase* lookup_entity_by_id(int ID);
}







/*
void print() {
        printf("Entity Load Options...\n");
        printf("\tOrientation: %s\n", quatLoad ? "Quaternion" : "Axis Angle");
        printf("\tUnits: %s\n", degreeLoad ? "Degree" : "Radian");
        printf("\tModel File Path: \"%s\"\n", modelFilePath);
        printf("\tSubtype: %s\n", 
            subType==ENT_Base ? "Default" : (
                subType==ENT_Player ? "Player" : (
                    "Static")));
        position.print("\tPosition: ");
        orientation.print(quatLoad ? "\tQuaternion: " : "\tAxis Angle: ");
        scale.print("\tScale: ");
    }
*/
#pragma once

#include "ModelLoader.h"
#include "Vector.h"
#include "Quaternion.h"
#include "Matrix.h"
#include "ShaderProgram.h"
#include "Texture.h"

#include "ResourceManager.h"

struct EntityBase {
    //Transform components
    vec3 position;
    quat orientation;
    vec3 scale = {1, 1, 1};
    vec3 Forward, Left, Up;

    //model transform matrix
    mat4 modelMatrix;

    //Actual mesh data (verts, norms, ...)
    TriMeshResource* mesh = NULL;

    //Entity information
    int ID = -1;//array[ID] to lookup pointer to this in manager
    int subType = -1;
    bool Remove = false;    //whether to remove next update cycle

    //Textures
    TextureResource* baseColor = NULL;
    TextureResource* normalMap = NULL;
    TextureResource* amrMap = NULL;

    //Other information
    vec3 Color = {1, 1, 1};
	float health = 100;

    //Common function prototypes
    virtual void update(double dt) {
        Matrix::buildFromTRS(&modelMatrix, position, orientation, scale);

        float C11 = 1 - 2*orientation.y*orientation.y - 2*orientation.z*orientation.z;
        float C12 = 2*(orientation.x*orientation.y - orientation.z*orientation.w);
        float C13 = 2*(orientation.z*orientation.x + orientation.y*orientation.w);
        float C21 = 2*(orientation.x*orientation.y + orientation.z*orientation.w);
        float C22 = 1 - 2*orientation.z*orientation.z - 2*orientation.x*orientation.x;
        float C23 = 2*(orientation.y*orientation.z - orientation.x*orientation.w);
        float C31 = 2*(orientation.z*orientation.x - orientation.y*orientation.w);
        float C32 = 2*(orientation.y*orientation.z + orientation.x*orientation.w);
        float C33 = 1 - 2*orientation.x*orientation.x - 2*orientation.y*orientation.y;
        
        Left    = {C11, C21, C31};
        Up      = {C12, C22, C32};
        Forward = {C13, C23, C33};
    }

    virtual void preRender() {}
    virtual void postRender() {}
    virtual void handleInput(int key, int scancode, int action, int mods) {}
    virtual void onCreate() {}

    virtual ~EntityBase() {
        printf("\tFreeing Entity [%2d]...\n", ID);
        //free(modelFilePath);
    }
};


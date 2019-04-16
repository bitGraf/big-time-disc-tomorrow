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

    //model transform matrix
    mat4 modelMatrix;

    //Actual mesh data (verts, norms, ...)
    TriMeshResource* mesh;

    //Entity information
    int ID = -1;//array[ID] to lookup pointer to this in manager
    int subType = -1;
    bool Remove = false;    //whether to remove next update cycle

    //Textures
    TextureResource* baseColor;

    //Other information
    vec3 Color = {1, 1, 1};

    //Common function prototypes
    virtual void update(double dt) {
        Matrix::buildFromTRS(&modelMatrix, position, orientation, scale);
    }

    virtual void preRender() {}
    virtual void handleInput(int key, int scancode, int action, int mods) {}

    virtual ~EntityBase() {
        printf("\tFreeing Entity [%2d]...\n", ID);
        //free(modelFilePath);
    }
};


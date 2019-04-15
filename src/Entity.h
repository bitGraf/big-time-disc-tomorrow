#pragma once

#include "ModelLoader.h"
#include "Vector.h"
#include "Quaternion.h"
#include "Matrix.h"
#include "ShaderProgram.h"
#include "Texture.h"

struct EntityBase {
    //Transform components
    vec3 position;
    quat orientation;
    vec3 scale;

    //model transform matrix
    mat4 modelMatrix;

    //Actual mesh data (verts, norms, ...)
    TriangleMesh mesh;
    char* modelFilePath = NULL;

    //Entity information
    int ID = -1;//array[ID] to lookup pointer to this in manager
    int subType = -1;

    //Textures
    Texture baseColor;

    //Other information
    vec3 Color;

    //Common function prototypes
    virtual void update(double dt) {
        Matrix::buildFromTRS(&modelMatrix, position, orientation, scale);
    }

    virtual void preRender() {}
    virtual void handleInput(int key, int scancode, int action, int mods) {}

    virtual ~EntityBase() {
        printf("\tFreeing Entity [%2d]...\n", ID);
        free(modelFilePath);
    }
};


#pragma once

#include "ModelLoader.h"
#include "Vector.h"
#include "Quaternion.h"
#include "Matrix.h"
#include "ShaderProgram.h"

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

    //Other information
    vec3 Color;

    //Shader to use
    //ShaderProgram* shader = NULL;
    void update() {
        //printf("[%d] EntityBase Update\n", ID);
    }

    ~EntityBase() {
        printf("\tFreeing Entity [%2d]...\n", ID);
        free(modelFilePath);
    }
};


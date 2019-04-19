#pragma once

#include "glad/glad.h"
#include "stdio.h"
#include "stdlib.h"
#include "Vector.h"
#include <assert.h>
#include <vector>

struct Triangle {
    int indices[3] = {0,0,0};
};

struct TriangleMesh {
    int numVerts = 0;
    int numFaces = 0;

    //Vertex data
    vec3 *vertices = NULL;
    vec3 *normals = NULL;
	vec2 *texcoords = NULL;

    //Face data
    GLuint *indices = NULL;

    //loading flags
    bool normalisedOnLoad = false;
    bool lineRender = false;
    bool calcNormalsFromVerts = false;
    bool index1 = false;

    //Render information
    GLuint VAO = 0;
    GLuint VBOpos = 0;
    GLuint VBOnorm = 0;
	GLuint VBOtex = 0;
    GLuint EBO = 0;

    ~TriangleMesh() {
        //is TriMesh ever loses scope, free memory
        printf("\t\tFreeing Triangle mesh data [VAO %d]\n", VAO);
        free(vertices);
        free(indices);
        free(normals);
		free(texcoords);
    }
};

namespace ModelLoader {
    void loadFile(TriangleMesh* triMesh, char* filename);
    void loadFileStanford(TriangleMesh* triMesh, char* filename);
    void bufferModel(TriangleMesh* mesh, bool lineRender = false);
    
    vec3 parseVec3(char* line, bool normalise = false);
	vec2 parseVec2(char* line, bool normalise = false);
    Triangle parseFace(char* line, bool index1 = false);
    void parseCommand(char* line, TriangleMesh* mesh);
}
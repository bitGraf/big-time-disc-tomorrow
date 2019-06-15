#ifndef EPA_H
#define EPA_H

#include "GJK.h"

#define EPA_MAX_SIMPLEX_SIZE 50

struct EPA_Face {
    float dotVal = 0;
    vec3 v1;
    vec3 v2;
    vec3 v3;

    vec3 normal;
    vec3 center;
    float distance;
};

struct EPA_Edge {
    float dotVal = 0;
    vec3 v1;
    vec3 v2;
    bool remove = false;
};

struct EPA_Simplex {
    unsigned int numFaces = 0;
    EPA_Face faces[EPA_MAX_SIMPLEX_SIZE];
};

vec3 EPA(GJK_Result* gjk_result);
void createFaceFromVerts(EPA_Face* face, vec3 a, vec3 b, vec3 c);
EPA_Face getClosestFace(EPA_Simplex* simplex);
void createNewSimplex(EPA_Simplex* simplex, EPA_Face face, vec3 P);
int getEdgeList(EPA_Edge* list, EPA_Simplex* simplex, EPA_Face face, vec3 P);
int addToList(EPA_Edge* list, int numEdges, EPA_Face);
void shiftList(int start, int end, int amount, EPA_Simplex* list);
void shiftEdgeList(int start, int end, int amount, EPA_Edge* list);
void addFace(EPA_Simplex* simplex, vec3 a, vec3 b, vec3 c);
bool notInList(EPA_Edge* list, int numEdges, EPA_Edge edge1);
bool similarEdge(vec3 a, vec3 b, vec3 c, vec3 d, float tol = .01f);

#endif
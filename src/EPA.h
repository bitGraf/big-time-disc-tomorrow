#ifndef EPA_H
#define EPA_H

#include "GJK.h"

#define EPA_MAX_SIMPLEX_SIZE 100

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

struct EPA_Result {
    //return values
    vec3 penetrationNormal;
    float penetrationDepth = 0;
    GJK_SupportPoint contactPoint;
    bool converged = false;

    //persistent information
    EPA_Simplex simplex;
    int iteration = 0;
};

void epa_seed(EPA_Result* res, GJK_Result* seed, EPA_Face* face);
int epa_iteration(EPA_Result* res, GJK_SupportPoint p, EPA_Face* face);
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
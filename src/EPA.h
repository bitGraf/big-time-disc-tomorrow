#ifndef EPA_H_
#define EPA_H_

#include "GJK.h"
#include <forward_list>
#include <vector>

struct EPA_Face {
    int ID;

    vec3 v1;
    vec3 v2;
    vec3 v3;

    vec3 normal;
    float distance;

    bool operator==(EPA_Face f);
};

struct EPA_Edge {
    vec3 v1;
    vec3 v2;
    bool keep = true;

    bool operator==(EPA_Edge e);
};

struct EPA_Struct {
    std::forward_list<EPA_Face> faces;
    GJK_SupportPoint contactPoint;
    int iteration;
};

int epa_seed(EPA_Struct* res, GJK_Struct* seed, EPA_Face* face);
int epa_iteration(EPA_Struct* s, EPA_Face* face, GJK_SupportPoint p);
void epa_process();

std::vector<EPA_Edge> makeUnique(std::vector<EPA_Edge> list);
void addFaceToList(std::vector<EPA_Edge>* list, EPA_Face f);
void updateSimplex(EPA_Struct* s, EPA_Face, vec3 p);
EPA_Face ripClosestFace(EPA_Struct* s);
EPA_Face createFaceFromVerts(vec3 a, vec3 b, vec3 c);
void printCurrentsimplex(EPA_Struct* s);

#endif

/*
#ifndef EPA_H
#define EPA_H

#include "GJK.h"

#define EPA_MAX_SIMPLEX_SIZE 400

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

struct EPA_Struct {
    //return values
    vec3 penetrationNormal;
    float penetrationDepth = 0;
    GJK_SupportPoint contactPoint;

    //persistent information
    EPA_Simplex simplex;
    int iteration = 0;
};

void epa_processResults(EPA_Struct* epa_res, GJK_Struct* gjk_res);
int epa_seed(EPA_Struct* res, GJK_Struct* seed, EPA_Face* face);
int epa_iteration(EPA_Struct* res, GJK_SupportPoint p, EPA_Face* face);
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
*/
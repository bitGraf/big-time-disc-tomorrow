#include "EPA.h"
#include <assert.h>

void epa_seed(EPA_Result* res, GJK_Result* seed, EPA_Face* face) {
    if (!seed->hit) {
        printf("Cannot perform EPA algorithm on a non-intersecting simplex.\n");
        return;
    }

    createFaceFromVerts(&res->simplex.faces[0], //ABC
        seed->simplex[0].P, seed->simplex[1].P, seed->simplex[2].P);
    createFaceFromVerts(&res->simplex.faces[1], //ABD
        seed->simplex[0].P, seed->simplex[1].P, seed->simplex[3].P);
    createFaceFromVerts(&res->simplex.faces[2], //BCD
        seed->simplex[1].P, seed->simplex[2].P, seed->simplex[3].P);
    createFaceFromVerts(&res->simplex.faces[3], //CAD
        seed->simplex[2].P, seed->simplex[0].P, seed->simplex[3].P);
    res->simplex.numFaces = 4;

    *face = getClosestFace(&res->simplex);
}

int epa_iteration(EPA_Result* res, GJK_SupportPoint p, EPA_Face* face) {
    float distanceFromPlane = Vector::dot(face->normal, p.P - face->center);
    if (distanceFromPlane < .1) {
        //converged to final solution
        res->converged = true;
        res->contactPoint = p;
        res->penetrationNormal = Vector::normalized(p.P);
        res->penetrationDepth = Vector::magnitude(p.P);
        return 0;
    }

    createNewSimplex(&res->simplex, *face, p.P);

    res->iteration++;

    *face = getClosestFace(&res->simplex);

    return 1;
}

vec3 EPA(GJK_Result* gjk_result) {
    // temp placeholders
    vec3 f1 = { 0,3,0 }; //r=4
    vec3 f2 = { 3,5,0 }; //r=2

    if (!gjk_result->hit) {
        printf("Cannot perform EPA algorithm on a non-intersecting simplex.\n");
        return { 0,0,0 };
    }

    EPA_Simplex simplex = { 0 };
    createFaceFromVerts(&simplex.faces[0], //ABC
        gjk_result->simplex[0].P, gjk_result->simplex[1].P, gjk_result->simplex[2].P);
    createFaceFromVerts(&simplex.faces[1], //ABD
        gjk_result->simplex[0].P, gjk_result->simplex[1].P, gjk_result->simplex[3].P);
    createFaceFromVerts(&simplex.faces[2], //BCD
        gjk_result->simplex[1].P, gjk_result->simplex[2].P, gjk_result->simplex[3].P);
    createFaceFromVerts(&simplex.faces[3], //CAD
        gjk_result->simplex[2].P, gjk_result->simplex[0].P, gjk_result->simplex[3].P);
    simplex.numFaces = 4;

    vec3 search_dir;

    int iteration = 0, maxIterations = 30;
    while (iteration < maxIterations) {
        EPA_Face face = getClosestFace(&simplex);
        search_dir = face.normal;
        vec3 P = gjk_support(search_dir, f1, f2);

        float distanceFromPlane = Vector::dot(face.normal, P - face.center);
        if (distanceFromPlane < .1) {
            break;
        }

        createNewSimplex(&simplex, face, P);

        iteration++;
    }
    
    vec3 P = gjk_support(search_dir, f1, f2);
    float penDepth = Vector::magnitude(P);
    vec3  penNormal = Vector::normalized(P);

    return (penNormal * penDepth);
}

void createNewSimplex(EPA_Simplex* simplex, EPA_Face face, vec3 P) {
    EPA_Edge edgeList[36];
    //printf("%d faces\n", simplex->numFaces);
    int numEdges = getEdgeList(edgeList, simplex, face, P);

    //edgeList is a unique list of edges
    for (int i = 0; i < numEdges; i++) {
        addFace(simplex, edgeList[i].v1, edgeList[i].v2, P);
    }
}

void addFace(EPA_Simplex* simplex, vec3 a, vec3 b, vec3 c) {
    createFaceFromVerts(&simplex->faces[simplex->numFaces], a, b, c);
    simplex->numFaces++;
}

int getEdgeList(EPA_Edge* list, EPA_Simplex* simplex, EPA_Face face, vec3 P) {
    int numEdges = addToList(&list[0], 0, face);
    int numBack = 0;

    for (int i = 0; i < simplex->numFaces; i++) {
        if (Vector::dot(simplex->faces[i].normal, P-simplex->faces[i].center) > 0) {
            // need to remove face i from the list, and add its edges to the list
            numEdges = addToList(list, numEdges, simplex->faces[i]); // ignores duplicates
            shiftList(i, simplex->numFaces, 1, simplex);
            simplex->numFaces--;
        }
    }
    return numEdges;
}

void shiftList(int start, int end, int amount, EPA_Simplex* list) {
    for (int i = start+amount; i < end; i++) {
        list->faces[i - amount] = list->faces[i];
    }
    list->faces[end - 1] = { 0 };
}

void shiftEdgeList(int start, int end, int amount, EPA_Edge* list) {
    for (int i = start + amount; i < end; i++) {
        list[i - amount] = list[i];
    }
    list[end - 1] = { 0 };
}

int addToList(EPA_Edge* list, int numEdges, EPA_Face face) {
    EPA_Edge toAdd[3];
    toAdd[0].v1 = face.v1;
    toAdd[0].v2 = face.v2;
    toAdd[0].dotVal = Vector::dot(toAdd[0].v1, toAdd[0].v2);
    toAdd[1].v1 = face.v2;
    toAdd[1].v2 = face.v3;
    toAdd[1].dotVal = Vector::dot(toAdd[1].v1, toAdd[1].v2);
    toAdd[2].v1 = face.v3;
    toAdd[2].v2 = face.v1;
    toAdd[2].dotVal = Vector::dot(toAdd[2].v1, toAdd[2].v2);
    int added = 0;
    for (int i = 0; i < 3; i++) {
        //if (notInList(list, numEdges, toAdd[i])) {
            list[numEdges + added] = toAdd[i];
            added++;
        //}
    }
    int newSize = numEdges + added;
    //Now prune the list
    for (int i = 0; i < newSize; i++) {
        EPA_Edge edge1 = list[i];
        for (int j = i + 1; j < newSize; j++) {
            EPA_Edge edge2 = list[j];

            if (similarEdge(edge1.v1, edge1.v2, edge2.v1, edge2.v2)) {
                // remove i and j from list
                list[i].remove = true;
                list[j].remove = true;
            }
        }
    }

    for (int i = 0; i < newSize; i++) {
        if (list[i].remove) {
            shiftEdgeList(i, newSize, 1, list);
            newSize--;
            added--;
        }
    }

    return newSize;
}

bool notInList(EPA_Edge* list, int numEdges, EPA_Edge edge1) {
    for (int i = 0; i < numEdges; i++) {
        EPA_Edge* edge2 = &list[i];
        if (similarEdge(edge1.v1, edge1.v2, edge2->v1, edge2->v2))
            return false;
    }
    return true;
}

bool similarEdge(vec3 a, vec3 b, vec3 c, vec3 d, float tol) {
    if ((Vector::magnitude(a - c) < tol && Vector::magnitude(b - d) < tol) ||
        (Vector::magnitude(a - d) < tol && Vector::magnitude(b - c) < tol))
        return true;
    else
        return false;
}

EPA_Face getClosestFace(EPA_Simplex* simplex) {
    EPA_Face closestFace;
    int closestID = 0;
    float closestDist = 10000000;

    // Find face that is closest to origin
    for (int i = 0; i < simplex->numFaces; i++) {
        auto face = &simplex->faces[i];
        if (face->distance < closestDist) {
            closestDist = face->distance;
            closestID = i;
        }
    }

    // Remove closest face from list, return that face though
    closestFace = simplex->faces[closestID];

    for (int n = closestID+1; n < simplex->numFaces; n++) {
        simplex->faces[n - 1] = simplex->faces[n];
    }
    simplex->numFaces--;
    simplex->faces[simplex->numFaces] = { 0 };

    return closestFace;
}

void createFaceFromVerts(EPA_Face* face, vec3 a, vec3 b, vec3 c) {
    face->v1 = a;
    face->v2 = b;
    face->v3 = c;
    face->center = (a + b + c) * (1.0f / 3.0f);
    face->normal = Vector::normalized(Vector::cross(b - a, c - a));
    face->dotVal = Vector::dot(a*2 + b, c - a);

    if (Vector::dot(face->normal, face->center) < 0) {
        face->normal = -face->normal;
    }
    face->distance = Vector::dot(face->normal, face->center);
}
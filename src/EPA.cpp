#include "EPA.h"

int epa_seed(EPA_Struct* res, GJK_Struct* seed, EPA_Face* face) {
    if (!seed->hit) {
        printf("Cannot perform EPA algorithm on a non-intersecting simplex.\n");
        return 0;
    }

    printf("GJK Simplex size: %d\n", seed->cnt);
    if (seed->cnt != 4) {
        printf("Exiting EPA algo\n");
        return 0;
    }

    vec3 a, b, c, d;
    a = seed->simplex[0].P;
    b = seed->simplex[1].P;
    c = seed->simplex[2].P;
    d = seed->simplex[3].P;

    res->faces.push_front(createFaceFromVerts(a, b, c));
    res->faces.push_front(createFaceFromVerts(a, b, d));
    res->faces.push_front(createFaceFromVerts(b, c, d));
    res->faces.push_front(createFaceFromVerts(c, a, d));

    printCurrentsimplex(res);

    *face = ripClosestFace(res);

    printCurrentsimplex(res);

    res->iteration = 0;

    return 1;
}

int epa_iteration(EPA_Struct* s, EPA_Face* face, GJK_SupportPoint p) {
    printf("EPA_Iteration %d\n", s->iteration);
    float distanceFromPlane = fabs(Vector::dot(face->normal, p.P - face->v1));
    if (distanceFromPlane < .25) {
        //converged to final solution
        printf("Done\n");

        //actually need to find point on face that is closest to origin
        s->contactPoint.P = face->normal * face->distance; 

        printCurrentsimplex(s);
        
        return 0;
    }

    updateSimplex(s, *face, p.P);

    *face = ripClosestFace(s);

    s->iteration++;
    return 1;
}

void epa_process() {

}

void updateSimplex(EPA_Struct* s, EPA_Face f, vec3 p) {
    std::vector<EPA_Edge> edgeList;
    printf("Edges: %d\n", (int)edgeList.size());
    // Get edges from face
    addFaceToList(&edgeList, f);
    printf("Edges: %d\n", (int)edgeList.size());

    std::vector<EPA_Face> facesToRemove;

    // Find every face that can see the point p, and add their edges to the list.
    for (auto face = s->faces.begin(); face != s->faces.end(); ++face) {
        if (Vector::dot(face->normal, p - face->v1) < 0) {
            //can be seen
            EPA_Face seenFace = *face;
            //s->faces.remove(seenFace);
            facesToRemove.push_back(seenFace);
            addFaceToList(&edgeList, seenFace);
        }
    }

    printf("Edges: %d\n", (int)edgeList.size());

    //remove faces that were seen
    for (int i = 0; i < facesToRemove.size(); i++) {
        s->faces.remove(facesToRemove[i]);
    }

    //make edge lsit unique
    edgeList = makeUnique(edgeList);
    printf("Edges: %d\n", (int)edgeList.size());
    //Construct new faces from edge list
    for (int i = 0; i < edgeList.size(); i++) {
        s->faces.push_front(createFaceFromVerts(edgeList[i].v1, edgeList[i].v1, p));
    }
}

std::vector<EPA_Edge> makeUnique(std::vector<EPA_Edge> list) {
    std::vector<EPA_Edge> newList;
    int numEdges = list.size();
    for (int n1 = 0; n1 < list.size(); n1++) {
        for (int n2 = n1+1; n2 < list.size(); n2++) {
            EPA_Edge e1 = list[n1];
            EPA_Edge e2 = list[n2];

            if ((e1 == e2)) {
                //do not keep
                list[n1].keep = false;
                list[n2].keep = false;
            }
        }
    }

    for (int n = 0; n < list.size(); n++) {
        if (list[n].keep) {
            newList.push_back(list[n]);
        }
    }

    return newList;
}

void addFaceToList(std::vector<EPA_Edge>* list, EPA_Face f) {
    vec3 a = f.v1;
    vec3 b = f.v2;
    vec3 c = f.v3;

    EPA_Edge e1 {a, b};
    EPA_Edge e2 {b, c};
    EPA_Edge e3 {c, a};

    list->push_back(e1);
    list->push_back(e2);
    list->push_back(e3);
}

EPA_Face ripClosestFace(EPA_Struct* s) {
    float closestDist = s->faces.front().distance;
    std::forward_list<EPA_Face>::iterator closest;
    for (auto face = s->faces.begin(); face != s->faces.end(); ++face) {
        if (face->distance <= closestDist) {
            closest = face;
            closestDist = closest->distance;
        }
    }

    EPA_Face closestVal = *closest;

    //Remove closest face from list
    s->faces.remove(closestVal);

    return closestVal;
}

EPA_Face createFaceFromVerts(vec3 a, vec3 b, vec3 c) {
    static int currentID = 1; //tracks the number of faces, and gives each a unique ID
    EPA_Face face;
    face.v1 = a;
    face.v2 = b;
    face.v3 = c;
    face.normal = Vector::normalized(Vector::cross(b - a, c - a));

    if (Vector::dot(face.normal, face.v1) < 0) {
        face.normal = -face.normal;
    }
    face.distance = Vector::dot(face.normal, face.v1);
    face.ID = currentID++;
    return face;
}

void printCurrentsimplex(EPA_Struct* s) {
    int num = 1;
    for (auto face = s->faces.begin(); face != s->faces.end(); ++face) {
        printf("Simplex face %d [%d]:\n", num++, face->ID);
        //printf(" distance: %.3f, ", face->distance);
        //face->normal.print("normal: ");
    }
}

bool EPA_Face::operator==(EPA_Face f) {
    return this->ID == f.ID;
}

bool EPA_Edge::operator==(EPA_Edge e) {
    float m1 = Vector::magnitude(v1 - e.v1);
    float m2 = Vector::magnitude(v2 - e.v2);
    float m3 = Vector::magnitude(v1 - e.v2);
    float m4 = Vector::magnitude(v2 - e.v1);

    float tol = .01f;
    if (((m1 < tol) && (m2 < tol)) || ((m3 < tol) && (m4 < tol))) {
        return true;
    } else {
        return false;
    }
}

/*
#include "EPA.h"
#include <assert.h>

int epa_seed(EPA_Struct* res, GJK_Struct* seed, EPA_Face* face) {
    if (!seed->hit) {
        printf("Cannot perform EPA algorithm on a non-intersecting simplex.\n");
        return 0;
    }

    printf("GJK Simplex size: %d\n", seed->cnt);
    if (seed->cnt != 4) {
        printf("Exiting EPA algo\n");
        return 0;
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
    face->normal.print("Closest face normal: ");

    return 1;
}

int epa_iteration(EPA_Struct* res, GJK_SupportPoint p, EPA_Face* face) {
    float distanceFromPlane = fabs(Vector::dot(face->normal, p.P - face->center));
    printf("Distance From Plane: %e\n", distanceFromPlane);
    if (distanceFromPlane < .25) {
        //converged to final solution
        res->contactPoint = p;
        res->penetrationNormal = face->normal;
        res->penetrationDepth = face->distance;

        p.P.print("---\nP = ");
        p.a.print("a = ");
        p.b.print("b = ");
        face->normal.print("normal: ");
        face->center.print("Center: ");
        return 0;
    }

    createNewSimplex(&res->simplex, *face, p.P);

    res->iteration++;

    *face = getClosestFace(&res->simplex);

    return 1;
}

void epa_processResults(EPA_Struct* epa_res, GJK_Struct* gjk_res) {
    gjk_res->P0 = epa_res->contactPoint.a;
    gjk_res->P1 = epa_res->contactPoint.b;
    vec3 d = epa_res->contactPoint.P;
    gjk_res->distance = sqrt(Vector::dot(d,d));
}


void createNewSimplex(EPA_Simplex* simplex, EPA_Face face, vec3 P) {
    EPA_Edge edgeList[400];
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
*/
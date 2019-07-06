#include "GJK.h"
#include <assert.h>

/**
 * Collider and GJK code based on (basically copied) kevinmoran on github
 * https://github.com/kevinmoran/GJK
 * modified to integrate into this project 
 */

bool gjk(Collider* col1, Collider* col2, vec3* mtv) {
    vec3 a, b, c, d; // Simplex verts -> a is always most recent
    vec3 search_dir = col1->position - col2->position; // Initial search direction

    // Get initial simplex point
    c = col2->support(search_dir) - col1->support(-search_dir);
    search_dir = -c;

    // Get second point
    b = col2->support(search_dir) - col1->support(-search_dir);

    if (Vector::dot(b, search_dir) < 0) { return false; } //didnt reach the origin, won't enclose it

    // search perpendicular to line segment towards origin
    search_dir = Vector::cross(Vector::cross(c-b,-b), c-b);
    if (Vector::isZero(search_dir)) { //origin on line segment
        // ? choose a normal search vector?
        search_dir = Vector::cross(c-b,{1,0,0}); // ? normal with x-axis
        if (Vector::isZero(search_dir)) search_dir = Vector::cross(c-b,{0,0,-1}); // ? normal with z-axis
    }
    int simp_dim = 2; //2-simplex

    for (int iterations = 0; iterations < GJK_MAX_ITERATIONS; iterations++) {
        a = col2->support(search_dir) - col1->support(-search_dir);
        if (Vector::dot(a, search_dir)<0) {return false;} //won't enclose the origin

        simp_dim++;
        if (simp_dim == 3) {
            update_simplex3(a,b,c,d,simp_dim,search_dir);
        } else if (update_simplex4(a,b,c,d,simp_dim,search_dir)) {
            if (mtv) *mtv = EPA(a,b,c,d,col1,col2);
            return true;
        }
    } // ! failed to converge in max number of iterations?
    return false;
}

// Triangle simplex
void update_simplex3(vec3 &a, vec3 &b, vec3 &c, vec3 &d, int &dim, vec3 &search_dir) {
    vec3 n = Vector::cross(b-a, c-a);
    vec3 AO = -a;

    dim = 2;
    if (Vector::dot(Vector::cross(b-a, n), AO)>0) {
        /* Take Edge AB */
        c = a;
        search_dir = Vector::cross(Vector::cross(b-a, AO), b-a);
        return;
    }
    if (Vector::dot(Vector::cross(n, c-a), AO)>0) {
        /* Take Edge AC */
        b = a;
        search_dir = Vector::cross(Vector::cross(c-a, AO), c-a);
        return;
    }

    dim = 3;
    /* Take triangle ABC */
    if (Vector::dot(n, AO)>0) {
        /* Above triangle */
        d = c;
        c = b;
        b = a;
        search_dir = n;
        return;
    }

    /* Below triangle */
    d = b;
    b = a;
    search_dir = -n;
    return;
}

// Tetrahedron simplex
bool update_simplex4(vec3 &a, vec3 &b, vec3 &c, vec3 &d, int &simp_dim, vec3 &search_dir){
    // Get normals of three faces
    vec3 ABC = Vector::cross(b-a,c-a);
    vec3 ACD = Vector::cross(c-a,d-a);
    vec3 ADB = Vector::cross(d-a,b-a);

    vec3 AO = -a;
    simp_dim = 3;

    if (Vector::dot(ABC, AO)>0) {
        d = c;
        c = b;
        b = a;
        search_dir = ABC;
        return false;
    }
    if (Vector::dot(ACD, AO)>0) {
        b = a;
        search_dir = ACD;
        return false;
    }
    if (Vector::dot(ADB, AO)>0) {
        c = d;
        d = b;
        b = a;
        search_dir = ADB;
        return false;
    }

    return true;
}

// EPA Algorithm
#define EPA_TOLERANCE 0.01
#define EPA_MAX_NUM_FACES 128
#define EPA_MAX_NUM_LOOSE_EDGES 32
#define EPA_MAX_NUM_ITERATIONS 64
vec3 EPA(vec3 a, vec3 b, vec3 c, vec3 d, Collider* col1, Collider* col2) {
    vec3 faces[EPA_MAX_NUM_FACES][4]; //Array of faces, each with 3 verts and a normal

    //Init with final simplex from GJK
    faces[0][0] = a;
    faces[0][1] = b;
    faces[0][2] = c;
    faces[0][3] = Vector::normalized(Vector::cross(b-a, c-a)); //ABC
    faces[1][0] = a;
    faces[1][1] = c;
    faces[1][2] = d;
    faces[1][3] = Vector::normalized(Vector::cross(c-a, d-a)); //ACD
    faces[2][0] = a;
    faces[2][1] = d;
    faces[2][2] = b;
    faces[2][3] = Vector::normalized(Vector::cross(d-a, b-a)); //ADB
    faces[3][0] = b;
    faces[3][1] = d;
    faces[3][2] = c;
    faces[3][3] = Vector::normalized(Vector::cross(d-b, c-b)); //BDC

    int num_faces = 4;
    int closest_face;

    for (int iterations = 0; iterations < EPA_MAX_NUM_ITERATIONS; iterations++) {
        // Find closest face
        float min_dist = Vector::dot(faces[0][0], faces[0][3]);
        closest_face = 0;
        for (int i = 1; i < num_faces; i++) {
            float dist = Vector::dot(faces[i][0], faces[i][3]);
            if (dist < min_dist) {
                min_dist = dist;
                closest_face = i;
            }
        }

        // Search normal to closest face
        vec3 search_dir = faces[closest_face][3];
        vec3 p = col2->support(search_dir) - col1->support(-search_dir);

        if (Vector::dot(p, search_dir)-min_dist < EPA_TOLERANCE) {
            // solution has converged
            return faces[closest_face][3]*Vector::dot(p, search_dir);
        }

        vec3 loose_edges[EPA_MAX_NUM_LOOSE_EDGES][2]; //edges we need to fix after ripping a face
        int num_loose_edges = 0;

        // Find all tris that can 'see' p
        for (int i = 0; i < num_faces; i++) {
            if (Vector::dot(faces[i][3], p-faces[i][0]) >0) { //triangle i can see p, remove it
                //add renived truabgkes edges to loose edge list
                // if its already there, remove it
                for (int j = 0; j < 3; j++) {
                    vec3 current_edge[2] = {faces[i][j], faces[i][(j+1)%3]};
                    bool found_edge = false;
                    for (int k = 0; k < num_loose_edges; k++) {
                        if (Vector::isZero(loose_edges[k][1] - current_edge[0]) &&
                            Vector::isZero(loose_edges[k][0] - current_edge[1])) {
                            loose_edges[k][0] = loose_edges[num_loose_edges-1][0];
                            loose_edges[k][1] = loose_edges[num_loose_edges-1][1];
                            num_loose_edges--;
                            found_edge = true;
                            k = num_loose_edges; //exit loop early
                        }
                    }//endfor loose_edges

                    if (!found_edge) {
                        assert(num_loose_edges < EPA_MAX_NUM_LOOSE_EDGES);
                        if (num_loose_edges >= EPA_MAX_NUM_LOOSE_EDGES) break;
                        loose_edges[num_loose_edges][0] = current_edge[0];
                        loose_edges[num_loose_edges][1] = current_edge[1];
                        num_loose_edges++;
                    }
                }
                
                // remove triangle i from list
                faces[i][0] = faces[num_faces-1][0];
                faces[i][1] = faces[num_faces-1][1];
                faces[i][2] = faces[num_faces-1][2];
                faces[i][3] = faces[num_faces-1][3];
                num_faces--;
                i--;
            }//endid p can see triangle i
        }//endfor num_faces

        //Reconstruct polytope with p added
        for (int i = 0; i < num_loose_edges; i++) {
            if (num_faces >= EPA_MAX_NUM_FACES) {
                printf("num_faces: %d\n", num_faces);
                for (int cface = 0; cface < num_faces; cface++) {
                    printf("%d: %f\n", cface, Vector::dot(faces[cface][3], faces[cface][0]));
                }
            }
            assert(num_faces < EPA_MAX_NUM_FACES);
            if (num_faces >= EPA_MAX_NUM_FACES) break;
            faces[num_faces][0] = loose_edges[i][0];
            faces[num_faces][1] = loose_edges[i][1];
            faces[num_faces][2] = p;
            faces[num_faces][3] = Vector::normalized(Vector::cross(loose_edges[i][0] - loose_edges[i][1], loose_edges[i][0]-p));

            //check for wrong normal to maintain CCW winding
            float bias = .000001;
            if (Vector::dot(faces[num_faces][0], faces[num_faces][3]) + bias < 0) {
                vec3 temp = faces[num_faces][0];
                faces[num_faces][0] = faces[num_faces][1];
                faces[num_faces][1] = temp;
                faces[num_faces][3] = -faces[num_faces][3];
            }
            num_faces++;
        }
    } //Endfor iterations
    printf("EPA did not converge\n");
    return faces[closest_face][3] * Vector::dot(faces[closest_face][0], faces[closest_face][3]);
}
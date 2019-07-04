#include "Collider.h"
#include <assert.h>

/* Oriented Bounding Box */
vec3 OBBCol::support(vec3 search_dir) {
    search_dir = Matrix::transformVector(&matRS_inverse, &search_dir);

    vec3 result;
    result.x = (search_dir.x>0) ? max.x : min.x;
    result.y = (search_dir.y>0) ? max.y : min.y;
    result.z = (search_dir.z>0) ? max.z : min.z;

    return Matrix::transformVector(&matRS, &result) + position;
}
void OBBCol::cleanup() {}

/* Bounding Sphere */
vec3 SphereCol::support(vec3 search_dir) {
    return Vector::normalized(search_dir)*r + position;
}
void SphereCol::cleanup() {}

/* Cylinder */
vec3 CylinderCol::support(vec3 search_dir) {
    search_dir = Matrix::transformVector(&matRS_inverse, &search_dir);

    vec3 dir_xz = {search_dir.x, 0, search_dir.z};
    vec3 result = Vector::normalized(dir_xz)*r;
    result.y = (search_dir.y>0) ? y_cap : y_base;

    return Matrix::transformVector(&matRS, &result) + position;
}
void CylinderCol::cleanup() {}

/* Capsule */
vec3 CapsuleCol::support(vec3 search_dir) {
    search_dir = Matrix::transformVector(&matRS_inverse, &search_dir);

    vec3 result = Vector::normalized(search_dir)*r;
    result.y += (search_dir.y>0) ? y_cap : y_base;

    return Matrix::transformVector(&matRS, &result) + position;
}
void CapsuleCol::cleanup() {}

/* Polytope */
vec3 PolyCol::support(vec3 search_dir) {
    search_dir = Matrix::transformVector(&matRS_inverse, &search_dir);

    assert(points);
    vec3 furthest_point = points[0];
    float max_dot = Vector::dot(furthest_point, search_dir);

    for (int i = 1; i < num_points; i++) {
        vec3 v = points[i];
        float d = Vector::dot(v, search_dir);
        if (d > max_dot) {
            max_dot = d;
            furthest_point = v;
        }
    }
    return Matrix::transformVector(&matRS, &furthest_point) + position;
}
void PolyCol::cleanup() {
    if (points) {
        free(points);
        num_points = 0;
    }
}
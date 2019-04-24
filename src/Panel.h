#include "Vector.h"
#include "Quaternion.h"

#include "Entity.h"

#define realmax 9999

inline float min(float a, float b) {
    return a < b ? a : b;
}

inline float max(float a, float b) {
    return a > b ? a : b;
}

struct PanelEnt : EntityBase {
    float d = realmax;
    float length = 10; //Panel size from edge to edge
    float width = 14;
    bool inVolume = false;
    float attachRadius2 = pow(1.5f, 2.0f);

    vec3 u;
    float dx, dz, K2 = realmax;

    void distanceToPoint(vec3 Py) {
        vec3 r = Py - position;
        d = abs(Vector::dot(r, Up));

        vec3 pRedDot = Py - Up*d;
        vec3 u_worldSpace = pRedDot - position;
        quat invOrientation = 
            {-orientation.x, -orientation.y, -orientation.z, orientation.w};
        u = Quaternion::transformVector(invOrientation, u_worldSpace);
        
        dx = max(abs(u.x), length/2.0f) - length/2.0f;
        dz = max(abs(u.z), width/2.0f)  - width/2.0f;
        K2 = dx*dx + dz*dz + d*d;
        inVolume = (K2 < attachRadius2);
    }
};
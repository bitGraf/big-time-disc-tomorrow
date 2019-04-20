#include "Vector.h"
#include "Quaternion.h"

#include "Entity.h"

#define realmax 9999

struct PanelEnt : EntityBase {
    float d = realmax;
    float distanceToPoint(vec3 Py) {
        vec3 r = Py - position;
        d = abs(Vector::dot(r, Up));

        return d;
    }
};
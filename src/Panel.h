#include "Vector.h"
#include "Quaternion.h"

#include "Entity.h"

#define realmax 9999

struct PanelEnt : EntityBase {
    float d = realmax;
    float length = 5;
    float width = 5;
    bool inSector = false;

    float distanceToPoint(vec3 Py) {
        vec3 r = Py - position;
        d = abs(Vector::dot(r, Up));

        vec3 pRedDot = Py - Up*d;
        vec3 u_worldSpace = pRedDot - position;
        quat invOrientation = 
            {-orientation.x, -orientation.y, -orientation.z, orientation.w};
        vec3 u = Quaternion::transformVector(invOrientation, u_worldSpace);

        if (abs(u.x) > length || abs(u.z) > width) {
            //The red dot is not inside the panel.
            inSector = false;
        } else {
            inSector = true;
        }

        return d;
    }
};
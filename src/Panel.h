#include "Vector.h"
#include "Quaternion.h"

struct Panel {
    vec3 position;
    quat orientation;

    vec3 Left, Up, Forward;

    void update() {
        float C11 = 1 - 2*orientation.y*orientation.y - 2*orientation.z*orientation.z;
        float C12 = 2*(orientation.x*orientation.y - orientation.z*orientation.w);
        float C13 = 2*(orientation.z*orientation.x + orientation.y*orientation.w);
        float C21 = 2*(orientation.x*orientation.y + orientation.z*orientation.w);
        float C22 = 1 - 2*orientation.z*orientation.z - 2*orientation.x*orientation.x;
        float C23 = 2*(orientation.y*orientation.z - orientation.x*orientation.w);
        float C31 = 2*(orientation.z*orientation.x - orientation.y*orientation.w);
        float C32 = 2*(orientation.y*orientation.z + orientation.x*orientation.w);
        float C33 = 1 - 2*orientation.x*orientation.x - 2*orientation.y*orientation.y;
        
        Left    = {C11, C12, -C13};
        Up      = {C21, C22, C23};
        Forward = {-C31, C32, C33};
    }

    float distanceToPoint(vec3 Py) {
        vec3 r = Py - position;
        float d = Vector::dot(r, Up);

        return abs(d);
    }
};
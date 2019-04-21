#pragma once

#include "stdio.h"
#define _USE_MATH_DEFINES
#include <math.h>

#include "Vector.h"

struct quat {
    float x = 0;    //Axis component
    float y = 0;
    float z = 0;
    float w = 1;    //Angle component

    quat& operator=(const quat q) {
        x = q.x;
        y = q.y;
        z = q.z;
        w = q.w;
        return *this;
    }
    quat operator+(const quat& q) const {
        return {x+q.x, y+q.y, z+q.z, w+q.w};
    }
    quat operator*(const quat& q) const {
        //Successive quaternion rotation
        float e1 =  w*q.x - z*q.y + y*q.z + x*q.w;
        float e2 =  z*q.x + w*q.y - x*q.z + y*q.w;;
        float e3 = -y*q.x + x*q.y + w*q.z + z*q.w;;
        float e4 = -x*q.x - y*q.y - z*q.z + w*q.w;;

        return {
            e1, e2, e3, e4
        };
    }

    void print(char* prepend = NULL) {
        if (prepend != NULL)
            printf("%s", prepend);
        printf("[%.2f, %.2f, %.2f, %.2f]\n", x, y, z, w);
    }
};

namespace Quaternion {
    void identity(quat& q);
    void normalize(quat& q);
    void buildFromAxisAngleD(quat& q, const vec3 axis, const float angleD);
    void buildFromAxisAngle(quat& q, const vec3 axis, const float angle);

    quat mul(quat q1, quat q2);
    quat succ(quat q1, quat q2);

    vec3 transformVector(quat, vec3);

    quat lerp(quat start, quat end, float progress);
}
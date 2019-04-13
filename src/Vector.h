#pragma once

#include "stdio.h"

struct vec3 {
    float x = 0;
    float y = 0;
    float z = 0;

    vec3& operator=(const vec3 v) {
        x = v.x;
        y = v.y;
        z = v.z;
        return *this;
    }
    vec3 operator+(const vec3& v) const {
        return {x+v.x, y+v.y, z+v.z};
    }
    vec3 operator-(const vec3& v) const {
        return {x-v.x, y-v.y, z-v.z};
    }
    vec3 operator-() const {
        return {-x, -y, -z};
    }
    vec3 operator*(const float& f) const {
        return {
            x*f, y*f, z*f
        };
    }
    void print(char* prepend = NULL) {
        if (prepend != NULL)
            printf(prepend);
        printf("(%8.4f, %8.4f, %8.4f)\n", x, y, z);
    }
};

namespace Vector {
    float dot(vec3 a, vec3 b);
    vec3 cross(vec3& a, vec3& b);

    vec3 normalized(vec3& v);
    void normalize(vec3& v);
}
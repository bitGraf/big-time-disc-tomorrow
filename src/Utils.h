#pragma once

#include "glad/glad.h"
#define _USE_MATH_DEFINES
#include <math.h>

struct TimeInfo {
    double deltaTime = 0;
    double lastFrameEnd = 0;
    double frameStart = 0;
    double lag = 0;
};

struct WindowInfo {
    GLuint width = 1280;        //Window size (also render target size)
    GLuint height = 960;
    float lastX = width / 2;    //Cursor position
    float lastY = height / 2;
};

inline double deg2Rad(double deg) {
    return deg * M_PI / 180.0;
}
inline double rad2deg(double rad) {
    return rad * 180.0 / M_PI;
}

inline float randomFloat(float min, float max) {
    return ((max - min) * ((float)rand() / RAND_MAX)) + min;
}
#pragma once

#include "GLFW/glfw3.h"
#include "Vector.h"
#include "Utils.h"
#include "Matrix.h"

#include <math.h>

static vec3 worldUp = {0, 1, 0};

struct Camera {
    vec3 targetPosition = {5, 5, -5};
    vec3 position = {60, 60, -60};
    quat targetOrientation = {0, 0.70710678f, 0, 0.70710678f}; // look along -Z direction
    quat orientation = {0, 0, 0, 1};
    float rate = 12.0f;

    vec3 Forward;
    vec3 Left;
    vec3 Up;

    mat4 viewMatrix;
    mat4 projectionMatrix;


    //Projection matrix settings
    float currentFov = 70;
    float clipNear = .01;
    float clipFar  = 1000;

    void lookAt(vec3 location) {
        //DOES NOTHING
        //vec3 F = Vector::normalized(location - position);

        //pitch = -rad2deg(asin(F.y));
        //yaw = rad2deg(atan2(-F.z, F.x));
    }

    void update(double dt) {
        //Forward is the driving vector (set by yaw/pitch)

        //orientation = targetOrientation;
        orientation = Quaternion::lerp(orientation, targetOrientation, rate * dt);
        position = Vector::lerp(position, targetPosition, rate * dt);

        Matrix::buildFromTRSInv(&viewMatrix, position, orientation);

        float C11 = 1 - 2*orientation.y*orientation.y - 2*orientation.z*orientation.z;
        float C12 = 2*(orientation.x*orientation.y - orientation.z*orientation.w);
        float C13 = 2*(orientation.z*orientation.x + orientation.y*orientation.w);
        float C21 = 2*(orientation.x*orientation.y + orientation.z*orientation.w);
        float C22 = 1 - 2*orientation.z*orientation.z - 2*orientation.x*orientation.x;
        float C23 = 2*(orientation.y*orientation.z - orientation.x*orientation.w);
        float C31 = 2*(orientation.z*orientation.x - orientation.y*orientation.w);
        float C32 = 2*(orientation.y*orientation.z + orientation.x*orientation.w);
        float C33 = 1 - 2*orientation.x*orientation.x - 2*orientation.y*orientation.y;
        
        Left    = {-C11, -C21, -C31};
        Up      = {-C12, -C22, -C32};
        Forward = {-C13, -C23, -C33};
    }

    void updateOrientation(float dYaw, float dPitch) {
        quat rotYaw, rotPitch;

        Quaternion::buildFromAxisAngle(rotYaw, {0, 1, 0}, dYaw);

        orientation = Quaternion::mul(orientation, rotYaw);
    }

    //void buildProjectionMatrix(mat4* m, float fov, float ratio, float znear, float zfar);
    void updateProjectionMatrix(WindowInfo win) {
        float ratio = (float)win.width / (float)win.height;
        Matrix::buildProjectionMatrix(&projectionMatrix, currentFov, ratio,
            clipNear, clipFar);
    }

    void handleInput(int key, int scancode, int action, int mods) {
            if (key == GLFW_KEY_F && action == GLFW_PRESS) {
                Forward.print("Forward: ");
            }
            if (key == GLFW_KEY_U && action == GLFW_PRESS) {
                Up.print     ("Up:      ");
            }
            if (key == GLFW_KEY_R && action == GLFW_PRESS) {
                Left.print     ("Left:   ");
            }
    }
};
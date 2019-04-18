#pragma once

#include "GLFW/glfw3.h"
#include "Vector.h"
#include "Utils.h"
#include "Matrix.h"

#include <math.h>

static vec3 worldUp = {0, 1, 0};

struct Camera {
    float yaw = 90;    //Set these three vars to drive 
    float pitch = 0;    //the position/orientation of the camera
    vec3 position;

    vec3 Forward;
    vec3 Right;
    vec3 Up;

    mat4 viewMatrix;
    mat4 projectionMatrix;


    //Projection matrix settings
    float currentFov = 70;
    float clipNear = .01;
    float clipFar  = 1000;

    void lookAt(vec3 location) {
        vec3 F = Vector::normalized(location - position);

        pitch = -rad2deg(asin(F.y));
        yaw = rad2deg(atan2(-F.z, F.x));
    }

    void updateVectors() {
        if (pitch > 89.0f)
			pitch = 89.0f;
		if (pitch < -89.0f)
			pitch = -89.0f;
        if (yaw > 360)
            yaw -= 360;
        if (yaw < 0)
            yaw += 360;

        //Forward is the driving vector (set by yaw/pitch)
        float pitchRad = -deg2Rad(pitch);
        float yawRad = -deg2Rad(yaw);
        Forward.x = cos(pitchRad) * cos(yawRad);
        Forward.y = sin(pitchRad);
        Forward.z = cos(pitchRad) * sin(yawRad);
        Vector::normalize(Forward);
        Right = Vector::normalized(Vector::cross(Forward, worldUp));
        Up = Vector::normalized(Vector::cross(Right, Forward));
    }

    void updateViewMatrix() {
        viewMatrix.a11 = Right.x;viewMatrix.a12 = Right.y;viewMatrix.a13 = Right.z;
        viewMatrix.a14 = -Vector::dot(Right, position);

        viewMatrix.a21 = Up.x;viewMatrix.a22 = Up.y;viewMatrix.a23 = Up.z;
        viewMatrix.a24 = -Vector::dot(Up, position);

        viewMatrix.a31 = -Forward.x;viewMatrix.a32 = -Forward.y;viewMatrix.a33 = -Forward.z;
        viewMatrix.a34 = Vector::dot(Forward, position);

        viewMatrix.a41 = 0;  viewMatrix.a42 = 0;  viewMatrix.a43 = 0;
        viewMatrix.a44 = 1;
    }

    void updateOrientation(float dYaw, float dPitch) {
        yaw += dYaw;
        pitch += dPitch;
        if (pitch > 89.0f)
			pitch = 89.0f;
		if (pitch < -89.0f)
			pitch = -89.0f;
        if (yaw > 360)
            yaw -= 360;
        if (yaw < 0)
            yaw += 360;
        
        updateVectors();
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
                Up.print	 ("Up:      ");
            }
            if (key == GLFW_KEY_R && action == GLFW_PRESS) {
                Right.print	 ("Right:   ");
            }
    }
};




/*
#include "Vector.h"
#include "Matrix.h"
#include "Quaternion.h"

vec3 WorldUp = {0,1,0};

struct Camera {
    vec3 position;
    quat orientation;

    mat4 viewMatrix;

    vec3 Front, Right, Up;

    void updateViewMatrix() {
        float e1 = orientation.x;
        float e2 = orientation.y;
        float e3 = orientation.z;
        float e4 = orientation.w;

        //orientation.print();

        float C11 = 1 - 2*e2*e2 - 2*e3*e3;
        float C12 = 2*(e1*e2 - e3*e4);
        float C13 = 2*(e3*e1 + e2*e4);
        float C21 = 2*(e1*e2 + e3*e4);
        float C22 = 1 - 2*e3*e3 - 2*e1*e1;
        float C23 = 2*(e2*e3 - e1*e4);
        float C31 = 2*(e3*e1 - e2*e4);
        float C32 = 2*(e2*e3 + e1*e4);
        float C33 = 1 - 2*e1*e1 - 2*e2*e2;

        //need to update Right, Forward, Up
        Right = {  C11,  C21,  C31};
        Up    = {  C12,  C22,  C32};
        Front = { -C13, -C23, -C33};
        //Right = {  C11,  C12,  C13};
        //Up    = {  C21,  C22,  C23};
        //Front = { -C31, -C32, -C33};

        viewMatrix.a11 = C11;viewMatrix.a12 = C21;viewMatrix.a13 = C31;
        viewMatrix.a14 = -(C11*position.x + C21*position.y + C31*position.z);

        viewMatrix.a21 = C12;viewMatrix.a22 = C22;viewMatrix.a23 = C32;
        viewMatrix.a24 = -(C12*position.x + C22*position.y + C32*position.z);

        viewMatrix.a31 = C13;viewMatrix.a32 = C23;viewMatrix.a33 = C33;
        viewMatrix.a34 = -(C13*position.x + C23*position.y + C33*position.z);
        
        viewMatrix.a41 = 0;  viewMatrix.a42 = 0;  viewMatrix.a43 = 0;  
        viewMatrix.a44 = 1;
    }

    void updateVectorsFromOrientation() {
        float e1 = orientation.x;
        float e2 = orientation.y;
        float e3 = orientation.z;
        float e4 = orientation.w;

        //orientation.print();

        float C11 = 1 - 2*e2*e2 - 2*e3*e3;
        float C12 = 2*(e1*e2 - e3*e4);
        float C13 = 2*(e3*e1 + e2*e4);
        float C21 = 2*(e1*e2 + e3*e4);
        float C22 = 1 - 2*e3*e3 - 2*e1*e1;
        float C23 = 2*(e2*e3 - e1*e4);
        float C31 = 2*(e3*e1 - e2*e4);
        float C32 = 2*(e2*e3 + e1*e4);
        float C33 = 1 - 2*e1*e1 - 2*e2*e2;

        //need to update Right, Forward, Up
        Right = {  C11,  C21,  C31};
        Up    = {  C12,  C22,  C32};
        Front = { -C13, -C23, -C33};
        //Right = {  C11,  C12,  C13};
        //Up    = {  C21,  C22,  C23};
        //Front = { -C31, -C32, -C33};
    }

    void lookAt(vec3 pos, vec3 target, vec3 up) {
        lookInDir(Vector::normalized(target - pos), Vector::normalized(up));
        position = pos;
    }

    void lookInDir(vec3 &lookDir, vec3 &upDir) {
        //lookDir.print();
        //upDir.print();
        vec3 forward = lookDir;
        //vec3 up = upDir;
	    vec3 right = Vector::normalized(Vector::cross(forward, upDir));
        vec3 up = Vector::normalized(Vector::cross(right, forward));

        //forward.print("Forward: ");
        Front = forward;
        //right.print(  "Right:   ");
        Right = right;
        //up.print(     "Up:      ");
        Up = up;

        //Matrix::identity(&viewMatrix);

        

        orientation.w = sqrtf(1.0f + right.x + up.y + forward.z) * 0.5f;
        float w4_recip = 1.0f / (4.0f * orientation.w);
        orientation.x = (up.z - forward.y) * w4_recip;
        orientation.y = (forward.x - right.z) * w4_recip;
        orientation.z = (right.y - up.x) * w4_recip;
        Quaternion::normalize(orientation);
        //orientation.print("quat:");
    }

    void updateOrientation(float dYaw, float dPitch) {
        //create two succ rotations, first the yaw, then the pitch.
        //yaw axis is WorldUp, pitch axis is BodyRight. Angles are dYaw and dPitch.
        //new orientation = orientation * yawRot * pitchRot;
        quat yawRot, pitchRot;

        Quaternion::buildFromAxisAngleD(yawRot, Up, dYaw);
        //orientation = orientation * yawRot;
        Quaternion::buildFromAxisAngleD(pitchRot, {1, 0, 0}, dPitch);
        orientation = orientation * (yawRot * pitchRot);
        //updateViewMatrix();
    }
};
*/
#include "Vector.h"
#include "Matrix.h"
#include "Quaternion.h"

float Vector::dot(const vec3 a, const vec3 b) {
    return ((a.x*b.x) + (a.y*b.y) + (a.z*b.z));
}

vec3 Vector::cross(const vec3& a, const vec3& b) {
    return {
        a.y*b.z - a.z*b.y,
        a.z*b.x - a.x*b.z,
        a.x*b.y - a.y*b.x
    };
}

vec3 Vector::normalized(const vec3& v) {
    float m = 1/sqrt(v.x*v.x + v.y*v.y + v.z*v.z);
    return {v.x*m, v.y*m, v.z*m};
}

void Vector::normalize(vec3& v) {
    float m = 1/sqrt(v.x*v.x + v.y*v.y + v.z*v.z);
    v = (v * m);
}

void Matrix::zero(mat4* m) {
    m->a11=0; m->a12=0; m->a13=0; m->a14=0;
    m->a21=0; m->a22=0; m->a23=0; m->a24=0;
    m->a31=0; m->a32=0; m->a33=0; m->a34=0;
    m->a41=0; m->a42=0; m->a43=0; m->a44=0;}
void Matrix::identity(mat4* m) {
    m->a11=1; m->a12=0; m->a13=0; m->a14=0;
    m->a21=0; m->a22=1; m->a23=0; m->a24=0;
    m->a31=0; m->a32=0; m->a33=1; m->a34=0;
    m->a41=0; m->a42=0; m->a43=0; m->a44=1;}

void Matrix::buildFromTRS(mat4* m, vec3 T, quat R, vec3 S) {
    //printf("Building mat4 from:\n\tTranslation:(%.2f, %.2f, %.2f)\n\tQuaternion:[%.2f %.2f %.2f %.2f]\n\tScale:(%.2f, %.2f, %.2f)\n", 
    //    T.x, T.y, T.z, R.x, R.y, R.z, R.w, S.x, S.y, S.z);
    Matrix::fromQuaternion(m, &R);

    m->a14=T.x;
    m->a24=T.y;
    m->a34=T.z;

    m->a11*=S.x; m->a12*=S.y; m->a13*=S.z;
    m->a21*=S.x; m->a22*=S.y; m->a23*=S.z;
    m->a31*=S.x; m->a32*=S.y; m->a33*=S.z;

    m->a41 = 0;m->a42 = 0;m->a43 = 0;m->a44 = 1;
}

void Matrix::buildProjectionMatrix(mat4* m, float fov, float ratio, float znear, float zfar) {
    //printf("Building projection matrix:\n\tfov: %.2f\n\tratio: %.2f\n\tzNear: %.2f\n\tzFar: %.2f\n",
        //fov, ratio, znear, zfar);
    zero(m);

    float tanHalf = tan(fov * 3.14159265359 / 360);

    m->a11 = 1 / (ratio * tanHalf);
    m->a22 = 1 / tanHalf;
    m->a33 = -(zfar+znear)/(zfar-znear);
    m->a43 = -1;
    m->a34 = -(2*zfar*znear) / (zfar-znear);
}

void Matrix::buildOrthoProjectionMatrix(mat4* m, float left, float right, float bottom, float top, float znear, float zfar) {
    //printf("Building projection matrix:\n\tfov: %.2f\n\tratio: %.2f\n\tzNear: %.2f\n\tzFar: %.2f\n",
        //fov, ratio, znear, zfar);
    zero(m);

    m->a11 = 2 / (right - left);
    m->a22 = 2 / (top - bottom);
    m->a33 = -2/(zfar-znear);
    m->a44 = 1;

    m->a14 = -(right + left) / (right - left);
    m->a24 = -(top + bottom) / (top - bottom);
    m->a34 = -(zfar + znear) / (zfar-znear);
}

void Quaternion::identity(quat& q) {
    q.x = 0;
    q.y = 0;
    q.z = 0;
    q.w = 1;
}
void Quaternion::buildFromAxisAngleD(quat& q, const vec3 axis, const float angleD) {
    //printf("Building quat from axis:(%.2f, %.2f, %.2f), angle:%.2f deg\n", axis.x, axis.y, axis.z, angleD);
    float halfSin = sin((double)angleD * M_PI/360.0);
    float halfCos = cos((double)angleD * M_PI/360.0);

    q.x = axis.x * halfSin;
    q.y = axis.y * halfSin;
    q.z = axis.z * halfSin;
    q.w = halfCos;
}
void Quaternion::buildFromAxisAngle(quat& q, const vec3 axis, const float angle) {
    //printf("Building quat from axis:(%.2f, %.2f, %.2f), angle:%.2f rad\n", axis.x, axis.y, axis.z, angle);
    float halfSin = sin((double)angle/2.0);
    float halfCos = cos((double)angle/2.0);

    q.x = axis.x * halfSin;
    q.y = axis.y * halfSin;
    q.z = axis.z * halfSin;
    q.w = halfCos;
}

void Quaternion::normalize(quat& v) {
    float m = 1/sqrt(v.x*v.x + v.y*v.y + v.z*v.z + v.w*v.w);
    v.x *= m;
    v.y *= m;
    v.z *= m;
    v.w *= m;
}

void Matrix::fromQuaternion(mat4* m, quat* q) {
    //printf("Converting quat:[%.2f, %.2f, %.2f, %.2f] to mat4\n", e1, e2, e3, e4);
    Matrix::identity(m);

    //remove these unnessecary floats
    float e1 = q->x;
    float e2 = q->y;
    float e3 = q->z;
    float e4 = q->w;

    m->a11 = 1 - 2*(e2*e2) - 2*(e3*e3);
    m->a12 = 2*(e1*e2 - e3*e4);
    m->a13 = 2*(e3*e1 + e2*e4);

    m->a21 = 2*(e1*e2 + e3*e4);
    m->a22 = 1 - 2*(e3*e3) - 2*(e1*e1);
    m->a23 = 2*(e2*e3 - e1*e4);

    m->a31 = 2*(e3*e1 - e2*e4);
    m->a32 = 2*(e2*e3 + e1*e4);
    m->a33 = 1 - 2*(e1*e1) - 2*(e2*e2);
}

void Matrix::fromYaw(mat4* m, float x) {
    Matrix::identity(m);

    float cT = cos(x * M_PI/180);
    float sT = sin(x * M_PI/180);

    m->a11 = cT;
    m->a12 = 0;
    m->a13 = sT;

    m->a21 = 0;
    m->a22 = 1;
    m->a23 = 0;

    m->a31 = sT;
    m->a32 = 0;
    m->a33 = cT;
}

void Matrix::fromPitch(mat4* m, float x) {
    Matrix::identity(m);

    float cT = cos(x * M_PI/180);
    float sT = sin(x * M_PI/180);

    m->a11 = 1;
    m->a12 = 0;
    m->a13 = 0;

    m->a21 = 0;
    m->a22 = cT;
    m->a23 = -sT;

    m->a31 = 0;
    m->a32 = sT;
    m->a33 = cT;
}

void Matrix::fromRoll(mat4* m, float x) {
    Matrix::identity(m);

    float cT = cos(x * M_PI/180);
    float sT = sin(x * M_PI/180);

    m->a11 = cT;
    m->a12 = -sT;
    m->a13 = 0;

    m->a21 = sT;
    m->a22 = cT;
    m->a23 = 0;

    m->a31 = 0;
    m->a32 = 0;
    m->a33 = 1;
}
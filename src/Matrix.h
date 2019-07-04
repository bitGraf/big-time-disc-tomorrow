#pragma once

#include "Vector.h"
#include "Quaternion.h"

struct mat4 { //default to identity
    float a11=1, a21=0, a31=0, a41=0;
    float a12=0, a22=1, a32=0, a42=0;
    float a13=0, a23=0, a33=1, a43=0;
    float a14=0, a24=0, a34=0, a44=1;

    mat4& operator=(const mat4 m) {
        a11=m.a11;a12=m.a12;a13=m.a13;a14=m.a14;
        a21=m.a21;a22=m.a22;a23=m.a23;a24=m.a24;
        a31=m.a31;a32=m.a32;a33=m.a33;a34=m.a34;
        a41=m.a41;a42=m.a42;a43=m.a43;a44=m.a44;
        return *this;
    }
    mat4 operator+(const mat4& v) const {
        return {
            a11+v.a11,a12+v.a12,a13+v.a13,a14+v.a14,
            a21+v.a21,a22+v.a22,a23+v.a23,a24+v.a24,
            a31+v.a31,a32+v.a32,a33+v.a33,a34+v.a34,
            a41+v.a41,a42+v.a42,a43+v.a43,a44+v.a44
        };
    }
    mat4 operator*(const float& f) const {
        return {
            a11*f, a21*f, a31*f, a41*f,
            a12*f, a22*f, a32*f, a42*f,
            a13*f, a23*f, a33*f, a43*f,
            a14*f, a24*f, a34*f, a44*f
        };
    }
    mat4 operator*(const mat4& v) const {
        float c11 = a11*v.a11 + a12*v.a21 + a13*v.a31 + a14*v.a41;
        float c21 = a21*v.a11 + a22*v.a21 + a23*v.a31 + a24*v.a41;
        float c31 = a31*v.a11 + a32*v.a21 + a33*v.a31 + a34*v.a41;
        float c41 = a41*v.a11 + a42*v.a21 + a43*v.a31 + a44*v.a41;

        float c12 = a11*v.a12 + a12*v.a22 + a13*v.a32 + a14*v.a42;
        float c22 = a21*v.a12 + a22*v.a22 + a23*v.a32 + a24*v.a42;
        float c32 = a31*v.a12 + a32*v.a22 + a33*v.a32 + a34*v.a42;
        float c42 = a41*v.a12 + a42*v.a22 + a43*v.a32 + a44*v.a42;

        float c13 = a11*v.a13 + a12*v.a23 + a13*v.a33 + a14*v.a43;
        float c23 = a21*v.a13 + a22*v.a23 + a23*v.a33 + a24*v.a43;
        float c33 = a31*v.a13 + a32*v.a23 + a33*v.a33 + a34*v.a43;
        float c43 = a41*v.a13 + a42*v.a23 + a43*v.a33 + a44*v.a43;

        float c14 = a11*v.a14 + a12*v.a24 + a13*v.a34 + a14*v.a44;
        float c24 = a21*v.a14 + a22*v.a24 + a23*v.a34 + a24*v.a44;
        float c34 = a31*v.a14 + a32*v.a24 + a33*v.a34 + a34*v.a44;
        float c44 = a41*v.a14 + a42*v.a24 + a43*v.a34 + a44*v.a44;

        return {
            c11, c21, c31, c41,
            c12, c22, c32, c42,
            c13, c23, c33, c43,
            c14, c24, c34, c44
        };
    }
    vec3 operator*(const vec3& v) const {
        float W = 1;
        float c1 = a11*v.x + a12*v.y + a13*v.z + a14*W;
        float c2 = a21*v.x + a22*v.y + a23*v.z + a24*W;
        float c3 = a31*v.x + a32*v.y + a33*v.z + a34*W;

        return {
            c1, c2, c3
        };
    }

    void print(char* prepend = NULL) {
        if (prepend != NULL)
            printf("%s", prepend);
        printf( " /%.4f %.4f %.4f %.4f\\\n", a11, a12, a13, a14);
        printf( " |%.4f %.4f %.4f %.4f|\n",  a21, a22, a23, a24);
        printf( " |%.4f %.4f %.4f %.4f|\n",  a31, a32, a33, a34);
        printf(" \\%.4f %.4f %.4f %.4f/\n",  a41, a42, a43, a44);
    }
};

struct mat3 {
    float a11=1, a21=0, a31=0;
    float a12=0, a22=1, a32=0;
    float a13=0, a23=0, a33=1;

    mat3& operator=(const mat3 m) {
        a11=m.a11;a12=m.a12;a13=m.a13;
        a21=m.a21;a22=m.a22;a23=m.a23;
        a31=m.a31;a32=m.a32;a33=m.a33;
        return *this;
    }
    mat3 operator+(const mat3& v) const {
        return {
            a11+v.a11,a12+v.a12,a13+v.a13,
            a21+v.a21,a22+v.a22,a23+v.a23,
            a31+v.a31,a32+v.a32,a33+v.a33
        };
    }
    mat3 operator*(const float& f) const {
        return {
            a11*f, a21*f, a31*f,
            a12*f, a22*f, a32*f,
            a13*f, a23*f, a33*f
        };
    }
    mat3 operator*(const mat3& v) const {
        float c11 = a11*v.a11 + a12*v.a21 + a13*v.a31;
        float c21 = a21*v.a11 + a22*v.a21 + a23*v.a31;
        float c31 = a31*v.a11 + a32*v.a21 + a33*v.a31;

        float c12 = a11*v.a12 + a12*v.a22 + a13*v.a32;
        float c22 = a21*v.a12 + a22*v.a22 + a23*v.a32;
        float c32 = a31*v.a12 + a32*v.a22 + a33*v.a32;

        float c13 = a11*v.a13 + a12*v.a23 + a13*v.a33;
        float c23 = a21*v.a13 + a22*v.a23 + a23*v.a33;
        float c33 = a31*v.a13 + a32*v.a23 + a33*v.a33;

        return {
            c11, c21, c31,
            c12, c22, c32,
            c13, c23, c33
        };
    }
    vec3 operator*(const vec3& v) const {
        float c1 = a11*v.x + a12*v.y + a13*v.z;
        float c2 = a21*v.x + a22*v.y + a23*v.z;
        float c3 = a31*v.x + a32*v.y + a33*v.z;

        return {
            c1, c2, c3
        };
    }

    void print(char* prepend = NULL) {
        if (prepend != NULL)
            printf("%s", prepend);
        printf( " /%.2f %.2f %.2f\\\n", a11, a12, a13);
        printf( " |%.2f %.2f %.2f|\n",  a21, a22, a23);
        printf(" \\%.2f %.2f %.2f/\n",  a31, a32, a33);
    }
};

namespace Matrix {
    void zero(mat4* m);
    void identity(mat4* m);
    void buildFromTRS(mat4* m, vec3 T, quat R, vec3 S);
    void buildFromTRSInv(mat4* m, vec3 T, quat R);
    
    void buildProjectionMatrix(mat4* m, float fov, float ratio, float znear, float zfar);
    void buildOrthoProjectionMatrix(mat4* m, float left, float right, float bottom, float top, float znear, float zfar);
    
    void fromQuaternion(mat4* m, quat* q);

    void fromYaw(mat4*, float x);
    void fromPitch(mat4*, float x);
    void fromRoll(mat4*, float x);

    vec3 transformVector(mat3*, vec3*);
}
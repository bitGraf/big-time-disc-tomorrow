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
            printf("%s", prepend);
        printf("(%8.4f, %8.4f, %8.4f)\n", x, y, z);
    }
};

struct vec2 {
	float x = 0;
	float y = 0;

	vec2& operator=(const vec2 v) {
		x = v.x;
		y = v.y;
		return *this;
	}
	vec2 operator+(const vec2& v) const {
		return { x + v.x, y + v.y };
	}
	vec2 operator-(const vec2& v) const {
		return { x - v.x, y - v.y };
	}
	vec2 operator-() const {
		return { -x, -y };
	}
	vec2 operator*(const float& f) const {
		return {
			x*f, y*f
		};
	}
	void print(char* prepend = NULL) {
		if (prepend != NULL)
			printf("%s", prepend);
		printf("(%8.4f, %8.4f)\n", x, y);
	}
};

namespace Vector {
    float dot(const vec3 a, const vec3 b);
    vec3 cross(const vec3& a, const vec3& b);

    vec3 normalized(const vec3& v);
    void normalize(vec3& v);

    float magnitude(vec3 v);
	float magnitude(vec2 v);

    vec3 lerp(vec3 start, vec3 end, float progression);
}
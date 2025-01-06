#include "vector.h"
#include <math.h>

Vec3 add(Vec3 v1, Vec3 v2){
    Vec3 v3;
    v3.x = v1.x+v2.x;
    v3.y=v1.y+v2.y;
    v3.z=v1.z+v2.z;
    return v3;
}

Vec3 subtract(Vec3 v1, Vec3 v2){
    Vec3 v3;
    v3.x = v1.x-v2.x;
    v3.y=v1.y-v2.y;
    v3.z=v1.z-v2.z;
    return v3;
}

Vec3 scalarMultiply(float s, Vec3 v){
    Vec3 v3;
    v3.x = s*v.x;
    v3.y = s*v.y;
    v3.z = s*v.z;
    return v3;
}

Vec3 scalarDivide(Vec3 v, float d){
    Vec3 v3;
    v3.x = v.x/d;
    v3.y = v.y/d;
    v3.z = v.z/d;
    return v3;
}

Vec3 normalize(Vec3 v){
    float length;
    Vec3 v3;
    length = sqrt((v.x)*(v.x)+(v.y)*(v.y)+(v.z)*(v.z));
    v3.x = v.x/length;
    v3.y = v.y/length;
    v3.z = v.z/length;
    return v3;
}

float dot(Vec3 v1, Vec3 v2){
    float result;
    result = v1.x*v2.x+v1.y*v2.y+v1.z*v2.z;
    return result;
}

float length2(Vec3 v){
    float length2;
    length2 = (v.x)*(v.x)+(v.y)*(v.y)+(v.z)*(v.z);
    return length2;
}

float length(Vec3 v){
    float length;
    length = sqrt((v.x)*(v.x)+(v.y)*(v.y)+(v.z)*(v.z));
    return length;
}

float distance2(Vec3 v1, Vec3 v2){
    Vec3 v3 = subtract(v1, v2);
    return length2(v3);
}

float distance(Vec3 v1, Vec3 v2){
    Vec3 v3 = subtract(v1, v2);
    return length(v3);
}
# include "spheres.h"
# include <stdlib.h>
# include <math.h>

void worldInit(World *world) {
    world->size = 0;
    world->capacity = 1;
    world->spheres = (Sphere **)malloc(sizeof(Sphere *) * world->capacity);
}
void freeWorld(World *world) {
    for (int i = 0; i < world->size; i++) {
        free(world->spheres[i]); 
    }
    free(world->spheres);
    world->size = 0;
    world->capacity = 0;
}
void addSphere(World *world, Sphere *sphere) {
    if (world->size >= world->capacity) {
        world->capacity *= 2;
        Sphere **newArray = (Sphere **)realloc(world->spheres, sizeof(Sphere *) * world->capacity);
        world->spheres = newArray;
    }
    world->spheres[world->size] = sphere;
    world->size++;
}
Sphere *createSphere(float radius, Vec3 position, Vec3 color) {
    Sphere *sphere = (Sphere *)malloc(sizeof(Sphere));
    sphere->r = radius;
    sphere->pos = position;
    sphere->color = color;
    return sphere;
}
int doesIntersect(const Sphere *sphere, Vec3 rayPos, Vec3 rayDir, float *t){
    Vec3 spherePos = sphere->pos;
    float sphereRadius = sphere->r;
    Vec3 V = subtract(rayPos, spherePos);       // V = rayPos - spherePos
    float a = dot(rayDir, rayDir);              // a = rayDir ⋅ rayDir
    float b = 2 * dot(rayDir, V);               // b = 2 * rayDir ⋅ V
    float c = dot(V, V) - sphereRadius * sphereRadius; 
    float discriminant = b * b - 4 * a * c;
    if (discriminant < 0) {
        return 0;
    }
    float sqrtDiscriminant = sqrt(discriminant);
    float t1 = (-b - sqrtDiscriminant) / (2 * a);
    float t2 = (-b + sqrtDiscriminant) / (2 * a);
    if (t1 < 0 && t2 < 0) {
        return 0;
    }
    if (t1 > 0 && t2 > 0) {
        *t = fmin(t1, t2); 
    } else if (t1 > 0) {
        *t = t1; 
    } else if (t2 > 0) {
        *t = t2; 
    }
    return 1;
}
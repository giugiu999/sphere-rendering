#include "assg.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define SHADOW_FACTOR 0.1

// set global variable
int imageWidth, imageHeight;
float viewportHeight, focalLength;
Light light;
Vec3 *colors;
Vec3 backgroundColor;
Sphere *spheres;
int numSpheres;
viewport v;

void initializeViewport() {
    v.height = viewportHeight;
    v.width = (float)imageWidth / imageHeight * viewportHeight;
    v.z = -focalLength;
}

int isInShadow(Vec3 intersectionPoint, Vec3 normal, Light light, Sphere *spheres, int numSpheres) {
    Vec3 lightDir = normalize(subtract(light.position, intersectionPoint));
    Vec3 shadowRayOrigin = add(intersectionPoint, scalarMultiply(0.001, normal));

    for (int i = 0; i < numSpheres; i++) {
        float t;
        if (doesIntersect(&spheres[i], shadowRayOrigin, lightDir, &t)) {
            float lightDistance = length(subtract(light.position, shadowRayOrigin));
            if (t > 0 && t < lightDistance) {
                return 1;
            }
        }
    }
    return 0;
}

void renderPipelineFS(int imageWidth, int imageHeight, Vec3 rayPos, Sphere *spheres, int numSpheres, Light light, Vec3 backgroundColor, const char *outputFilePath) {
    FILE *outputFile = fopen(outputFilePath, "w");
    if (!outputFile) {
        perror("Error opening output file");
        exit(1);
    }

    fprintf(outputFile, "P3\n%d %d\n255\n", imageWidth, imageHeight);

    int samples = 3; // 3x3 抗锯齿采样
    for (int j = 0; j < imageHeight; j++) {
        for (int i = 0; i < imageWidth; i++) {
            Vec3 totalColor = {0.0f, 0.0f, 0.0f};
            for (int sy = 0; sy < samples; sy++) {
                for (int sx = 0; sx < samples; sx++) {
                    float u = ((float)i + (float)(sx + 0.5f) / samples) / imageWidth;
                    float a = ((float)j + (float)(sy + 0.5f) / samples) / imageHeight;

                    Vec3 viewportPoint = {
                        (u - 0.5f) * v.width,
                        (0.5f - a) * v.height,
                        v.z
                    };
                    Vec3 rayDir = normalize(viewportPoint);

                    float closestT = INFINITY;
                    Sphere *closestSphere = NULL;

                    for (int k = 0; k < numSpheres; k++) {
                        float t;
                        if (doesIntersect(&spheres[k], rayPos, rayDir, &t)) {
                            if (t < closestT) {
                                closestT = t;
                                closestSphere = &spheres[k];
                            }
                        }
                    }

                    Vec3 sampleColor = backgroundColor;
                    if (closestSphere) {
                        Vec3 intersectionPoint = add(rayPos, scalarMultiply(closestT, rayDir));
                        Vec3 normal = normalize(subtract(intersectionPoint, closestSphere->pos));
                        Vec3 lightDir = normalize(subtract(light.position, intersectionPoint));

                        float intensity = (light.brightness * fmax(0.0f, dot(normal, lightDir))) /
                                          pow(length(subtract(light.position, intersectionPoint)), 2);
                        intensity = fmin(1.0f, intensity);

                        if (isInShadow(intersectionPoint, normal, light, spheres, numSpheres)) {
                            intensity *= SHADOW_FACTOR;
                        }

                        sampleColor = scalarMultiply(intensity, closestSphere->color);
                    }

                    totalColor = add(totalColor, sampleColor);
                }
            }

            Vec3 finalColor = scalarDivide(totalColor, samples * samples);
            writeColour(outputFile, finalColor);
        }
        fprintf(outputFile, "\n");
    }

    fclose(outputFile);
}
void read_files(const char *file_path) {
    FILE *file = fopen(file_path, "r");
    if (!file) {
        perror("Error opening file");
        exit(1);
    }

    fscanf(file, "%d %d", &imageWidth, &imageHeight);
    fscanf(file, "%f", &viewportHeight);
    fscanf(file, "%f", &focalLength);

    fscanf(file, "%f %f %f %f", &light.position.x, &light.position.y, &light.position.z, &light.brightness);

    int numColors;
    fscanf(file, "%d", &numColors);
    colors = (Vec3 *)malloc(numColors * sizeof(Vec3));
    unsigned int *hexColors = (unsigned int *)malloc(numColors * sizeof(unsigned int));
    for (int i = 0; i < numColors; i++) {
        fscanf(file, "%x", &hexColors[i]); // HEX 格式
        colors[i] = unpackRGB(hexColors[i]);
    }
    qsort(hexColors, numColors, sizeof(unsigned int), compareColor);
    for (int i = 0; i < numColors; i++) {
        colors[i] = unpackRGB(hexColors[i]);
    }
    free(hexColors);
    int backgroundColorIndex;
    fscanf(file, "%d", &backgroundColorIndex);
    backgroundColor = colors[backgroundColorIndex];
    fscanf(file, "%d", &numSpheres);
    spheres = (Sphere *)malloc(numSpheres * sizeof(Sphere));
    for (int i = 0; i < numSpheres; i++) {
        Vec3 position;
        float radius;
        int colorIndex;
        fscanf(file, "%f %f %f %f %d", &position.x, &position.y, &position.z, &radius, &colorIndex);
        spheres[i] = (Sphere){radius, position, colors[colorIndex]};
    }

    fclose(file);
}

void renderPipeline(int imageWidth, int imageHeight, Vec3 rayPos, Sphere *spheres, int numSpheres, Light light, Vec3 backgroundColor, const char *outputFilePath) {
    FILE *outputFile = fopen(outputFilePath, "w");
    if (!outputFile) {
        perror("Error opening output file");
        exit(1);
    }
    fprintf(outputFile, "P3\n%d %d\n255\n", imageWidth, imageHeight);

    int samples = 3; // 3 by 3 sample
    for (int j = 0; j < imageHeight; j++) {
        for (int i = 0; i < imageWidth; i++) {
            Vec3 totalColor = {0.0, 0.0, 0.0};
            for (int sy = 0; sy < samples; sy++) {
                for (int sx = 0; sx < samples; sx++) {
                    float u = ((float)i + (float)(sx + 0.5) / samples) / imageWidth;
                    float a = ((float)j + (float)(sy + 0.5) / samples) / imageHeight;
                    Vec3 viewportPoint = {
                        (u - 0.5) * v.width,
                        (0.5 - a) * v.height,
                        v.z
                    };
                    Vec3 rayDir = normalize(viewportPoint);
                    float closestT = INFINITY;
                    Sphere *closestSphere = NULL;
                    for (int k = 0; k < numSpheres; k++) {
                        float t;
                        if (doesIntersect(&spheres[k], rayPos, rayDir, &t)) {
                            if (t < closestT) {
                                closestT = t;
                                closestSphere = &spheres[k];
                            }
                        }
                    }
                    Vec3 sampleColor;
                    if (closestSphere) {
                        Vec3 intersectionPoint = add(rayPos, scalarMultiply(closestT, rayDir));
                        Vec3 normal = normalize(subtract(intersectionPoint, closestSphere->pos));
                        Vec3 lightDir = normalize(subtract(light.position, intersectionPoint));
                        // lighting intensity
                        float intensity = (light.brightness * fmax(0.0, dot(normal, lightDir))) /
                                          (length(subtract(light.position, intersectionPoint)) * length(subtract(light.position, intersectionPoint)));
                        intensity = fmin(1.0, intensity); // max 1
                        if (isInShadow(intersectionPoint, normal, light, spheres, numSpheres)) {
                            intensity *= SHADOW_FACTOR;
                        }
                        sampleColor = (Vec3){intensity, intensity, intensity}; // intersection
                    } else { 
                        sampleColor = (Vec3){0.0, 0.0, 0.0}; // set background to black
                    }
                    totalColor = add(totalColor, sampleColor);
                }
            }
            // avg color
            Vec3 pixelColor = scalarDivide(totalColor, samples * samples);
            int grayValue = (int)(pixelColor.x * 255); 
            fprintf(outputFile, "%d %d %d ", grayValue, grayValue, grayValue);
        }
        fprintf(outputFile, "\n");
    }
    fclose(outputFile);
}

void read_filesMS1(const char *file_path) {
    FILE *file = fopen(file_path, "r");
    if (!file) {
        perror("Error opening file");
        exit(1);
    }
    fscanf(file, "%d %d", &imageWidth, &imageHeight);
    fscanf(file, "%f", &viewportHeight);
    fscanf(file, "%f", &focalLength);
    v.width = viewportHeight * imageWidth / imageHeight;
    fscanf(file, "%f %f %f %f", &light.position.x, &light.position.y, &light.position.z, &light.brightness);
    // ms1 default color and bgcolor
    int numColors;
    fscanf(file, "%d", &numColors);
    for (int i = 0; i < numColors; i++) {
        unsigned int hexColor;
        fscanf(file, "%x", &hexColor);
    }
    int backgroundColorIndex;
    fscanf(file, "%d", &backgroundColorIndex);
    fscanf(file, "%d", &numSpheres);
    spheres = (Sphere *)malloc(numSpheres * sizeof(Sphere));
    for (int i = 0; i < numSpheres; i++) {
        Vec3 position;
        float radius;
        int colorIndex;
        fscanf(file, "%f %f %f %f %d", &position.x, &position.y, &position.z, &radius, &colorIndex);
        spheres[i] = (Sphere){radius,position,(Vec3){1.0, 1.0, 1.0}}; // default white
    }
    fclose(file);
}

void output(const char *ofilePath) {
    FILE *outputFile = fopen(ofilePath, "w");
    // add
    Vec3 resultV = add(backgroundColor, light.position);
    fprintf(outputFile, "(%.1f, %.1f, %.1f) + (%.1f, %.1f, %.1f) = (%.1f, %.1f, %.1f)\n",
            backgroundColor.x, backgroundColor.y, backgroundColor.z,
            light.position.x, light.position.y, light.position.z,
            resultV.x, resultV.y, resultV.z);
    //subtract
    resultV = subtract(backgroundColor, light.position);
    fprintf(outputFile, "(%.1f, %.1f, %.1f) - (%.1f, %.1f, %.1f) = (%.1f, %.1f, %.1f)\n",
            backgroundColor.x, backgroundColor.y, backgroundColor.z,
            light.position.x, light.position.y, light.position.z,
            resultV.x, resultV.y, resultV.z);
    //scalarMultiply
    resultV = scalarMultiply(v.width, light.position);
    fprintf(outputFile, "%.1f * (%.1f, %.1f, %.1f) = (%.1f, %.1f, %.1f)\n",
            v.width, light.position.x, light.position.y, light.position.z,
            resultV.x, resultV.y, resultV.z);
    // normalize
    resultV = normalize(light.position);
    fprintf(outputFile, "normalize(%.1f, %.1f, %.1f) = (%.1f, %.1f, %.1f)\n",
            light.position.x, light.position.y, light.position.z,
            resultV.x, resultV.y, resultV.z);
    // calculate&output (.1f)
    for (int i = 0; i < numSpheres; i++) {
        Sphere sphere = spheres[i];
        fprintf(outputFile, "\n");
        // scalarDivide
        resultV = scalarDivide(sphere.color, sphere.r);
        fprintf(outputFile, "(%.1f, %.1f, %.1f) / %.1f = (%.1f, %.1f, %.1f)\n",
                sphere.color.x, sphere.color.y, sphere.color.z, sphere.r,
                resultV.x, resultV.y, resultV.z);
        // dot
        float result = dot(light.position, sphere.pos);
        fprintf(outputFile, "dot((%.1f, %.1f, %.1f), (%.1f, %.1f, %.1f)) = %.1f\n",
                light.position.x, light.position.y, light.position.z,
                sphere.pos.x, sphere.pos.y, sphere.pos.z,
                result);
        // distance
        result = distance(light.position, sphere.pos);
        fprintf(outputFile, "distance((%.1f, %.1f, %.1f), (%.1f, %.1f, %.1f)) = %.1f\n",
                light.position.x, light.position.y, light.position.z,
                sphere.pos.x, sphere.pos.y, sphere.pos.z,
                result);
        // length
        result = length(sphere.pos);
        fprintf(outputFile, "length(%.1f, %.1f, %.1f) = %.1f\n",
                sphere.pos.x, sphere.pos.y, sphere.pos.z,
                result);
    }
    fclose(outputFile);
}

int main(int argc, char *argv[]) {
    const char *inputFilePath = argv[1];
    const char *outputFilePath = argv[2];
    #ifdef MS1
    read_filesMS1(inputFilePath);
    output(outputFilePath);
    #endif
    #ifdef MS2
    read_files(inputFilePath);
    initializeViewport();
    renderPipeline(imageWidth, imageHeight, (Vec3){0, 0, 0}, spheres, numSpheres, light, backgroundColor, outputFilePath);
    #endif
    #ifdef FS
    read_files(inputFilePath);
    initializeViewport();
    renderPipelineFS(imageWidth, imageHeight, (Vec3){0, 0, 0}, spheres, numSpheres, light, backgroundColor, outputFilePath);
    #endif
    free(colors);
    free(spheres);
    return 0;
}